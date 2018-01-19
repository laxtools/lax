#include "stdafx.h"

#include <lax/net/session.hpp>
#include <lax/net/service.hpp>
#include <lax/net/protocol/protocol_factory.hpp>
#include <lax/util/logger.hpp>
#include <lax/util/exception.hpp>
#include <spdlog/fmt/fmt.h>

namespace lax
{
namespace net
{

channel::channel session::channel_("session");

session::session(const id& id, tcp::socket&& soc, bool accepted, const std::string& protocol)
	: socket_(std::move(soc))
	, id_(id)
	, accepted_(accepted)
{
	remote_addr_ = fmt::format(
		"{0}:{1}",
		socket_.remote_endpoint().address().to_string(),
		socket_.remote_endpoint().port()
	);

	local_addr_ = fmt::format(
		"{0}:{1}",
		socket_.local_endpoint().address().to_string(),
		socket_.local_endpoint().port()
	);

	desc_ = fmt::format(
		"session/{0}/{1}", 
		get_id().get_index(), 
		remote_addr_
	);

	util::log()->info(
		"{2}. local: {0} remote: {1}",
		local_addr_, 
		remote_addr_, 
		accepted_ ? "accepted" : "connected"
	);

	protocol_ = protocol_factory::inst().create(protocol);

	if (!protocol_)
	{
		THROW("fail to create protocol");

		return;
	}

	protocol_->bind(this);

	request_recv();
}

session::~session()
{
	close();

	util::log()->info("bye! {0}", get_desc());
}

session::key_t session::sub(
	const message::topic_t& topic,
	cond_t cond,
	cb_t cb
)
{
	return channel_.subscribe(topic, cond, cb, channel::sub::mode::immediate);
}

session::key_t session::sub(
	const message::topic_t& topic,
	cb_t cb
)
{
	return channel_.subscribe(topic, cb, channel::sub::mode::immediate);
}

bool session::unsub(key_t key)
{
	return channel_.unsubscribe(key);
}

void session::push(message::ptr m)
{
	channel_.push(m);
}

session::result session::send(message::ptr m)
{
	return protocol_->send(m);
}

void session::close()
{
	if (socket_.is_open())
	{
		asio::error_code ec; 

		socket_.shutdown(socket_.shutdown_both, ec);
		socket_.close(ec);
	}
}

session::result session::send(uint8_t* data, std::size_t len)
{
	return_if(!socket_.is_open(), result(false, reason::fail_socket_closed));

	// append 
	{
		std::lock_guard<std::recursive_mutex> lock(send_segs_mutex_);
		send_buffer_.append(data, len);
	}

	return request_send();
}

void session::error(const asio::error_code& ec)
{
	util::log()->warn(
		"session error. id: {0}, reason: {1}",
		get_id().get_value(), 
		ec.message()
	);

	util::log()->flush();

	last_error_ = ec;

	close();

	if (!sending_ && !recving_) // wait both if any 
	{
		(void)protocol_->on_error(ec);

		service::inst().error(get_id());
	}
}

session::result session::request_recv()
{
	return_if(!is_open(), result(false, reason::fail_socket_closed));

	// check recv
	{
		std::lock_guard<std::recursive_mutex> lock(session_mutex_);

		check(!recving_);

		if (recving_)
		{
			// exception 수준임. 
			return result(false, reason::fail_session_already_recving);
		}

		recving_ = true;
	}

	// request recv. 한번에 하나만 읽고 위에서 막히므로 락 필요 없음
	socket_.async_read_some(
		asio::buffer(recv_buf_.data(), recv_buf_.size()),
		[this](asio::error_code ec, std::size_t len) { this->on_recv_completed(ec, len); }
	);

	return result(true, reason::success);
}

session::result session::request_send()
{
	return_if(!is_open(), result(false, reason::fail_socket_closed));

	// check send
	{
		std::lock_guard<std::recursive_mutex> lock(session_mutex_);

		if (sending_)
		{
			return result(true, reason::success_session_already_sending);
		}

		// check data available
		{
			std::lock_guard<std::recursive_mutex> lock(send_segs_mutex_);

			if (send_buffer_.size() == 0)
			{
				return result(true, reason::success_session_no_data_to_send);
			}
		}

		sending_ = true;
	}

	// get bufs and send. 한번에 하나만 보내고 위에서 막히므로 send_segs_mutex_만 사용
	{
		std::lock_guard<std::recursive_mutex> lock(send_segs_mutex_);

		check(send_buffer_.size() > 0);
		check(sending_segs_.empty());
		check(sending_bufs_.empty());

		sending_bufs_.clear();
		sending_segs_.clear();

		// rvo and move will be fine
		sending_segs_ = send_buffer_.transfer();
	}

	check(send_buffer_.size() == 0);
	check(!sending_segs_.empty());

	send_request_size_ = 0;

	for (auto& b : sending_segs_)
	{
		sending_bufs_.push_back(asio::buffer(b->get().data(), b->get().size()));
		send_request_size_ += b->get().size();
	}

	check(!sending_bufs_.empty());
	check(send_request_size_ > 0);

	asio::async_write(
		socket_,
		sending_bufs_,
		[this](asio::error_code ec, std::size_t len)
	{
		this->on_send_completed(ec, len);
	});

	return result(true, reason::success);
}

void session::on_recv_completed(asio::error_code& ec, std::size_t len)
{
	// make recv false
	{
		std::lock_guard<std::recursive_mutex> lock(session_mutex_);
		check(recving_);
		recving_ = false;
	}

	if (!ec)
	{
		check(len > 0);

		auto rc = protocol_->on_recv(recv_buf_.data(), len);

		if (rc)
		{
			request_recv();
		}
		else
		{
			util::log()->info(
				"on_recv() protocol error. id: {0}, reason: {1}",
				get_id().get_value(),
				rc.value
			);

			error(ec);
		}
	}
	else
	{
		error(ec);
	}
}

void session::on_send_completed(asio::error_code& ec, std::size_t len)
{
	// release segs before try sending again
	{
		std::lock_guard<std::recursive_mutex> lock(send_segs_mutex_);
		check(!sending_segs_.empty());

		if (!ec)
		{
			check(send_request_size_ == len);
		}

		send_buffer_.release(sending_segs_);
		sending_segs_.clear();
		sending_bufs_.clear();
		send_request_size_ = 0;
	}

	// make send false
	{
		std::lock_guard<std::recursive_mutex> lock(session_mutex_);
		check(sending_);
		sending_ = false;
	}

	if (!ec)
	{
		request_send();
	}
	else
	{
		error(ec);
	}
}

} // net
} // lax

#include "stdafx.h"

#include <lax/net/session.hpp>
#include <lax/net/service.hpp>
#include <lax/net/protocol/protocol_factory.hpp>
#include <lax/net/protocol/sys/sys_messages.hpp>
#include <lax/util/logger.hpp>
#include <lax/util/exception.hpp>
#include <spdlog/fmt/fmt.h>

namespace lax
{
namespace net
{

channel::channel session::channel_("session");
subs_close session::subs_close_;

session::session(const id& id, tcp::socket&& soc, bool accepted, const std::string& protocol)
	: socket_(std::move(soc))
	, id_(id)
	, accepted_(accepted)
{
	// �ٸ� ���� �������� ���� ���¸� ����
	// thread-unsafe�� �͵� ���� ����. 
	// service �Լ��� ȣ�� ����. (�� �̽��� �߻� ����)

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

	util::log()->debug( "{0} protocol bound", get_desc() );

	notify_session_ready();

	request_recv();
}

session::~session()
{
	close();

	util::log()->info("bye! {0}", get_desc());
}

session::key_t session::sub(
	const packet::topic_t& topic,
	cond_t cond,
	cb_t cb
)
{
	return channel_.subscribe(topic, cond, cb, channel::sub::mode::immediate);
}

session::key_t session::sub(
	const packet::topic_t& topic,
	cb_t cb
)
{
	util::log()->debug(
		"subscribe: {0}:{1}", 
		topic.get_group(), 
		topic.get_type()
	);

	return channel_.subscribe(topic, cb, channel::sub::mode::immediate);
}

bool session::unsub(key_t key)
{
	return channel_.unsubscribe(key);
}

void session::post(packet::ptr m)
{
	channel_.post(m);
}

session::key_t session::sub_close(
	subs_close::sid_t id,
	subs_close::cb_t cb
)
{
	return subs_close_.subscribe(id, cb);
}

void session::unsub_close(key_t key)
{
	subs_close_.unsubscribe(key);
}

session::result session::send(packet::ptr m)
{
	return protocol_->send(m);
}

session::result session::send(packet::ptr m, const uint8_t* const data, std::size_t len)
{
	return protocol_->send(m, data, len);
}

void session::close()
{
	// ������ ��ȿ�ϸ� ���� �Ҹ��� �õ��Ѵ�. 

	RETURN_IF(destroyed_);

	// close
	{
		if (socket_.is_open())
		{
			asio::error_code ec;

			socket_.shutdown(socket_.shutdown_both, ec);
			socket_.close(ec);

			util::log()->debug("{0} close", get_desc());
		}
	}

	// socket is closed. 
	// VERIFY for destroy

	destroy();
}

session::result session::send(const uint8_t* const data, std::size_t len)
{
	RETURN_IF(!socket_.is_open(), result(false, reason::fail_socket_closed));

	// append 
	{
		std::lock_guard<std::recursive_mutex> lock(send_segs_mutex_);
		send_buffer_.append(data, len);
	}

	return request_send();
}

void session::error(const asio::error_code& ec)
{
	util::log()->info(
		"{0} error. reason: {1}",
		get_desc(), 
		ec.message()
	);

	util::log()->flush();

	close();		// will call destroy
}

void session::destroy(const asio::error_code& ec)
{
	// �Ҹ��� �����ϸ� �Ҹ��� ó���Ѵ�. 
	// - app -> close -> destroy �Ǵ�
	// - recv_comp -> error -> close -> destroy �Ǵ� 
	// - send_comp -> error -> close -> destroy �̴�

	// VERIFY
	{
		std::lock_guard<std::recursive_mutex> lock(session_mutex_);

		if (sending_ || recving_) // wait both if any 
		{
			return;
		}
	}

	util::log()->debug("{0} destroying...", get_desc());

	// notify
	destroyed_ = true;

	(void)protocol_->on_error(ec);

	notify_session_closed(ec);

	auto desc = get_desc();

	service::inst().error(get_id());

	util::log()->debug("{0} destroyed", desc);
}

session::result session::request_recv()
{
	RETURN_IF(!is_open(), result(false, reason::fail_socket_closed));

	// VERIFY recv
	{
		std::lock_guard<std::recursive_mutex> lock(session_mutex_);

		VERIFY(!recving_);

		if (recving_)
		{
			// exception ������. 
			return result(false, reason::fail_session_already_recving);
		}

		recving_ = true;
	}

	util::log()->debug( "{} request recv", get_desc() );

	// request recv. �ѹ��� �ϳ��� �а� ������ �����Ƿ� �� �ʿ� ����
	socket_.async_read_some(
		asio::buffer(recv_buf_.data(), recv_buf_.size()),
		[this](asio::error_code ec, std::size_t len) { this->on_recv_completed(ec, len); }
	);

	return result(true, reason::success);
}

session::result session::request_send()
{
	RETURN_IF(!is_open(), result(false, reason::fail_socket_closed));

	// VERIFY send
	{
		std::lock_guard<std::recursive_mutex> lock(session_mutex_);

		if (sending_)
		{
			return result(true, reason::success_session_already_sending);
		}

		// VERIFY data available
		{
			std::lock_guard<std::recursive_mutex> lock(send_segs_mutex_);

			if (send_buffer_.size() == 0)
			{
				return result(true, reason::success_session_no_data_to_send);
			}
		}

		sending_ = true;
	}

	// get bufs and send. �ѹ��� �ϳ��� ������ ������ �����Ƿ� send_segs_mutex_�� ���
	{
		std::lock_guard<std::recursive_mutex> lock(send_segs_mutex_);

		VERIFY(send_buffer_.size() > 0);
		VERIFY(sending_segs_.empty());
		VERIFY(sending_bufs_.empty());

		sending_bufs_.clear();
		sending_segs_.clear();

		// rvo and move will be fine
		sending_segs_ = send_buffer_.transfer();
	}

	VERIFY(send_buffer_.size() == 0);
	VERIFY(!sending_segs_.empty());

	send_request_size_ = 0;

	for (auto& b : sending_segs_)
	{
		sending_bufs_.push_back(asio::buffer(b->get().data(), b->get().size()));
		send_request_size_ += b->get().size();
	}

	VERIFY(!sending_bufs_.empty());
	VERIFY(send_request_size_ > 0);

	util::log()->debug( 
		"{0} request send. {1} bytes", 
		get_desc(), 
		send_request_size_ 
	);

	asio::async_write(
		socket_,
		sending_bufs_,
		[this](asio::error_code ec, std::size_t len) {
		this->on_send_completed(ec, len);
	});

	return result(true, reason::success);
}

void session::on_recv_completed(asio::error_code& ec, std::size_t len)
{
	// make recv false
	{
		std::lock_guard<std::recursive_mutex> lock(session_mutex_);
		VERIFY(recving_);
		recving_ = false;
	}

	if (!ec)
	{
		VERIFY(len > 0);

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
		VERIFY(!sending_segs_.empty());

		if (!ec)
		{
			VERIFY(send_request_size_ == len);
		}

		send_buffer_.release(sending_segs_);
		sending_segs_.clear();
		sending_bufs_.clear();
		send_request_size_ = 0;
	}

	// make send false
	{
		std::lock_guard<std::recursive_mutex> lock(session_mutex_);
		VERIFY(sending_);
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

void session::notify_session_ready()
{
	auto mp = std::make_shared<sys_session_ready>();
	mp->id = get_id().get_value();

	post(mp); // send to channel
}

void session::notify_session_closed(const asio::error_code& ec)
{
	// destroy���� ȣ��

	auto mp = std::make_shared<sys_session_closed>();
	mp->id = get_id().get_value();
	mp->ec = ec;

	post(mp); // send to channel

	subs_close_.post(get_id().get_value(), ec); // send to refs
}

} // net
} // lax

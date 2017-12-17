#include "stdafx.h"
#include "service_impl.h"
#include <lax/net/exception.h>

namespace lax
{
namespace net
{

service_impl::service_impl(app* ap, service& svc)
	: app_(ap)
	, svc_(svc)
	, ios_()
	, id_gen_(1, MAXUINT16)
	, seq_gen_(1, MAXUINT16)
{
	check(app_);

	start(std::thread::hardware_concurrency());
}

service_impl::~service_impl()
{
	// service calls stop on destruction
}

service::result service_impl::listen(const std::string& addr, service::creator& c)
{
	std::unique_lock<std::shared_timed_mutex> lock(mutex_);

	auto id = id_gen_.next();
	auto ptr = std::make_shared<acceptor>(&svc_, id, c, addr);
	auto rc = ptr->listen();

	if (rc)
	{
		acceptors_[id] = ptr;
	}

	return rc;
}

service::result service_impl::connect(const std::string& addr, service::creator& c)
{
	std::unique_lock<std::shared_timed_mutex> lock(mutex_);

	auto id = id_gen_.next();
	auto ptr = std::make_shared<connector>(&svc_, id, c, addr);
	auto rc = ptr->connect();

	if (rc)
	{
		connectors_[id] = ptr;
	}

	return rc;
}

service::result service_impl::send(const session::id& id, uint8_t* data, std::size_t len)
{
	auto ptr = get(id);

	return_if(!ptr, service::result(false, reason::fail_invalid_session_index));

	return ptr->send(data, len);
}

void service_impl::close(const session::id& id)
{
	auto ptr = get(id);

	return_if(!ptr);

	ptr->close();
}

void service_impl::error(const session::id& id)
{
	util::log_helper::get()->info(
		"{0} remove on error. id: {1}, reason: {2}",
		__FUNCTION__,
		id.get_value()
	);

	std::unique_lock<std::shared_timed_mutex> lock(mutex_);

	auto idx = id.get_index();

	return_if(idx >= sessions_.size());

	sessions_[idx].reset();

	free_slots_.push_back(idx);

	seq_gen_.release(id.get_seq());
}

session::ptr service_impl::get(const session::id& id)
{
	std::shared_lock<std::shared_timed_mutex> lock(mutex_);

	auto idx = id.get_index();

	return_if(idx >= sessions_.size(), session::ptr());

	auto ptr = sessions_[idx];

	if (ptr->get_id() == id)
	{
		return ptr;
	}

	return session::ptr();
}

void service_impl::on_accepted(key k, tcp::socket&& soc)
{
	session::ptr session;

	// unique
	{
		std::unique_lock<std::shared_timed_mutex> lock(mutex_);

		uint32_t seq = 0;
		uint16_t slot = 0;

		auto iter = acceptors_.find(k);

		if (iter == acceptors_.end())
		{
			soc.close();
			return;
		}

		seq = get_next_seq();
		slot = get_free_slot();

		session = iter->second->create(session::id(seq, slot), std::move(soc));

		sessions_[session->get_id().get_index()] = session;
	}

	// 락 밖에서 호출
	session->created();
}

void service_impl::on_accept_failed(key k)
{
	// log 
}

void service_impl::on_connected(key k, tcp::socket&& soc)
{
	session::ptr session;

	// unique
	{
		std::unique_lock<std::shared_timed_mutex> lock(mutex_);

		uint32_t seq = 0;
		uint16_t slot = 0;

		auto iter = connectors_.find(k);

		if (iter == connectors_.end())
		{
			soc.close();
			return;
		}

		seq = get_next_seq();
		slot = get_free_slot();

		session = iter->second->create(session::id(seq, slot), std::move(soc));

		sessions_[session->get_id().get_index()] = session;

		connectors_.erase(k);
		id_gen_.release(k);
	}

	// 락 밖에서 호출
	session->created();
}

void service_impl::on_connect_failed(key k)
{
	std::string addr;

	// shared
	{
		std::shared_lock<std::shared_timed_mutex> lock(mutex_);

		auto iter = connectors_.find(k);

		if (iter != connectors_.end())
		{
			addr = iter->second->get_addr().get_raw();
		}
	}

	// exclusive 
	{
		std::unique_lock<std::shared_timed_mutex> lock(mutex_);

		connectors_.erase(k);

		id_gen_.release(k);
	}

	// 락 밖에서 호출
	if (!addr.empty())
	{
		app_->on_connect_failed(addr);
	}
}

void service_impl::start(unsigned int n)
{
	check(n > 0);
	check(stop_);

	stop_ = false;

	threads_.resize(n);

	for (unsigned int i = 0; i < n; ++i)
	{
		threads_[i].swap(std::thread([this]() {this->run();}));
	}
}

void service_impl::run()
{
	while (!stop_)
	{
		ios_.run();
	}
}

void service_impl::stop()
{
	check(!stop_);

	stop_ = true;

	// post to all threads
	for (auto& t : threads_)
	{
		ios_.stop(); 
	}

	// wait all threads
	for (auto& t : threads_)
	{
		t.join();
	}
}

uint16_t service_impl::get_free_slot()
{
	// 외부에서 exclusive 락 거는 것으로 가정

	check(sessions_.size() < UINT16_MAX);

	if (sessions_.size() >= UINT16_MAX)
	{
		throw exception("session slot limit reached");
	}

	if (free_slots_.empty())
	{
		sessions_.push_back(session::ptr());
		
		return static_cast<uint16_t>(sessions_.size() - 1);
	}

	auto index = free_slots_.front();

	free_slots_.pop_front();

	check(index < sessions_.size());

	return index;
}

uint32_t service_impl::get_next_seq()
{
	return seq_gen_.next();
}

} // net 
} // lax

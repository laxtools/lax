#include "stdafx.h"
#include <lax/net/detail/service_impl.hpp>
#include <lax/util/exception.hpp>
#include <lax/net/protocol/sys/sys_messages.hpp>
#include <lax/net/protocol/protocol_factory.hpp>
#include <lax/net/protocol/bits/bits_protocol.hpp>

namespace lax
{
namespace net
{

service_impl::service_impl(service& svc)
	: svc_(svc)
	, ios_()
	, id_gen_(1, MAXUINT16)
{
}

service_impl::~service_impl()
{
	// fini()�� �ܺο��� ȣ���ؼ� �����Ѵ�. 
	// - unique_ptr�� ������ �� ���μ��� �����鼭 ȣ��ȴ�. 
	// - �� �� �޸𸮰� �����Ǿ� �ִ�. 
	// - �� �׷���????
}

service::result service_impl::listen(const std::string& addr, const std::string& protocol)
{
	if (!protocol_factory::inst().has(protocol))
	{
		util::log()->critical(
			"protocol {0} not implemented or added", protocol
		);

		return service::result(false, reason::fail_protocol_not_added);
	}

	std::unique_lock<std::shared_timed_mutex> lock(mutex_);

	auto id = id_gen_.next();
	auto ptr = std::make_shared<acceptor>(id, protocol, addr);
	auto rc = ptr->listen();

	if (rc)
	{
		acceptors_[id] = ptr;
	}

	return rc;
}

service::result service_impl::connect(const std::string& addr, const std::string& protocol)
{
	if (!protocol_factory::inst().has(protocol))
	{
		util::log()->critical(
			"protocol {0} not implemented or added", protocol
		);

		return service::result(false, reason::fail_protocol_not_added);
	}

	std::unique_lock<std::shared_timed_mutex> lock(mutex_);

	auto id = id_gen_.next();
	auto ptr = std::make_shared<connector>(id, protocol, addr);
	auto rc = ptr->connect();

	if (rc)
	{
		connectors_[id] = ptr;
	}

	return rc;
}

session::ref service_impl::acquire(const session::id& id)
{
	std::shared_lock<std::shared_timed_mutex> lock(mutex_);

	RETURN_IF(id.get_index() > sessions_.size(), session::ref());

	auto& sp = sessions_[id.index_];

	return session::ref(sp.session);
}

void service_impl::error(const session::id& id)
{
	util::log()->info(
		"{0} remove on error. id: {1}/{2}",
		__FUNCTION__,
		id.get_index(), id.get_seq()
	);

	std::unique_lock<std::shared_timed_mutex> lock(mutex_);

	auto idx = id.get_index();

	RETURN_IF(idx >= sessions_.size());

	sessions_[idx].session.reset();

	free_slots_.push_back(idx);

	session_count_--;
}

void service_impl::on_accepted(key k, tcp::socket&& soc)
{
	std::unique_lock<std::shared_timed_mutex> lock(mutex_);

	auto iter = acceptors_.find(k);

	if (iter == acceptors_.end())
	{
		soc.close();
		return;
	}

	on_new_socket(iter->second->get_protocol(), std::move(soc), true);
}

void service_impl::on_accept_failed(key k, const asio::error_code& ec)
{
	acceptor::ptr apt;

	{
		std::shared_lock<std::shared_timed_mutex> lock(mutex_);

		auto iter = acceptors_.find(k);
		if (iter != acceptors_.end())
		{
			apt = iter->second;
		}
	}

	if (apt)
	{
		util::log()->error(
			"failed to accept on protocol:{0}, addr:{1}", 
			apt->get_protocol(), 
			apt->get_addr().get_raw()
		);

		auto mp = std::make_shared<sys_accept_failed>();
		mp->addr = apt->get_addr().get_raw();
		mp->ec = ec;

		session::post(mp);
	}
}

void service_impl::on_connected(key k, tcp::socket&& soc)
{
	std::unique_lock<std::shared_timed_mutex> lock(mutex_);

	auto iter = connectors_.find(k);

	if (iter == connectors_.end())
	{
		soc.close();
		return;
	}

	on_new_socket(iter->second->get_protocol(), std::move(soc), false);

	connectors_.erase(iter);
}

void service_impl::on_connect_failed(key k, const asio::error_code& ec)
{
	connector::ptr cnt;

	// shared
	{
		std::shared_lock<std::shared_timed_mutex> lock(mutex_);

		auto iter = connectors_.find(k);
		if (iter != connectors_.end())
		{
			cnt = iter->second;
		}
	}

	// exclusive 
	{
		std::unique_lock<std::shared_timed_mutex> lock(mutex_);

		connectors_.erase(k);
	}

	if (cnt)
	{
		auto mp = std::make_shared<sys_connect_failed>();
		mp->addr = cnt->get_addr().get_raw();
		mp->ec = ec;

		session::post(mp);
	}
}

bool service_impl::init()
{
	std::unique_lock<std::shared_timed_mutex> lock(mutex_);

	init_protocols();

	auto n = service::cfg.concurreny_level;

	if (service::cfg.use_hardware_concurreny)
	{
		n = std::thread::hardware_concurrency();
	}

	VERIFY(n > 0);
	VERIFY(stop_);
	RETURN_IF(!stop_, true); // alreay running. no effect

	stop_ = false;

	threads_.resize(n);

	for (int i = 0; i < n; ++i)
	{
		threads_[i].swap(std::thread([this]() {this->run();}));
	}

	return true;
}

void service_impl::init_protocols()
{
	protocol_factory::inst().add(
		"bits", 
		[]() { return std::make_shared<bits_protocol>(); }
	);
}

void service_impl::run()
{
	while (!stop_)
	{
		ios_.run();
	}
}

void service_impl::fini()
{
	RETURN_IF(stop_);
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

	cleanup();
}

void service_impl::cleanup()
{
	// �� �ʿ��� ������ �ƴ����� ��������� 
	// �����ϸ� ������ ���� ������ ã�� ����.

	sessions_.clear();
	acceptors_.clear();
	connectors_.clear();
	threads_.clear();
}

void service_impl::on_new_socket(const std::string& protocol, tcp::socket&& soc, bool accepted)
{
	// called w/ a unique lock

	uint16_t slot_idx = slot_idx = get_free_slot();
	uint32_t seq = 0;

	seq = sessions_[slot_idx].seq++;
	seq = (seq == 0 ? sessions_[slot_idx].seq++ : seq);

	VERIFY(seq > 0);

	mutex_.unlock(); // session ������ ����

	// TODO: exception safety?

	auto sp = std::make_shared<session>(
		session::id(seq, slot_idx),
		std::move(soc),
		accepted,
		protocol
	);

	mutex_.lock(); // �ٽ� �� �ʿ�

	sessions_[slot_idx].session = sp;
	session_count_++;

	ENSURE(sessions_[slot_idx].session);
	ENSURE(session_count_ > 0);

	mutex_.unlock();
}

uint16_t service_impl::get_free_slot()
{
	// �ܺο��� unique �� �Ŵ� ������ ����

	VERIFY(sessions_.size() < UINT16_MAX);

	if (sessions_.size() >= UINT16_MAX)
	{
		THROW("session slot limit reached");
	}

	if (free_slots_.empty())
	{
		sessions_.push_back({ session::ptr(), 0});
		
		return static_cast<uint16_t>(sessions_.size() - 1);
	}

	auto index = free_slots_.front();

	free_slots_.pop_front();

	VERIFY(index < sessions_.size());

	return index;
}

uint16_t service_impl::get_session_count() const
{
	return session_count_;
}

} // net 
} // lax

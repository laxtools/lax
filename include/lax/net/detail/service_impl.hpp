#pragma once 

#include <lax/net/service.hpp>
#include <lax/net/detail/acceptor.hpp>
#include <lax/net/detail/connector.hpp>
#include <lax/util/sequence.hpp>
#include <lax/util/logger.hpp>

#include <atomic>
#include <map>
#include <shared_mutex>
#include <vector>

namespace lax
{
namespace net
{

/// service implementation
class service_impl
{
private: 
	friend class service; 

	using key = uint16_t;

public: 
	service_impl(service& svc);
	~service_impl();

	/// addr에서 listen. addr은 ip:port 형식. 
	service::result listen(const std::string& addr, const std::string& protocol);

	/// connect to a addr. addr은 ip:port 형식
	service::result connect(const std::string& addr, const std::string& protocol);

	/// acquire ref to a session
	session::ref acquire(const session::id& id);

	/// session에서 에러 발생 시 호출
	void error(const session::id& id);

	/// called when accepted
	void on_accepted(key k, tcp::socket&& soc);

	/// called when accept failed
	void on_accept_failed(key k, const asio::error_code& ec);

	/// called when connected
	void on_connected(key k, tcp::socket&& soc);

	/// called when connect failed
	void on_connect_failed(key k, const asio::error_code& ec);

	/// internal use only
	asio::io_service& get_ios() 
	{
		return ios_;
	}

private: 
	using sequence = util::sequence<uint16_t>;
	using acceptors = std::map<key, acceptor::ptr>;
	using connectors = std::map<key, connector::ptr>;
	using threads = std::vector<std::thread>;

	struct slot
	{
		session::ptr session; 
		uint16_t seq = 0;
	};

	using sessions = std::vector<slot>;

private:
	/// start
	bool init();

	/// add default protocols. bits, .
	void init_protocols();

	/// io_service 쓰레드 함수. android와 같은 경우 별도 쓰레드가 느린 경우 있음
	void run();

	/// io_service 쓰레드 대기. service에서 호출
	void fini();

	/// cleanup all
	void cleanup();

	/// handle new connection. accepted or connected
	void on_new_socket(const std::string& protocol, tcp::socket&& soc, bool accepted);

	/// 가용한 슬롯을 가져옴. 없으면 만들고 추가 
	uint16_t get_free_slot();

	/// get session count. 
	uint16_t get_session_count() const;

private: 
	service&			svc_;
	asio::io_service	ios_;

	std::shared_timed_mutex mutex_;			/// use shared lock for container access. 
											/// recursive lock이 아니므로 주의

	acceptors				acceptors_;
	connectors				connectors_;
	sequence				id_gen_;		/// acceptor와 connector용 

	std::atomic<uint16_t>	seq_ = 0;
	sessions				sessions_;		/// 세션 벡터. push_back만 사용
	std::deque<uint16_t>    free_slots_;	/// 가용한 세션 슬롯들

	std::atomic<bool>		stop_ = true;
	threads					threads_;
	std::shared_ptr<spdlog::logger> logger_;

	uint16_t				session_count_;
};

} // net
} // lax

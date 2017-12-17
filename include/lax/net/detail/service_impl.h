#pragma once 

#include <lax/net/service.h>
#include <lax/net/detail/acceptor.h>
#include <lax/net/detail/connector.h>
#include <lax/util/sequence.h>
#include <lax/util/logger.h>

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
	service_impl(app* ap, service& svc);
	~service_impl();

	/// addr에서 listen. addr은 ip:port 형식. 
	service::result listen(const std::string& addr, service::creator& c);

	/// connect to a addr. addr은 ip:port 형식
	service::result connect(const std::string& addr, service::creator& c);

	/// send to a session
	service::result send(const session::id& id, uint8_t* data, std::size_t len);

	/// close a session 
	void close(const session::id& id);

	/// session에서 에러 발생 시 호출
	void error(const session::id& id);

	/// get session to handle directly
	session::ptr get(const session::id& id);

	/// access app
	app* get_app()
	{
		return app_;
	}

	/// access io_service for implementation
	asio::io_service& get_ios()
	{
		return ios_;
	}

	/// called when accepted
	void on_accepted(key k, tcp::socket&& soc);

	/// called when accept failed
	void on_accept_failed(key k);

	/// called when connected
	void on_connected(key k, tcp::socket&& soc);

	/// called when connect failed
	void on_connect_failed(key k);

private: 
	using sequence = util::sequence<uint16_t>;
	using acceptors = std::map<key, acceptor::ptr>;
	using connectors = std::map<key, connector::ptr>;
	using sessions = std::vector<session::ptr>;
	using threads = std::vector<std::thread>;

private:
	/// io_service 쓰레드 실행
	void start(unsigned int n); 

	/// io_service 쓰레드 함수. android와 같은 경우 별도 쓰레드가 느린 경우 있음
	void run();

	/// io_service 쓰레드 대기. service에서 호출
	void stop();

	/// 가용한 슬롯을 가져옴. 없으면 만들고 추가 
	uint16_t get_free_slot();

	/// 세션 아이디의 시퀀스 부분 가져옴. 
	uint32_t get_next_seq();

private: 
	app*				app_ = nullptr;
	service&			svc_;
	asio::io_service	ios_;

	std::shared_timed_mutex mutex_;			// use shared lock for container access. recursive lock이 아니므로 주의

	acceptors				acceptors_;
	connectors				connectors_;
	sequence				id_gen_;		// acceptor와 connector용 

	sequence				seq_gen_;		// session sequence 용  
	sessions				sessions_;		// 세션 벡터. push_back만 사용
	std::deque<uint16_t>    free_slots_;	// 가용한 세션 슬롯들

	std::atomic<bool>		stop_ = true;
	threads					threads_;
	std::shared_ptr<spdlog::logger> logger_;
};

} // net
} // lax

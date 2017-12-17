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

	/// addr���� listen. addr�� ip:port ����. 
	service::result listen(const std::string& addr, service::creator& c);

	/// connect to a addr. addr�� ip:port ����
	service::result connect(const std::string& addr, service::creator& c);

	/// send to a session
	service::result send(const session::id& id, uint8_t* data, std::size_t len);

	/// close a session 
	void close(const session::id& id);

	/// session���� ���� �߻� �� ȣ��
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
	/// io_service ������ ����
	void start(unsigned int n); 

	/// io_service ������ �Լ�. android�� ���� ��� ���� �����尡 ���� ��� ����
	void run();

	/// io_service ������ ���. service���� ȣ��
	void stop();

	/// ������ ������ ������. ������ ����� �߰� 
	uint16_t get_free_slot();

	/// ���� ���̵��� ������ �κ� ������. 
	uint32_t get_next_seq();

private: 
	app*				app_ = nullptr;
	service&			svc_;
	asio::io_service	ios_;

	std::shared_timed_mutex mutex_;			// use shared lock for container access. recursive lock�� �ƴϹǷ� ����

	acceptors				acceptors_;
	connectors				connectors_;
	sequence				id_gen_;		// acceptor�� connector�� 

	sequence				seq_gen_;		// session sequence ��  
	sessions				sessions_;		// ���� ����. push_back�� ���
	std::deque<uint16_t>    free_slots_;	// ������ ���� ���Ե�

	std::atomic<bool>		stop_ = true;
	threads					threads_;
	std::shared_ptr<spdlog::logger> logger_;
};

} // net
} // lax

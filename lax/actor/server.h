#pragma once 

#include "cluster_actor.h"
#include <lax/net/service.h>
#include <lax/util/simple_timer.h>

namespace lax
{
namespace actor 
{

/// config class for server
struct server_config
{
	/// enable listening within cluster
	bool enable_cluster_listen = true;

	/// cluster listen address 
	std::string cluster_listen_addr = "0.0.0.0:7000";

	/// enable listening for clients
	bool enable_client_listen = false;

	/// client listen address
	std::string client_listen_addr = "0.0.0.0:7001";

	/// enable json protocol 
	bool enable_json_protocol_for_client = false;

	/// json for client 
	std::string client_json_listen_addr = "0.0.0.0:7003";

	/// cluster peers to connect to. 
	std::vector<std::string> cluster_peers;

	/// try to connect when failed after reconnect_interval seconds. 
	float reconnect_interval = 5.f;		

	/// starting encryption key. use to exchange session encryption key
	std::string default_encryption_key;	
};

class server : public cluster_actor, public net::app
{
public:
	server(id aid);
	~server();

	/// start server 
	reason start(const server_config& cfg);

	/// stop server
	void stop();

	/// find a session
	net::session::ptr find(net::session::id id);

	/// 액터와 연결된 세션을 찾음 
	net::session::ptr find(actor::id id);

	/// 특정 연결 끊기
	void close(net::session::id id);

protected:

	virtual reason send_direct(net::session::id sid, net::msg::ptr m) override;

	/// started. process further 
	/** 
	 * net::service 생성되고 호출됨. 
	 * - 프로토콜 관련 등록 
	 */
	virtual bool on_started();

	/// 주기적인 처리를 실행
	/** 
	 * 재접속 처리 등 
	 */
	void on_process() override;

	/// stopped. process further 
	/**
	 * 외부에서 stop 호출. net::service 소멸되고 호출됨
	 */
	virtual void on_stopped();

	/// session is ready
	/** 
	 * 세션 준비됨. local endpoint, remote endpoint 보고 추가 분류 및 처리.
	 */
	void on_ready(const net::session::id& id) override;

	/// called when session has a message to handle
	/** 
	 * push로 전달. postbox에 연결되면 바로 전송하고 로컬 처리 안 함
	 */
	void on_recv(const net::session::id& id, net::msg::ptr m) override;

	/// called when session is closed or error happened
	/** 
	 * msg_disconnected push 
	 */
	void on_error(const net::session::id& id, const asio::error_code& ec) override;

	/// called when accept failed
	/** 
	 * 특별히 처리 할 수 있는 게 있는가? 
	 */
	void on_accept_failed(const std::string& addr) override;

	/// called when connect failed  
	/** 
	 * 주소 보고 재접속 시도 등을 처리
	 */
	void on_connect_failed(const std::string& addr) override;

	/// build desc string
	void build_desc() override;

private:
	/// listen on cluster / client ports
	bool listen_on_ports();

	/// start connecting to peers
	bool connect_to_peers();

	/// msg_pack protocol session creator
	static net::session::ptr msg_pack_creator(
		net::service& svc,
		net::session::id id,
		tcp::socket&& soc,
		bool accepted);

	/// json protocol session creator
	static net::session::ptr json_creator(
		net::service& svc,
		net::session::id id,
		tcp::socket&& soc,
		bool accepted);

protected: 
	struct connect_peer
	{
		std::string addr;
		std::string protocol;
		net::session::id id;

		bool is_open = false;
		bool connecting = false;
		float last_retry_tick = 0.f;
		int close_count = 0;
		int retry_count = 0;
	};
	using peer_map = std::map<std::string, connect_peer>;

private:
	server_config cfg_;
	std::unique_ptr<net::service> svc_;

	peer_map peers_;
	util::simple_timer timer_;
};


} // actor 
} // lax

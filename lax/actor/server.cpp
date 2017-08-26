#include "stdafx.h"
#include "server.h"
#include "actor_roles.h"
#include <lax/net/protocol/msgpack/session_pack.h>
#include <lax/net/protocol/json/session_json.h>
#include <lax/util/logger.h>

namespace lax
{
namespace actor
{

server::server(id aid)
	: cluster_actor(actor::ptr(), aid, actor_roles::role_server)
{
}

server::~server()
{

}

reason server::start(const server_config& cfg)
{
	cfg_ = cfg; 

	svc_ = std::make_unique<net::service>(this);

	(void)process(timer_.elapsed()); // set current tick

	return_if(!on_started(), reason::fail_server_start);

	return_if(!listen_on_ports(), reason::fail_to_listen);

	return_if(!connect_to_peers(), reason::fail_to_connect);

	return reason::success;
}

net::session::ptr server::find(net::session::id id)
{
	return svc_->get(id);
}

net::session::ptr server::find(actor::id id)
{
	auto ptr = get_dir().find(id);
	return_if(!ptr, net::session::ptr());

	return find(ptr->get_session_id());
}

void server::close(net::session::id id)
{
	svc_->close(id);
}

void server::stop()
{
	svc_.reset();

	on_stopped();
}

reason server::send_direct(net::session::id sid, net::msg::ptr m)
{
	// session_binder::send_proto 기능만 사용 
	// TODO: log 친화적으로 변경

	auto ptr = svc_->get(sid);
	return_if(!ptr, reason::fail_null_pointer);

	return get_session_binder().send_proto(ptr, m).value;
}

bool server::on_started()
{
	return true;
}

void server::on_process()
{
	cluster_actor::on_process();

	// 나의일..
}

void server::on_stopped()
{

}

void server::on_ready(const net::session::id& id)
{
	//
	// 분류해서 클러스터 쪽 연결인 지 확인
	// 구성 파일과 비교해서 처리
	// 클라 연결이면 ref 등록 
	// 
}

void server::on_recv(const net::session::id& id, net::msg::ptr m)
{
	// get protocol function 
	// process

	switch (m->get_protocol())
	{
		//
		// 변환해서 처리한다.
		// msg_pack_actor, msg_json_actor
		// 타겟을 찾아서 처리
		// 없으면 내가 처리 
		// 타겟은 ref를 통해서만 전파된다. 
		// cluster_actor들만 ref를 통해서 전파된다.
		// 
	}
}

void server::on_error(const net::session::id& id, const asio::error_code& ec)
{
	// TODO: make a local message and handles later

	auto ptr = find(id);

	return_if(!ptr);

	// TODO: create a local message and push

	if (!ptr->is_accepted())
	{
		on_connect_failed(ptr->get_remote_addr());
	}
}

void server::on_accept_failed(const std::string& addr)
{
	util::log()->error("server. accept failed on {0}", addr);
}

void server::on_connect_failed(const std::string& addr)
{
	// TODO: make a local message and handles later

	auto& iter = peers_.find(addr);

	if (iter == peers_.end())
	{
		util::log()->warn("server. peer not found. addr: {0}", addr);
		return;
	}

	iter->second.is_open = false;
	iter->second.close_count++;
	iter->second.last_retry_tick = 0; // make it retry 

	// reconnect timer handler will retry 
}

void server::build_desc() 
{
	cluster_actor::build_desc();

	std::ostringstream  oss;

	oss << " self: server " << "base: " << desc_;

	desc_ = oss.str();
}

bool server::listen_on_ports()
{
	bool rc = true;

	net::service::creator msg_pack_fn = msg_pack_creator;
	net::service::creator json_fn = json_creator;

	rc = rc && svc_->listen(cfg_.cluster_listen_addr, msg_pack_fn);

	if (!rc)
	{
		util::log()->error(
			"server. fail to listen on {0}", cfg_.cluster_listen_addr
		);

		return false;
	}

	if (cfg_.enable_client_listen)
	{
		rc = rc && svc_->listen(cfg_.client_listen_addr, msg_pack_fn);

		if (!rc)
		{
			util::log()->error(
				"server. fail to listen on {0}", cfg_.client_json_listen_addr
			);

			return false;
		}
	}

	if (cfg_.enable_json_protocol_for_client)
	{
		rc = rc && svc_->listen(cfg_.client_json_listen_addr, json_fn);

		if (!rc)
		{
			util::log()->error(
				"server. fail to listen on {0}", cfg_.client_json_listen_addr
			);

			return false;
		}
	}

	return rc;
}

bool server::connect_to_peers()
{
	net::service::creator msg_pack_fn = msg_pack_creator;
	net::service::creator json_fn = json_creator;

	bool rc = true;

	for (auto& peer : peers_)
	{
		if (peer.second.protocol == "msgpack")
		{
			rc = svc_->connect(peer.second.addr, msg_pack_fn);
		}
		else if (peer.second.protocol == "json")
		{
			rc = svc_->connect(peer.second.addr, json_fn);
		}
		else
		{
			util::log()->error(
				"server. unknown peer protocol: {0} addr: {1}", 
				peer.second.protocol, peer.second.addr
			);

			rc = false;
		}

		peer.second.connecting = rc;

		if (rc)
		{
			peer.second.last_retry_tick = 0.f;
		}
	}

	return rc;
}

net::session::ptr server::msg_pack_creator(
	net::service& svc,
	net::session::id id,
	tcp::socket&& soc,
	bool accepted)
{
	return std::make_shared<net::session_pack>(svc, id, std::move(soc), accepted);
}

net::session::ptr server::json_creator(
	net::service& svc,
	net::session::id id,
	tcp::socket&& soc,
	bool accepted)
{
	return std::make_shared<net::session_json>(svc, id, std::move(soc), accepted);
}

} // actor 
} // lax


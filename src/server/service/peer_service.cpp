#include "stdafx.h"
#include <lax/server/service/peer_service.hpp>
#include <lax/util/logger.hpp>

namespace lax
{
namespace server
{

peer_service::peer_service(server& _server, const nlm::json& _config)
	: service_actor(_server, _config)
{
}

peer_service::~peer_service()
{
}

void peer_service::add_peer(
	const std::string& addr, 
	const std::string& protocol, 
	float reconnect_interval
)
{
	auto iter = peers_.find(addr); 

	if (iter != peers_.end())
	{
		util::log()->warn(
			"peer w/ remote_addr: {} already exist. overriding...",
			addr
		);
	}

	reconnect_interval = std::max<float>(reconnect_interval, check_interval);

	peer p; 

	p.remote_addr = addr; 
	p.protocol = protocol;
	p.reconnect_interval = reconnect_interval;

	peers_[addr] = p;

	util::log()->info(
		"peer: {}/{} added", p.remote_addr, p.protocol
	);
}

bool peer_service::on_start()
{
	timer_.reset();

	// register dispatcher functions

	return true;
}

void peer_service::on_execute()
{
	if (timer_.elapsed() < check_interval)
	{
		return;
	}

	timer_.reset();

	process_messages();
	process_reconnect();
}

void peer_service::on_finish()
{
	peers_.clear();
	disp_.clear();
}

void peer_service::process_messages()
{

}

void peer_service::process_reconnect()
{

}

void peer_service::on_connect_failed()
{

}

void peer_service::on_session_ready()
{

}

void peer_service::on_session_closed()
{

}

} // server
} // lax
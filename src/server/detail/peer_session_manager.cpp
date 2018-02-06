#include "stdafx.h"
#include <lax/server/detail/peer_session_manager.hpp>

namespace lax
{
namespace server
{

peer_session_manager::peer_session_manager(server& server)
	: server_(server)
{

}

peer_session_manager::~peer_session_manager()
{

}

bool peer_session_manager::start()
{
	return false;
}

void peer_session_manager::execute()
{

}

void peer_session_manager::finish()
{

}

void peer_session_manager::add_peer(
	const std::string& addr, const std::string& protocol)
{

}


void peer_session_manager::on_connect_failed()
{

}

void peer_session_manager::on_session_ready()
{

}

void peer_session_manager::on_session_closed()
{

}

} // server
} // lax
#include "stdafx.h"
#include <lax/server/service_actor.hpp>
#include <lax/server/server.hpp>
#include <lax/util/logger.hpp>

namespace lax
{
namespace server
{

service_actor::service_actor(server& _server, const nlm::json& _config)
	: actor()
	, server_(_server)
	, config_(_config)
	, desc_(config_["name"] + "." + std::to_string(get_id()))
	, channel_(desc_)
{
	// change channel's configuration
}

service_actor::~service_actor()
{
}

bool service_actor::on_start()
{
	util::log()->info("{} started", get_desc());

	return true;
}

void service_actor::on_execute()
{
	get_channel().post();
}

void service_actor::on_finish()
{
	util::log()->info("{} finished", get_desc());
}

} // server
} // lax

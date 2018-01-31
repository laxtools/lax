#include "stdafx.h"
#include <lax/server/service.hpp>
#include <lax/server/server.hpp>
#include <lax/util/logger.hpp>

namespace lax
{
namespace server
{

service::service(server& _server, const config& _config)
	: actor()
	, server_(_server)
	, config_(_config)
	, desc_(_config.name + "." + std::to_string(get_id()))
	, channel_(desc_, _config.channel)
{
}

service::~service()
{
}

bool service::on_start()
{
	util::log()->info("{} started", get_desc());

	return true;
}

void service::on_execute()
{
	get_channel().post();
}

void service::on_finish()
{
	util::log()->info("{} finished", get_desc());
}

} // server
} // lax

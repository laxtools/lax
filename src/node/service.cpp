#include "stdafx.h"
#include <lax/server/service.hpp>
#include <lax/server/server.hpp>
#include <lax/util/logger.hpp>

namespace lax
{
namespace server
{

service::service(server& _server, const nlm::json& _config)
	: actor()
	, server_(_server)
	, config_(_config)
	, desc_(config_["name"].get<std::string>() + "." + std::to_string(get_id()))
	, channel_(desc_)
{
	// change channel's configuration
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
	get_channel().execute();
}

void service::on_finish()
{
	util::log()->info("{} finished", get_desc());
}

} // server
} // lax

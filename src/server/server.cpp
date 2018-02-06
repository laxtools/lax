#include "stdafx.h"
#include <lax/server/server.hpp>
#include <lax/net/service.hpp>
#include <lax/util/logger.hpp>

namespace lax
{
namespace server
{

server::server(const std::string& name, const nlm::json& config)
	: config_(config)
{
	load_config();
}

server::~server()
{
}

bool server::start()
{
	auto rc = net::service::inst().init();

	if (!rc)
	{
		util::log()->error( "failed to init net::service" );
		return false;
	}

	rc = scheduler_.start();

	if (!rc)
	{
		util::log()->error("failed to start task_scheduler");
		return false;
	}

	// listen

	return on_start();
}

void server::execute()
{
	scheduler_.execute();

	on_execute();
}

void server::finish()
{
	on_finish();

	scheduler_.finish();

	net::service::inst().fini();
}

bool server::on_start()
{
	return true;
}

void server::on_execute()
{
}

void server::on_finish()
{
}

void server::load_config()
{
	util::log()->info("loading server...");

	auto sconfig = config_.dump(4);

	util::log()->info(sconfig);

	// 

	util::log()->info("loaded."); 
}

} // server 
} // lax


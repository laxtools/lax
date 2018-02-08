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
	finish();
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

	rc = on_start();
	RETURN_IF(!rc, false);

	state_ = state::started;

	return state_ == state::started;
}

void server::execute()
{
	VERIFY(state_ == state::started);

	scheduler_.execute();

	on_execute();
}

void server::finish()
{
	RETURN_IF(state_ == state::init);
	RETURN_IF(state_ == state::finished);

	on_finish();

	scheduler_.finish();

	net::service::inst().fini();

	state_ = state::finished;
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


	auto jlistens = config_["listens"];

	util::log()->info("loaded."); 
}

} // server 
} // lax


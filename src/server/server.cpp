#include "stdafx.h"
#include <lax/server/server.hpp>
#include <lax/net/service.hpp>

namespace lax
{
namespace server
{

server::server(const nlm::json& config)
	: config_(config)
{
	load_config();
}

server::~server()
{
}

bool server::start()
{
	net::service::inst().init();

	scheduler_.start();

	// listen

	return on_start();
}

void server::execute()
{
	scheduler_.run();

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

}

} // server 
} // lax


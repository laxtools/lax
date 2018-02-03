#include "stdafx.h"
#include <lax/server/server.hpp>

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
	// start task_scheduler

	return on_start();
}

void server::execute()
{
	on_execute();
}

void server::finish()
{
	// finish task_scheduler

	on_finish();
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


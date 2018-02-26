#include "stdafx.h"
#include <lax/server/server.hpp>
#include <lax/net/service.hpp>
#include <lax/util/logger.hpp>

namespace lax
{
namespace server
{

server::server(const std::string& name, const nlm::json& config)
	: name_(name)
	, config_(config)
{
	load_config();
	load_peer_service();
}

server::~server()
{
	finish();
}

bool server::start()
{
	auto rc = start_scheduler();
	RETURN_IF(!rc, false);

	rc = net::service::inst().start();

	if (!rc)
	{
		util::log()->error("failed to init net::service");
		return false;
	}

	rc = start_listeners();
	RETURN_IF(!rc, false);

	if (peer_service_)
	{
		rc = peer_service_->start();
		RETURN_IF(!rc, false);
	}

	rc = on_start();
	RETURN_IF(!rc, false);

	state_ = state::started;

	return state_ == state::started;
}

void server::execute()
{
	VERIFY(state_ == state::started);

	scheduler_.execute();

	if (peer_service_)
	{
		peer_service_->execute();
	}

	on_execute();
}

void server::finish()
{
	RETURN_IF(state_ == state::constructed);
	RETURN_IF(state_ == state::finished);

	on_finish();

	scheduler_.finish();

	if (peer_service_)
	{
		peer_service_->finish();
	}

	net::service::inst().finish();

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

bool server::start_scheduler()
{
	auto jscheduler = config_["scheduler"];
	task::task_scheduler::config config;

	auto iter_runner_count = jscheduler.find("runner_count");
	if (iter_runner_count != jscheduler.end())
	{
		config.runner_count = iter_runner_count->get<int>();
	}

	auto iter_idle_check = jscheduler.find("idle_check_threshold_time");
	if (iter_idle_check != jscheduler.end())
	{
		config.idle_check_threshold_time = iter_idle_check->get<float>();
	}

	auto iter_single_loop_run = jscheduler.find("single_loop_run_limie");
	if (iter_single_loop_run != jscheduler.end())
	{
		config.single_loop_run_limit = iter_single_loop_run->get<unsigned int>();
	}

	auto iter_start_task = jscheduler.find("start_task_when_schedule_called");
	if (iter_start_task != jscheduler.end())
	{
		config.start_task_when_schedule_called = iter_start_task->get<bool>();
	}

	bool rc = scheduler_.start(config);

	if (!rc)
	{
		util::log()->error("failed to start task_scheduler");
		return false;
	}

	return rc;
}

bool server::start_listeners()
{
	int listener_count = 0;

	auto iter = config_["listens"];

	for (auto& peer : iter)
	{
		std::string addr = peer["address"].get<std::string>();
		std::string protocol = peer["protocol"].get<std::string>();

		auto rc = net::service::inst().listen(addr, protocol);

		if (rc)
		{
			listener_count++;
		}
	}

	return listener_count > 0;
}

void server::load_config()
{
	auto jid = config_["id"];
	id_ = jid.get<id_t>();
	desc_ = fmt::format("name:{}/id:{}", name_, id_);

	auto sconfig = config_.dump(4);
	util::log()->info("loading {} with {}", desc_, sconfig);

	util::log()->info("loaded."); 
}

void server::load_peer_service()
{
	auto jps = config_.find("peer_service");
	if (jps == config_.end())
	{
		return;
	}

	peer_service_ = std::make_unique<peer_service>(*this, config_["peer_service"]);
}

} // server 
} // lax


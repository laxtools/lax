#pragma once
#include <lax/actor/actor.hpp>
#include <lax/channel/channel.hpp>
#include <lax/net/session.hpp>
#include <lax/util/json.hpp>

namespace lax
{
namespace server
{

class server;

/// base class of services
/**
 * built with push common functionality upward approach
 * - design many not be clear, but useful
 *
 * subclass construction: 
 * - define config 
 * - load at server 
 * - schedule with task_runner at server
 * - topic handling
 *   - sub in on_start()
 *   - unsub in on_finish() 
 *   - write handlers for topics
 */
class service_actor : public actor::actor
{
public:
	using ptr = std::shared_ptr<service_actor>;

	/// config 
	struct config
	{
		std::string name = "service_actor";

		channel::channel::config channel;
	};

	/// reference to service_actor 
	struct ref
	{
		id_t node;				// server id  
		id_t id;				// actor id
		role_t role;			// actor role
		net::session::ref sref; // session ref (for remote)
	};

public:
	/// constructor
	service_actor(server& _server, const nlm::json& _config);

	/// destructor
	virtual ~service_actor();

	server& get_server()
	{
		return server_;
	}

	const server& get_server() const
	{
		return server_;
	}
	
	/// get underlying service_actor channel
	channel::channel& get_channel()
	{
		return channel_;
	}

	/// unique per service_actor instance
	const std::string& get_desc() const
	{
		return desc_;
	}

protected:
	/// start log
	virtual bool on_start() override;

	/// post messages in channel
	virtual void on_execute() override;

	/// finish log
	virtual void on_finish() override;

private: 
	service_actor(const service_actor& other) = delete;
	service_actor& operator=(const service_actor& other) = delete;

private:
	server& server_;
	nlm::json config_;
	std::string desc_;
	channel::channel channel_;
};

} // server
} // lax

#pragma once
#include <lax/task/task_scheduler.hpp>
#include <lax/server/service.hpp>

namespace lax
{
namespace server
{

/// server that start services 
class server
{
public: 
	using id_t = service::id_t;

	struct peer
	{
		enum state
		{
			init,
			connecting,
			connected,
			up, 
			down,
			disconnected,  
		};

		std::string addr;
		state state = state::init;
		id_t node;					
		// TODO: more properties
		net::session::ref sref;
	};

public:
	/// load and create services using configuration
	server(const std::string& file);

	/// destructor
	~server();

	/// start server
	/** 
	 * start services. 
	 * start protocols (listen on)
	 * start connecting to peers
	 */
	bool start();

	/// monitor services
	void execute();

	/// finish all services
	void finish();

	id_t get_id() const
	{
		return id_;
	}

	const std::string& get_desc() const
	{
		return desc_;
	}

private: 
	id_t id_;
	task::task_scheduler scheduler_;
	std::string desc_;
};

} // server 
} // lax


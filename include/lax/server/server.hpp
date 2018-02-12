#pragma once
#include <lax/server/service_actor.hpp>
#include <lax/task/task_scheduler.hpp>

namespace lax
{
namespace server
{

/// server that start services 
class server
{
public: 
	using id_t = service_actor::id_t;


public:
	/// load and create services using configuration
	server(const std::string& name, const nlm::json& config);

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

protected: 
	/// ���� Ŭ���� ����. start()���� ȣ��
	virtual bool on_start();

	/// ���� Ŭ���� ����. execute()���� ȣ��
	virtual void on_execute();

	/// ���� Ŭ���� ����. finish()���� ȣ��
	virtual void on_finish();

private: 
	enum state
	{
		init,
		started,
		finished
	};

	bool start_scheduler();

	bool start_listeners();

	void load_config();

	server(const server& other) = delete;
	server& operator=(const server& other) = delete;

private: 
	std::string name_;
	nlm::json config_;
	id_t id_ = 0;
	state state_ = state::init;

	task::task_scheduler scheduler_;
	std::string desc_;
};

} // server 
} // lax


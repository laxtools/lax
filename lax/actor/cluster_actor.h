#pragma once 

#include <lax/actor/actor.h>
#include <lax/actor/detail/directory.h>
#include <lax/util/result.h>

namespace lax
{
namespace actor
{

class server;

/// �ڽ��� ���¸� ��ü ������ �����ϴ� ��ɸ� ����
/** 
 * Ŭ������ ������ ���� ���ĸ� �ϴ� Ŭ�������� ��� Ŭ����. 
 * ��Ӹ� ������ �ڵ����� server�� ���� ���� ������Ʈ ����
 * 
 * add_attribute() �Լ� �����ϸ� �ʿ� �Ӽ� ���� ����
 */
class cluster_actor : public actor
{
public: 
	/// construct.
	/** 
	 * setup timers for cluster 
	 * setup message subs for cluster 
	 */
	cluster_actor(ptr parent, id aid, ref::role role);

	/// ����� ����
	~cluster_actor();

	/// for test only
	directory& get_dir_for_test_only();

protected: 
	/// bind a session with actor id using directory
	bool bind_session(id aid);

	/// bind a session with session id  
	bool bind_session(net::session::id sid);

	/// get more attributes into json obj
	/**
	 * obj["name"] = value�� ����
	 */
	virtual void add_attributes(json::Json& obj);

	/// override this to use more data
	virtual void proc_attributes(
		id aid,
		ref::role role,
		ref::state astate,
		const json::Json& obj
	);

	/// �� ���� ���� 
	void proc_state_self();

	/// �ٸ� Ŭ������ �޼��� 
	void on_cluster_state(net::msg::ptr m);

	/// get server from root
	server* get_server() const;

	/// get role
	ref::role get_role() const
	{
		return role_;
	}

	/// get directory
	directory& get_dir()
	{
		return dir_;
	}

	/// 
	void build_desc() override;

private:
	/// timer
	void setup_cluster_udpate_timer();

	/// message
	void setup_cluster_message_subscriptions();

protected: 
	ref::role role_;
	directory dir_;
};

} // actor 
} // lax

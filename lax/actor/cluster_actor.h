#pragma once 

#include <lax/actor/actor.h>
#include <lax/actor/detail/directory.h>
#include <lax/util/result.h>

namespace lax
{
namespace actor
{

class server;

/// 자신의 상태를 전체 서버에 전송하는 기능만 보유
/** 
 * 클러스터 내에서 상태 전파를 하는 클래스들의 기반 클래스. 
 * 상속만 받으면 자동으로 server를 통해 상태 업데이트 진행
 * 
 * add_attribute() 함수 구현하면 필요 속성 같이 전송
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

	/// 사라짐 통지
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
	 * obj["name"] = value로 저장
	 */
	virtual void add_attributes(json::Json& obj);

	/// override this to use more data
	virtual void proc_attributes(
		id aid,
		ref::role role,
		ref::state astate,
		const json::Json& obj
	);

	/// 내 상태 전파 
	void proc_state_self();

	/// 다른 클러스터 메세지 
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

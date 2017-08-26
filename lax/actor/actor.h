#pragma once 

#include <lax/actor/id.h>
#include <lax/actor/component.h>
#include <lax/actor/detail/timer.h>
#include <lax/actor/detail/post.h>
#include <lax/actor/detail/mailbox.h>
#include <lax/actor/detail/postbox.h>
#include <lax/actor/detail/ref.h>
#include <lax/actor/detail/frame_process.h>
#include <lax/actor/detail/subscription.h>
#include <lax/actor/detail/session_binder.h>

#include <lax/net/msg.h>
#include <lax/util/macros.h>
#include <lax/util/concurrent_queue.h>

#include <memory>
#include <thread>

namespace lax
{
namespace actor
{

/// abstract actor class
/** 
 * most processing / message objects are actors including: 
 *  - cluster node 
 *  - instance runner 
 *  - game room, game object
 * 
 * post / sub / unsub are used for messaging
 * process / on_process are used for processing
 */
class actor : public frame_process, public std::enable_shared_from_this<actor>
{
public: 
	using id = id;
	using ref = ref;
	using ptr = std::shared_ptr<actor>;
	using queue = util::concurrent_queue<net::msg::ptr>;
	using action = std::function<void(net::msg::ptr)>;
	using child_map = std::unordered_map<actor::id, actor::ptr>;
	using sid_vec = std::vector<net::session::id>;
	using aid_vec = std::vector<id>;

	friend class subscription;

	/// for test purpose only
	explicit actor(id aid); 

	/// parent must be valid 
	explicit actor(ptr parent, id aid); 

	/// destructor
	virtual ~actor();

	/// send to a session
	/** 
	 * session_binder를 통해 전송 시도. 안 되면 부모. 안 되면 send_direct
	 */
	reason send(net::session::id sid, net::msg::ptr m);

	/// send to an actor
	/** 
	 * session_binder를 통해 액터와 연겯된 세션으로 전송
	 */
	reason send(id aid, net::msg::ptr m);

	/// send to actors. return send count
	int send(const aid_vec& aids, net::msg::ptr m);

	/// send to sessions. returns send count
	int send(const sid_vec& sids, net::msg::ptr m);

	/// deliver message to others 
	/** 
	 * lookup subs. find actor. notify message
	 */
	void push(net::msg::ptr m);

	/// id 
	const id& get_id() const
	{
		return id_;
	}

	/// get 하기 전 꼭 사용
	/**
	* usage:
	*
	*  if ( has(comp_move) )
	*  {
	*		auto cp_mv = get(comp_move);
	*      cp_mv->move_to(target);
	*  }
	*/
	bool has_comp(component::key key)
	{
		return comps_.find(key) != comps_.end();
	}

	/// get component. 없으면 exception 발생. has_comp로 체크 
	template <typename Type> auto get_comp() const;

	/// test 용으로만 사용. actor 외에 메일 박스 접근 안 함
	mailbox& get_mailbox_for_test_only()
	{
		return mailbox_;
	}

	/// timer를 액터 문맥에서 테스트 하기 위해서만 사용
	timer& get_timer_for_test_only()
	{
		return timer_;
	}

	/// to debug, log 
	const std::string& get_desc() const
	{
		return desc_;
	}

protected:
	/// 컴포넌트 생성 및 등록
	/**
	 * 생성자에서 등록하는 것을 기본으로 함
	 */
	template <class Comp, class... Args> 
	bool init(Args&&... args)
	{
		return add_comp(std::make_shared<Comp>(args...));
	}

	virtual void on_process() override;

	/// send to the session (server implements this)
	/**
	* 마지막 fallback으로 service를 통해 전송. server에서 구현
	*
	* 고려: virtual 말고 함수를 지정하는 방식으로도 가능하게 할 지
	* 성능 측정이나 사용성을 보고 개선하도록 함
	*/
	virtual reason send_direct(net::session::id sid, net::msg::ptr m);

	/// 컴포넌트 등록. settle 호출 
	bool add_comp(component::ptr comp);

	/// check 
	bool has_child(actor::id child);

	/// add actor child
	void add_child(actor::ptr child);

	/// remvoe actor child 
	bool remove_child(actor::id id);

	/// subscribe and remember. auto release on destruct. returns memory id 
	int subscribe_to(actor::ptr ptr, post::tag& tv, post::target& tgt);

	/// subscribe local and remember. call sub_to. 
	int subscribe_local(post::tag& tv, post::target& tgt);

	/// subscribe and remember. auto release on destruct. returns memory id 
	int subscribe_post_to(actor::ptr ptr, post::tag& tv, post::target& tgt);

	/// register to recv for tag by push into me from ptr
	int subscribe_push(actor::ptr ptr, post::tag& tv);

	/// bulid debug string to describe self
	virtual void build_desc();

	/// 부모 있는 지 확인 
	bool has_parent() const
	{
		return (bool)parent_;
	}

	/// 부모 가져오기
	ptr get_parent() const
	{
		return parent_;
	}

	/// 자식 가져오기
	child_map& get_children()
	{
		return children_;
	}

	/// 타이머. 컴포넌트 등에서 사용
	timer& get_timer()
	{
		return timer_;
	}

	/// 메세지 큐 처리용 메일 박스
	mailbox& get_mailbox()
	{
		return mailbox_;
	}

	postbox& get_postbox()
	{
		return postbox_;
	}

	session_binder& get_session_binder()
	{
		return session_binder_;
	}

private: 
	void post();


protected: 
	ptr					parent_;
	id					id_;
	std::string			desc_;
	component::map		comps_;
	queue				queue_;
	child_map			children_;
	timer				timer_;
	mailbox				mailbox_;
	subscription		mailbox_subs_;
	postbox				postbox_;
	subscription		postbox_subs_;
	session_binder		session_binder_;
};


template <typename Type>
auto actor::get_comp() const
{
	auto iter = comps_.find(Type::key_type);
	if (iter == comps_.end())
	{
		check(false && "has() must be used to check");

		throw std::exception("component not found");
	}

	return std::static_pointer_cast<Type>(iter->second);
}

} // actor
} // lax

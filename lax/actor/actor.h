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
	 * session_binder�� ���� ���� �õ�. �� �Ǹ� �θ�. �� �Ǹ� send_direct
	 */
	reason send(net::session::id sid, net::msg::ptr m);

	/// send to an actor
	/** 
	 * session_binder�� ���� ���Ϳ� ����� �������� ����
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

	/// get �ϱ� �� �� ���
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

	/// get component. ������ exception �߻�. has_comp�� üũ 
	template <typename Type> auto get_comp() const;

	/// test �����θ� ���. actor �ܿ� ���� �ڽ� ���� �� ��
	mailbox& get_mailbox_for_test_only()
	{
		return mailbox_;
	}

	/// timer�� ���� ���ƿ��� �׽�Ʈ �ϱ� ���ؼ��� ���
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
	/// ������Ʈ ���� �� ���
	/**
	 * �����ڿ��� ����ϴ� ���� �⺻���� ��
	 */
	template <class Comp, class... Args> 
	bool init(Args&&... args)
	{
		return add_comp(std::make_shared<Comp>(args...));
	}

	virtual void on_process() override;

	/// send to the session (server implements this)
	/**
	* ������ fallback���� service�� ���� ����. server���� ����
	*
	* ���: virtual ���� �Լ��� �����ϴ� ������ε� �����ϰ� �� ��
	* ���� �����̳� ��뼺�� ���� �����ϵ��� ��
	*/
	virtual reason send_direct(net::session::id sid, net::msg::ptr m);

	/// ������Ʈ ���. settle ȣ�� 
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

	/// �θ� �ִ� �� Ȯ�� 
	bool has_parent() const
	{
		return (bool)parent_;
	}

	/// �θ� ��������
	ptr get_parent() const
	{
		return parent_;
	}

	/// �ڽ� ��������
	child_map& get_children()
	{
		return children_;
	}

	/// Ÿ�̸�. ������Ʈ ��� ���
	timer& get_timer()
	{
		return timer_;
	}

	/// �޼��� ť ó���� ���� �ڽ�
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

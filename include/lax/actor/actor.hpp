#pragma once 

#include <lax/actor/component.hpp>
#include <lax/actor/tick.hpp>
#include <lax/util/result.hpp>
#include <lax/util/sequence.hpp>
#include <typeinfo>
#include <unordered_map>

namespace lax 
{
namespace actor 
{

/// actor w/ components
/**
 * task
 * - ��𼭳� ���� ����. 
 * - �� task_runner�� �ƴ϶� ���� ����
 *
 * component
 * - actor�� ���� ������ ó��
 * - actor Ŭ������ ������ �������� ��
 *
 * id 
 * - 32��Ʈ sequence�� ���. ���� 42�ﰳ ������ ����
 *
 * type
 * - ���� ���� Ŭ������ Ÿ�� �̸�
 */
class actor : public std::enable_shared_from_this<actor>
{
public: 
	using ptr = std::shared_ptr<actor>;
	using weak_ptr = std::weak_ptr<actor>;
	using result = util::result<bool, std::string>;
	using id_t = uint32_t;
	using type_t = const char*;
	using vec_t = std::vector<actor::ptr>;

public:
	static constexpr type_t type = "actor";

	actor(weak_ptr parent = weak_ptr());

	virtual ~actor();

	/// ����. on_start() ȣ��. ���� �� false
	bool start();

	/// Ư�� runner���� ����. on_execute() ȣ��
	void execute();

	/// ����. on_finish() ȣ��. 
	void finish();

	/// add component
	template <class Comp, class... Args>
	bool add_comp(Args&&... args)
	{
		return add_comp(std::make_shared<Comp>(*this, args...));
	}

	/// get component
	/**
	 * auto move = get_comp<comp_movement>()
	 * if ( move ) { .. } 
	 */
	template <class Comp>
	typename Comp::ptr get_comp() const
	{
		return std::static_pointer_cast<Comp>(get_comp(Comp::type));
	}


	id_t get_id() const
	{
		return id_;
	}

	bool is_started() const
	{
		return started_;
	}

	virtual type_t get_type() const
	{
		return type;
	}

	/// apply function to actors
	template <typename Comp, typename Func>
	static void for_each(vec_t& actors, Func func)
	{
		for (auto& ap : actors)
		{
			auto comp = ap->get_comp<Comp>();
			if (comp)
			{
				func(*comp);
			}
		}
	}

protected:
	/// ���� Ŭ���� ����. start()���� ȣ��
	virtual bool on_start();

	/// ���� Ŭ���� ����. execute()���� ȣ��
	virtual void on_execute();

	/// ���� Ŭ���� ����. finish()���� ȣ��
	virtual void on_finish();

	bool has_parent() const
	{
		return !parent_.expired();
	}

	weak_ptr get_parent() const
	{
		return parent_;
	}

private: 
	using comp_map = std::unordered_map<component::type_t, component::ptr>;

	component::ptr get_comp(component::type_t type) const;

	bool add_comp(component::ptr comp);

private:
	static util::sequence<uint32_t, std::recursive_mutex> id_seq_;

	weak_ptr parent_;
	uint32_t id_;
	comp_map comps_;
	bool started_ = false;
};

} // actor 
} // lax

#define ACTOR_HEAD(cls) \
static constexpr type_t type = #cls; \
using ptr = std::shared_ptr<cls>; \
type_t get_type() const override { return type; } 
#pragma once 

#include <lax/actor/component.hpp>
#include <lax/actor/tick.hpp>
#include <lax/actor/detail/type_object_container.hpp>
#include <lax/util/result.hpp>
#include <lax/util/sequence.hpp>
#include <lax/util/exception.hpp>
#include <typeinfo>
#include <unordered_map>

namespace lax 
{
namespace actor 
{

/// actor w/ components
/**
 * component
 * - actor�� ���� ������ ó��
 * - actor Ŭ������ ������ �������� ��
 *
 * id 
 * - 32��Ʈ sequence�� ���. ���� 42�ﰳ ������ ����
 * 
 * type_object ���
 * - �����ڿ��� push_type<Class>()�� ��� 
 */
class actor : public std::enable_shared_from_this<actor>, public type_object
{
public: 
	using ptr = std::shared_ptr<actor>;
	using weak_ptr = std::weak_ptr<actor>;
	using result = util::result<bool, std::string>;
	using id_t = uint32_t;
	using role_t = uint32_t;
	using vec_t = std::vector<actor::ptr>;

public:
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
	typename Comp::ptr add_component(Args&&... args)
	{
		auto cp = std::make_shared<Comp>(*this, args...);

		return comps_.add<Comp>(cp);
	}

	/// get component
	/**
	 * auto move = get_component<comp_movement>()
	 * if ( move ) { .. } 
	 */
	template <class Comp>
	typename Comp::ptr get_component() const
	{
		return comps_.get<Comp>();
	}

	template <class Comp>
	typename std::vector<typename Comp::ptr> get_components() const
	{
		return comps_.get_types<Comp>();
	}

	/// Func == bool (Base::ptr)
	template <class Comp, typename Func>
	std::size_t apply_components(Func func) const
	{
		return comps_.apply<Comp>(func);
	}

	id_t get_id() const
	{
		return id_;
	}

	bool is_started() const
	{
		return state_ == state::started;
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
	enum state
	{
		constructed, 
		started, 
		finished
	};
	using component_container = type_object_container<component>;

private:
	static util::sequence<uint32_t, std::recursive_mutex> id_seq_;

	weak_ptr parent_;
	uint32_t id_ = 0;
	state state_ = state::constructed;
	component_container comps_;
};

} // actor 
} // lax

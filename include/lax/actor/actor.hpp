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
 * - actor의 세부 구현을 처리
 * - actor 클래스간 공유를 목적으로 함
 *
 * id 
 * - 32비트 sequence를 사용. 동시 42억개 정도가 제한
 * 
 * type_object 사용
 * - 생성자에서 push_type<Class>()로 등록 
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

	/// 시작. on_start() 호출. 실패 시 false
	bool start();

	/// 특정 runner에서 실행. on_execute() 호출
	void execute();

	/// 종료. on_finish() 호출. 
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
	/// 하위 클래스 구현. start()에서 호출
	virtual bool on_start();

	/// 하위 클래스 구현. execute()에서 호출
	virtual void on_execute();

	/// 하위 클래스 구현. finish()에서 호출
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

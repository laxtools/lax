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
 * - 어디서나 실행 가능. 
 * - 꼭 task_runner가 아니라도 실행 가능
 *
 * component
 * - actor의 세부 구현을 처리
 * - actor 클래스간 공유를 목적으로 함
 *
 * id 
 * - 32비트 sequence를 사용. 동시 42억개 정도가 제한
 *
 * type
 * - 최종 액터 클래스의 타잎 이름
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

	/// 시작. on_start() 호출. 실패 시 false
	bool start();

	/// 특정 runner에서 실행. on_execute() 호출
	void execute();

	/// 종료. on_finish() 호출. 
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
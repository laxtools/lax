#pragma once 

#include <lax/actor/detail/frame_process.h>
#include <lax/util/macros.h>

#include <algorithm>
#include <memory>
#include <unordered_map>

namespace lax
{
namespace actor
{

class actor;

/// componet abstract base for actor to separate concerns
/** 
 * component의 소유권은 하나의 액터에서만 갖고 
 * actor의 생성자에서 init<Type>()으로 생성하고 
 * actor 소멸자에서 component::map 소멸로 소멸한다. 
 *
 * Usage: 
 *   actor에 각 component 포인터를 가짐. 
 *
 *   add()를 통해 process 전체 도는 것과 
 *   다른 actor / 코드에서 참조 가능하도록 함. 
 * 
 *   사용할 때 has()로 체크하고 get()을 하도록 함 
 *   get()에서 실패나면 exception 발생. 
 * 
 *   on_process()에서 주기적인 처리 등록.
 * 
 *   COMPONENT_OVERRIDES(399); 와 같이 
 *   public section에 추가하면 필요 함수와 키값 정의
 * 
 */
class component : public frame_process 
{
public:
	using key = uint16_t;
	using ptr = std::shared_ptr<component>;
	using map = std::unordered_map<key, ptr>;

public: 
	component() = default;
	virtual ~component() = default;

	virtual key get_key() const = 0;

	/// 부모 액터를 지정한다. 
	/** 
	 * 포인터 사용에서 문제를 피하려면 
	 * actor의 생성자에서만 컴포넌트를 생성한다. 
	 * actor::init() 호출로 간편하게 가능하다.
	 */
	bool settle(actor* owner)
	{
		check(owner);
		return_if(!owner, false);

		check(!owner_);
		return_if(owner_, false);

		owner_ = owner;
		return true;
	}

	/// get owner. always valid after settle
	actor* owner() const
	{
		if (!owner_)
		{
			throw std::exception("component onwer is null");
		}

		return owner_;
	}

protected:
	void on_process() override { }

private: 
	actor* owner_ = nullptr;
};

} // actor
} // lax

/// component 하위 클래스 필요 함수. static 변수 등록
#define COMPONENT_BODY_OVERRIDE(KEY) \
		static const key key_type = (KEY); \
		\
		key get_key() const override \
		{ \
			return key_type; \
		}
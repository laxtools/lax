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
 * component�� �������� �ϳ��� ���Ϳ����� ���� 
 * actor�� �����ڿ��� init<Type>()���� �����ϰ� 
 * actor �Ҹ��ڿ��� component::map �Ҹ�� �Ҹ��Ѵ�. 
 *
 * Usage: 
 *   actor�� �� component �����͸� ����. 
 *
 *   add()�� ���� process ��ü ���� �Ͱ� 
 *   �ٸ� actor / �ڵ忡�� ���� �����ϵ��� ��. 
 * 
 *   ����� �� has()�� üũ�ϰ� get()�� �ϵ��� �� 
 *   get()���� ���г��� exception �߻�. 
 * 
 *   on_process()���� �ֱ����� ó�� ���.
 * 
 *   COMPONENT_OVERRIDES(399); �� ���� 
 *   public section�� �߰��ϸ� �ʿ� �Լ��� Ű�� ����
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

	/// �θ� ���͸� �����Ѵ�. 
	/** 
	 * ������ ��뿡�� ������ ���Ϸ��� 
	 * actor�� �����ڿ����� ������Ʈ�� �����Ѵ�. 
	 * actor::init() ȣ��� �����ϰ� �����ϴ�.
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

/// component ���� Ŭ���� �ʿ� �Լ�. static ���� ���
#define COMPONENT_BODY_OVERRIDE(KEY) \
		static const key key_type = (KEY); \
		\
		key get_key() const override \
		{ \
			return key_type; \
		}
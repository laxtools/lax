#pragma once 

#include <lax/actor/detail/type_info.hpp>
#include <vector>

namespace lax
{
namespace actor
{

/// single inheritance reflection object
class type_object
{
public:
	using types = std::vector<type_info>;

public: 
	type_object()
	{
		// 나는 등록 안 함
	}

	bool is_a(const std::type_info& info)
	{
		for (auto& type : types_)
		{
			if (type == info)
			{
				return true;
			}
		}

		return false;
	}

	const types& get_types() const
	{
		return types_;
	}

protected: 
	template <typename Type>
	void push_type()
	{
		push_type(typeid(Type));
	}

	void push_type(const std::type_info& info)
	{
		types_.push_back(info);
	}

private: 
	types types_;
};

} // actor
} // lax
#pragma once 

#include <lax/actor/detail/type_object.hpp>
#include <lax/util/exception.hpp>
#include <unordered_map>

namespace lax
{
namespace actor
{

/// used to query w/ inheritance types
template <typename Base>
class type_object_container
{
public: 
	using ptr_t = std::shared_ptr<Base>;

public: 
	type_object_container()
	{
	}

	/// Func == bool (Base::ptr)
	template <typename Func>
	std::size_t apply(Func func) const
	{
		return apply<Base>(func);
	}

	/// Func == bool (Base::ptr)
	template <typename Type, typename Func>
	std::size_t apply(Func func) const
	{
		auto iter = map_.find(typeid(Type));
		return_if(iter == map_.end(), 0);

		auto& vec = iter->second.ptrs;

		std::size_t count = 0;

		for (auto& p : vec)
		{
			if (func(p))
			{
				++count;
			}
		}

		return count;
	}

	template <typename Type>
	typename Type::ptr add(typename Type::ptr type_ptr)
	{
		auto info = std::static_pointer_cast<Base>(type_ptr);

		// check w/ c++
		if (!info)
		{
			THROW("Type must be derived from Base class!");
		}

		// check w/ type_object
		if (!type_ptr->is_a(typeid(Base)))
		{
			THROW("Type must be derived from Base class! 2nd check.");
		}

		auto& types = type_ptr->get_types();

		for (auto& type : types)
		{
			add_type(type, type_ptr);
		}

		return type_ptr;
	}

	/// get the first matching type object
	template <typename Type>
	typename Type::ptr get() const
	{
		return std::static_pointer_cast<Type>(get_type(typeid(Type)));
	}

	template <typename Type>
	bool has() const
	{
		return !!get_type(typeid(Type));
	}

	template <typename Type>
	typename const std::vector<typename Type::ptr> get_types() const
	{
		auto& types = get_types(typeid(Type));

		std::vector<typename Type::ptr> vec; 

		for (auto& type : types)
		{
			vec.push_back(std::static_pointer_cast<Type>(type));
		}

		return vec;
	}

	template <typename Type>
	typename std::vector<typename Type::ptr> get_types() 
	{
		std::vector<typename Type::ptr> vec; 

		for (auto& type : types)
		{
			vec.push_back(std::static_pointer_cast<Type>(type));
		}

		return vec;
	}

	/// returns the count of type object w/ Type
	template <typename Type>
	std::size_t size() const
	{
		return get_types(typeid(Type)).size();
	}

private: 
	using vec = std::vector<ptr_t>;

	struct entry
	{
		vec ptrs;
	};

	using type_map = std::unordered_map<type_info, entry>;

private:
	void add_type(const type_info& info, ptr_t ptr)
	{
		auto iter = map_.find(info);

		if (iter != map_.end())
		{
			iter->second.ptrs.push_back(ptr);
		}
		else
		{
			entry e; 
			e.ptrs.push_back(ptr);

			map_.insert(type_map::value_type(info, e));
		}
	}

	ptr_t get_type(const type_info& info) const
	{
		auto iter = map_.find(info);
		return_if(iter == map_.end(), ptr_t());

		return iter->second.ptrs[0];
	}

	const std::vector<ptr_t>& get_types(const type_info& info) const
	{
		auto iter = map_.find(info);
		return_if(iter == map_.end(), null_);

		return iter->second.ptrs;
	}

	std::vector<ptr_t>& get_types(const type_info& info) 
	{
		auto iter = map_.find(info);
		return_if(iter == map_.end(), null_);

		return iter->second.ptrs;
	}

private:
	type_map map_;
	vec null_;
};

} // actor
} // lax

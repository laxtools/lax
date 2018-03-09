#pragma once

#include "idl_type_objects.hpp"
#include "idl_unit.hpp"
#include "idl_result.hpp"

namespace bitsc
{

class idl_parser
{
public: 
	idl_parser(); 

	~idl_parser(); 

	/// test implementation
	void test_parse(const std::string& file);

private: 
	// type object table
	// units 

	result parse(nlohmann::json::const_iterator& it);

	result parse_type_object(const std::string& key, const nlohmann::json& js);

	bool create_current_type(const std::string& type, const std::string& name);

	template <typename Type>
	bool create_type(const std::string& type, const std::string& name)
	{
		current_type_ = std::make_shared<Type>();
		current_type_->type_name = type;
		current_type_->name = name;

		return true;
	}

private: 
	idl_unit::ptr main_unit_;
	type_object::ptr current_type_;
};

} // bitsc
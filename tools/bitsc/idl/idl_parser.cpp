#include "stdafx.h"
#include "idl_parser.hpp"

#include <lax/util/logger.hpp>
#include <lax/util/macros.hpp>

#include <fstream>

using namespace nlohmann;
using namespace lax::util;

namespace bitsc
{

idl_parser::idl_parser()
{

}

idl_parser::~idl_parser()
{

}

void idl_parser::test_parse(const std::string& file)
{
	std::ifstream fs(file);

	RETURN_IF(!fs.is_open());

	json js;
	fs >> js;

	// create a main unit
	main_unit_ = std::make_shared<idl_unit>();
	main_unit_->input_file = file;

	current_unit_ = main_unit_;

	for (json::const_iterator it = js.cbegin(); it != js.cend(); ++it) 
	{
		log::get()->info("parsing {}", it.key());

		parse(it);
	}

	// using sequence, generate files. 



	// include
	// - parse tree saved
	// - load parse tree

	// namespace 

	// handling errors
}

bool idl_parser::complete_current_type()
{
	VERIFY(current_type_);
	VERIFY(current_unit_);
	RETURN_IF(!current_type_, false);
	RETURN_IF(!current_unit_, false);

	// 현재 unit에 현재 타잎 등록
	auto& types = current_unit_->table.types;

	auto iter = types.find(current_type_->name);
	RETURN_IF(iter != types.end(), false);			// 

	types[current_type_->name] = current_type_;
}

result idl_parser::parse(json::const_iterator& it)
{
	// branch on it.key()

	return parse_type_object(it.key(), it.value());
}

result idl_parser::parse_type_object(const std::string& key, const json& js)
{
	auto type_it = js.find("type");

	if (type_it == js.cend())
	{
		return result{
			key, 
			"fail: type is not found", 
			result::error::fail_object_type_not_found 
		};
	}

	auto type_name = type_it.value().get<std::string>();

	auto name_iter = js.find("name");
	if (name_iter == js.end())
	{
		return result{
			key, 
			"fail: object name not found", 
			result::error::fail_object_name_not_found 
		};
	}

	auto name = name_iter.value().get<std::string>();

	auto rc = create_current_type(type_name, name); 

	if (!rc)
	{
		return result{
			name,
			"fail: object type not supported",
			result::error::fail_object_type_not_supported
		};
	}

	return current_type_->parse(*this, js);
}

bool idl_parser::create_current_type(
	const std::string& type, 
	const std::string& name
)
{
	// TODO: remove repetitions with a prototype function map

	if (type == "enum")
	{
		return create_type<enum_type>(type, name);
	}

	if (type == "struct")
	{
		return create_type<struct_type>(type, name);
	}

	if (type == "message")
	{
		return create_type<message_type>(type, name);
	}

	return false;
}

} // bitsc
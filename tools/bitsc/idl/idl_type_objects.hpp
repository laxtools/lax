#pragma once

#include "idl_result.hpp"
#include <lax/util/json.hpp>

#include <memory>
#include <string>
#include <vector>

namespace bitsc
{

class idl_parser;

struct type_object
{
	using ptr = std::shared_ptr<type_object>;

	std::string name;
	std::string type_name;

	virtual result parse(idl_parser& parser, const nlohmann::json& js) = 0;

	virtual result generate(idl_parser& parser, std::ostream& os) = 0;
};

struct value
{
	std::string name;
	int val;

	result parse(idl_parser& parser, const nlohmann::json& js);

	result generate(idl_parser& parser, std::ostream& os);
};

struct values
{
	std::vector<value> vec;

	result parse(idl_parser& parser, const nlohmann::json& js);

	result generate(idl_parser& parser, std::ostream& os);
};

struct enum_type : type_object
{
	values vals;

	result parse(idl_parser& parser, const nlohmann::json& js) override;

	result generate(idl_parser& parser, std::ostream& os) override;
};

/// a field
struct field
{
	std::string name;
	std::string type_name;
	std::string default_value; 

	result parse(idl_parser& parser, const nlohmann::json& js);

	result generate(idl_parser& parser, std::ostream& os);
};

struct fields
{
	std::vector<field> vec;

	result parse(idl_parser& parser, const nlohmann::json& js);

	result generate(idl_parser& parser, std::ostream& os);
};

struct struct_type : type_object
{
	fields fields;

	result parse(idl_parser& parser, const nlohmann::json& js) override;

	result generate(idl_parser& parser, std::ostream& os) override;
};

struct message_type : type_object
{
	fields fields;

	result parse(idl_parser& parser, const nlohmann::json& js) override;

	result generate(idl_parser& parser, std::ostream& os) override;
};

} // bitsc
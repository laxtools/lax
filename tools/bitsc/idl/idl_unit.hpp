#pragma once 

#include "idl_type_objects.hpp"

#include <map>
#include <memory>

namespace bitsc
{

struct type_table
{
	std::map<std::string, type_object::ptr> types;

	std::vector<std::string> sequence;
};

struct idl_unit
{
	using ptr = std::shared_ptr<idl_unit>;

	std::string input_file;  
	std::string parse_file;	 
	std::string outptu_file; 
	
	type_table table;

	ptr parent;
};

} // bitsc
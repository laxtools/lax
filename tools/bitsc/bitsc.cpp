#include "stdafx.h"

#include "idl/idl_parser.hpp"

int main()
{
	bitsc::idl_parser parser; 

	// parse types only
	parser.test_parse("idl_test_1.json");
	
    return 0;
}


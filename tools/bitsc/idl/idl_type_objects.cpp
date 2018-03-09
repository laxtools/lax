#include "stdafx.h"
#include "idl_type_objects.hpp"
#include "idl_parser.hpp"

using namespace nlohmann;


namespace bitsc
{

result value::parse(idl_parser& parser, const json& js)
{
	return result{ "", "", result::error::success };
}

result values::parse(idl_parser& parser, const json& js)
{
	return result{ "", "", result::error::success };
}

result field::parse(idl_parser& parser, const json& js)
{
	return result{ "", "", result::error::success };
}

result fields::parse(idl_parser& parser, const json& js)
{
	return result{ "", "", result::error::success };
}

result enum_type::parse(idl_parser& parser, const json& js)
{
	return vals.parse(parser, js);
}

result struct_type::parse(idl_parser& parser, const json& js)
{
	return fields.parse(parser, js);
}

result message_type::parse(idl_parser& parser, const json& js)
{
	return fields.parse(parser, js);
}

result value::generate(idl_parser& parser, std::ostream& os)
{
	return result{ "", "", result::error::success };
}

result values::generate(idl_parser& parser, std::ostream& os)
{
	return result{ "", "", result::error::success };
}

result field::generate(idl_parser& parser, std::ostream& os)
{
	return result{ "", "", result::error::success };
}

result fields::generate(idl_parser& parser, std::ostream& os)
{
	return result{ "", "", result::error::success };
}

result enum_type::generate(idl_parser& parser, std::ostream& os)
{
	return result{ "", "", result::error::success };
}

result struct_type::generate(idl_parser& parser, std::ostream& os)
{
	return result{ "", "", result::error::success };
}

result message_type::generate(idl_parser& parser, std::ostream& os)
{
	return result{ "", "", result::error::success };
}

} // bitsc
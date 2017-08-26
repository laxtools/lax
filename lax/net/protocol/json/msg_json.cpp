#include "stdafx.h"
#include "msg_json.h"
#include <lax/util/logger.h>
#include <lax/net/protocol/protocol.h>

namespace lax
{
namespace net
{

msg_json::msg_json()
	: msg(protocol::json)
{
}

msg_json::~msg_json()
{
}

msg_json::result msg_json::pack(const json::Json& obj)
{
	json_ = obj; // copy intentional
	json_.dump(str_);

	str_.append("\r\n");

	if (obj["key"].is_null())
	{
		return result(false, reason::fail_json_build_error_key_not_found);
	}

	if (obj["msg"].is_null())
	{
		return result(false, reason::fail_json_build_error_msg_not_found);
	}

	valid_ = true;
	error_.clear();

	return result(true, reason::success);
}

msg_json::result msg_json::unpack(const std::string& json)
{
	error_.clear();
	valid_ = false;
	str_ = json;

	json_ = json::Json::parse(json, error_);

	if (!error_.empty())
	{
		util::log()->error(
			"func: {0} parse failed. input: {1}, error: {2}",
			__FUNCTION__, json, error_
		);

		return result(false, fail_json_parse_error);
	}

	if (json_["key"].is_null())
	{
		error_ = "parse error. key field not found";

		return result(false, reason::fail_json_parse_error_key_not_found);
	}

	if (json_["msg"].is_null())
	{
		error_ = "parse error. msg field not found";

		return result(false, reason::fail_json_parse_error_msg_not_found);
	}

	valid_ = true;

	return result(true, reason::success);
}

} // net
} // lax

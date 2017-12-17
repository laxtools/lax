#pragma once 

#include <lax/net/msg.h>
#include <lax/net/reason.h>
#include <lax/util/result.h>
#include <lax/util/json.hpp>

namespace lax
{
namespace net
{

/// json msg 
/**
 * { key: "key value", msg: { body of message }}
 *
 * msg_json m; 
 * m.parse( "
 *     { key : "hello", msg : 
 *     { name: "kd", age : 33, values : [ true, 1, 333 ] }
 *     " );
 * 
 * auto d = m.get_msg();
 * 
 * d["name"].string_value(), 
 * d["age"].int_value(), 
 * d["values"][0].bool_value()
 * ...
 */
class msg_json : public msg
{
public: 
	using result = util::result<bool, reason>;

	const static std::size_t max_string_length = 8 * 1024;

public: 
	msg_json();
	~msg_json();

	/// object에서 메세지로 변환
	result pack(const json::Json& obj);

	/// string에서 Json으로 변환
	result unpack(const std::string& json);

	/// get msg object
	const json::Json& get_msg() const
	{
		return json_["msg"];
	}

	/// get string
	const std::string& get_str() const
	{
		return str_;
	}

	/// get message key 
	const std::string& get_key() const
	{
		return json_["key"].string_value();
	}

	/// check validity
	bool is_valid() const
	{
		return valid_;
	}

private: 
	bool		valid_ = false;
	std::string error_;
	std::string str_;
	json::Json  json_;
};

} // net
} // lax

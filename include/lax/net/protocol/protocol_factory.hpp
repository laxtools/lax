#pragma once 

#include <lax/net/protocol/protocol.hpp>

#include <functional>
#include <map>

namespace lax
{
namespace net
{

class protocol_factory
{
public: 
	using creator = std::function<protocol::ptr()>;

	static protocol_factory& inst();

	/// add a creator for a protocol
	void add(const std::string& name, creator c);

	/// check
	bool has(const std::string& name);

	/// create a protocol
	protocol::ptr create(const std::string& name) const;

private: 
	using map = std::map<const std::string, creator>;

	map map_;
};

} // net 
} // lax

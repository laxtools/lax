#pragma once

#include <lax/server/service_actor.hpp>

namespace lax
{
namespace server
{

class service_factory
{
public:
	using creator = std::function<service_actor::ptr(const nlm::json&)>;

public:
	static service_factory& inst();

	~service_factory();

	/// add a creator for a protocol
	void add(const std::string& name, creator c);

	/// check
	bool has(const std::string& name);

	/// create a protocol
	service_actor::ptr create(const std::string& name, const nlm::json& config) const;

private:
	using map = std::map<const std::string, creator>;

	map map_;
};

} // server
} // lax
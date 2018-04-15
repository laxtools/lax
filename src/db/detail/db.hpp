#pragma once

#include <string>

namespace lax
{
namespace db
{

/// db information
class db 
{
public: 
	struct config
	{
		std::string name; 
		std::string account; 
		std::string passworld;
	};

public: 
	db(const config& cfg)
		: config_(cfg)
	{
	}

private: 
	config config_;
};

} // db
} // lax
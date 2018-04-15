#pragma once

#include <lax/db/nanodbc.h>

namespace lax
{
namespace db
{

/// database connection
class connection
{
public: 
	using ptr_t = std::shared_ptr<connection>;

public:
	connection()
	{
	}

	~connection();

private:
	nanodbc::connection conn_;
};

} // db
} // lax
#pragma once

#include <lax/db/query.hpp>
#include <lax/db/nanodbc.h>
#include <lax/task/task.hpp>

namespace lax
{
namespace db
{

class query_task : public task::task
{
public:
	query_task(query::ptr query);

	~query_task();

private:
	void on_execute() override;

private:
	query::ptr query_;
	nanodbc::connection conn_;
};

} // db
} // lax
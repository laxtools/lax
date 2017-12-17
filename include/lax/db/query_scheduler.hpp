#pragma once

#include <lax/db/query.hpp>
#include <lax/task/task_scheduler.hpp>

namespace lax
{
namespace db
{

class query_scheduler
{
public:
	query_scheduler();

	~query_scheduler();

	/// task로 만들어 넣기. 결과는 채널로 통지
	void schedule(query::ptr query, const query::channel_key_t& channel);
	
private:
	task::task_scheduler scheduler_;
};

} // db
} // lax
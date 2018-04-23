#pragma once

#include <lax/db/nanodbc.h>
#include <lax/channel/message.hpp>

namespace lax
{
namespace db
{

class query : public channel::message
{
public: 
	struct request
	{
		std::string db;
		channel_key_t resp_channel;
	};

	struct result
	{
		channel_key_t channel;
		bool success = true;
		std::size_t rows_processed = 0;
		float execution_time = 0.f;
		std::string error_desc;
		// ...
	};

	using ptr = std::shared_ptr<query>;

public: 
	query(const std::string& db);

	virtual ~query();

	/// 쿼리 실행. resp_channel로 결과 통보
	bool execute(nanodbc::connection& con);

	/// set response channel
	void set_response_channel(const channel_key_t& channel)
	{
		request_.resp_channel = channel;
	}

	/// result 
	const result& get_result() const
	{
		return result_;
	}

protected:
	virtual bool on_execute(nanodbc::connection& con);

private:
	request request_;
	result result_;
};

} // db
} // lax
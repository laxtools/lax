#pragma once 

#include <lax/net/protocol/topics.hpp>
#include <lax/net/message.hpp>

#include <asio.hpp>

namespace lax
{
namespace net
{

/// 연결 실패 전달. 에러 처리는 session::sub로 등록해서 진행
struct sys_connect_failed : public message
{
	std::string addr; 
	asio::error_code ec;

	sys_connect_failed()
		: message(topic_t(topics::groups::sys, topics::sys_keys::connect_failed))
	{
	}
};

/// 연결 받기 실패 전달. 에러 처리는 session::sub로 등록해서 진행
struct sys_accept_failed : public message
{
	std::string addr; 
	asio::error_code ec;

	sys_accept_failed()
		: message(topic_t(topics::groups::sys, topics::sys_keys::accept_failed))
	{
	}
};

/// 세션 준비됨
struct sys_session_ready : public message
{
	sys_session_ready()
		: message(topic_t(topics::groups::sys, topics::sys_keys::session_ready))
	{
	}
};

/// 세션 종료됨
struct sys_session_closed : public message
{
	asio::error_code ec;

	sys_session_closed()
		: message(topic_t(topics::groups::sys, topics::sys_keys::session_closed))
	{
	}
};

} // net
} // lax





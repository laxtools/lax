#pragma once 

#include <lax/net/protocol/topics.hpp>
#include <lax/net/packet.hpp>

#include <asio.hpp>

namespace lax
{
namespace net
{

/// 연결 실패 전달. 에러 처리는 session::sub로 등록해서 진행
struct sys_connect_failed : public packet
{
	MSG_TOPIC(topics::groups::sys, topics::types::connect_failed);
	MSG_CLASS(sys_connect_failed);

	std::string addr; 
	asio::error_code ec;
};

/// 연결 받기 실패 전달. 에러 처리는 session::sub로 등록해서 진행
struct sys_accept_failed : public packet
{
	MSG_TOPIC(topics::groups::sys, topics::types::accept_failed);
	MSG_CLASS(sys_accept_failed);

	std::string addr; 
	asio::error_code ec;
};

/// 세션 준비됨
struct sys_session_ready : public packet
{
	MSG_TOPIC(topics::groups::sys, topics::types::session_ready);
	MSG_CLASS(sys_session_ready);
};

/// 세션 종료됨
struct sys_session_closed : public packet
{
	MSG_TOPIC(topics::groups::sys, topics::types::session_closed);
	MSG_CLASS(sys_session_closed);

	asio::error_code ec;
};

} // net
} // lax

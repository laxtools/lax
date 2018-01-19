#pragma once 

#include <lax/net/protocol/topics.hpp>
#include <lax/net/message.hpp>

#include <asio.hpp>

namespace lax
{
namespace net
{

/// ���� ���� ����. ���� ó���� session::sub�� ����ؼ� ����
struct sys_connect_failed : public message
{
	std::string addr; 
	asio::error_code ec;

	sys_connect_failed()
		: message(topic_t(topics::groups::sys, topics::sys_keys::connect_failed))
	{
	}
};

/// ���� �ޱ� ���� ����. ���� ó���� session::sub�� ����ؼ� ����
struct sys_accept_failed : public message
{
	std::string addr; 
	asio::error_code ec;

	sys_accept_failed()
		: message(topic_t(topics::groups::sys, topics::sys_keys::accept_failed))
	{
	}
};

/// ���� �غ��
struct sys_session_ready : public message
{
	sys_session_ready()
		: message(topic_t(topics::groups::sys, topics::sys_keys::session_ready))
	{
	}
};

/// ���� �����
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





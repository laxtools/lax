#pragma once 

#include <lax/net/protocol/topics.hpp>
#include <lax/net/packet.hpp>

#include <asio.hpp>

namespace lax
{
namespace net
{

/// ���� ���� ����. ���� ó���� session::sub�� ����ؼ� ����
struct sys_connect_failed : public packet
{
	MSG_TOPIC(topics::groups::sys, topics::types::connect_failed);
	MSG_CLASS(sys_connect_failed);

	std::string addr; 
	asio::error_code ec;
};

/// ���� �ޱ� ���� ����. ���� ó���� session::sub�� ����ؼ� ����
struct sys_accept_failed : public packet
{
	MSG_TOPIC(topics::groups::sys, topics::types::accept_failed);
	MSG_CLASS(sys_accept_failed);

	std::string addr; 
	asio::error_code ec;
};

/// ���� �غ��
struct sys_session_ready : public packet
{
	MSG_TOPIC(topics::groups::sys, topics::types::session_ready);
	MSG_CLASS(sys_session_ready);
};

/// ���� �����
struct sys_session_closed : public packet
{
	MSG_TOPIC(topics::groups::sys, topics::types::session_closed);
	MSG_CLASS(sys_session_closed);

	asio::error_code ec;
};

} // net
} // lax

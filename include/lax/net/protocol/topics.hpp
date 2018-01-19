#pragma once 

namespace lax
{
namespace net
{

struct topics
{
	enum groups 
	{
		sys,		// system internal local messages
		bits,		// bits protocol messages if any  
		end = 4,	// end mark. reserved up to end
	};

	enum sys_keys 
	{
		accepted,
		accept_failed, 
		connected,
		connect_failed,
		session_ready, 
		session_closed,
	};

}; // topics

} // net
} // lax
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
		groups_end = 4,	// end mark. reserved up to end
	};

	enum types
	{
		accepted,
		accept_failed, 
		connected,
		connect_failed,
		session_ready, 
		session_closed,
		sys_keys_end
	};

}; // topics

} // net
} // lax
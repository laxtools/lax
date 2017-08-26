#pragma once 

namespace lax
{
namespace actor
{

/// actor 라이브러리의 내부 에러 코드
enum reason
{
	success, 

	fail_null_pointer,
	faill_session_already_bound, 
	fail_session_bind, 
	fail_invalid_actor_id, 
	fail_actor_session_bind, 
	fail_session_not_found, 
	fail_session_closed, 
	fail_no_bound_actor, 
	fail_send_to_session, 
	fail_no_proper_sender,		 
	fail_not_implemented, 
	fail_server_start, 
	fail_to_listen, 
	fail_to_connect


};

} // actor
} // lax

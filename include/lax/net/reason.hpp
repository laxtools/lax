#pragma once

#include <string>

namespace lax
{
namespace net
{

enum reason
{
	success,

	// general
	fail_null_pointer,
	fail_zero_size_data, 
	fail_invalid_parameter, 
	fail_not_implemented, 

	// net
	fail_invalid_session_index, 
	fail_invalid_address,
	fail_acceptor_open, 
	fail_acceptor_bind, 
	fail_acceptor_listen,

	// session
	fail_session_already_recving, 
	success_session_already_sending,
	success_session_no_data_to_send,
	fail_invalid_session,

	fail_socket_closed, 

	// protocol 
	fail_null_message_pointer,
	fail_invalid_message_header,
	fail_protocol_not_added, 
	fail_invalid_bits_message, 
	fail_bits_message_not_registered, 
	fail_bits_unpack_error

};

/// get description from reason
static std::string get_reason_desc(reason r);

} // net 
} // lax

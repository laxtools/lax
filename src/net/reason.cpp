#include "stdafx.h"
#include <lax/net/reason.hpp>

std::string lax::net::get_reason_desc(reason r)
{
	static std::string desc[] = { 
		"success",

		// general
		"fail_null_pointer",
		"fail_zero_size_data",
		"fail_invalid_parameter",
		"fail_not_implemented",

		// net
		"fail_invalid_session_index",
		"fail_invalid_address",
		"fail_acceptor_open",
		"fail_acceptor_bind",
		"fail_acceptor_listen",

		"fail_session_already_recving",
		"success_session_already_sending",
		"success_session_no_data_to_send",

		"fail_socket_closed",
	};

	return desc[static_cast<int>(r)];
}

#pragma once

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

	fail_session_already_recving, 
	success_session_already_sending,
	success_session_no_data_to_send,

	fail_socket_closed, 

	// net::json
	fail_json_parse_error,
	fail_json_build_error_key_not_found, 
	fail_json_build_error_msg_not_found, 
	fail_json_parse_error_key_not_found, 
	fail_json_parse_error_msg_not_found, 
	fail_session_buffer_overflow,

	// msg_pack
	fail_msg_pack_invalid_type,
	fail_msg_pack_domain_type_not_found,
	fail_msg_pack_duplicate_add, 
	fail_msg_pack_serialize, 
	fail_msg_pack_deserialize, 
	fail_msg_pack_decryption, 
	fail_msg_pack_encryption, 
	fail_msg_pack_crc32_invalid, 
};

} // net 
} // lax

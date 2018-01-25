#include "stdafx.h"
#include <lax/net/protocol/util/sequencer.hpp>

namespace lax
{
namespace net
{

modifier::result sequencer::on_bind()
{
	send_seq_ = 1; // sender sends with 1
	recv_seq_ = 1; // receiver expect 1 for next recv

	return result(true, reason::success);
}

modifier::result sequencer::on_recv(
	resize_buffer& buf,
	std::size_t msg_pos,
	std::size_t msg_len, 
	std::size_t& new_len
)
{
	new_len = msg_len;

	// �׻� msg �������� �� ������ �ʵ��� �����ϸ� ��. 
	// ������� msg_len�� �����ϸ鼭 ȣ���� ����

	uint8_t seq = buf.data()[msg_pos + msg_len - sequence_size];
	
	if (seq == recv_seq_++)
	{
		new_len = msg_len - sequence_size;
		return result(true, reason::success);
	}

	return result(false, reason::fail_invalid_message_sequence);
}

modifier::result sequencer::on_send(
	resize_buffer& buf,
	std::size_t msg_pos,
	std::size_t msg_len
)
{
	buf.append("a", sequence_size); // ensure 1 more byte

	buf.data()[msg_pos + msg_len] = send_seq_++;

	update_length(buf, msg_pos, msg_len + sequence_size);

	return result(true, reason::success);
}

} // net
} // lax
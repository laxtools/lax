#pragma once 

#include <lax/net/service.h>
#include <lax/net/protocol/json/msg_json.h>

namespace lax
{
namespace net
{
/// json string protocol session 
/** 
 * usage: 
 *  send flow:
 *   auto m = std::make_shared<msg_json>();
 *   m->build(json_obj);
 * 
 *   session_json::send(m);
 *
 *	recv flow: 
 *	 app::on_recv(id, m); 
 * 
 *  TODO: subscription and posting messages to target actor(s) and/or sessions
 */
class session_json : public session
{
public:
	session_json(
		service& svc,
		const id& id,
		tcp::socket&& soc,
		bool accepted);

	~session_json();

	/// send msg_json
	result send(msg::ptr m);

	/// 프로토콜 처리 테스트를 위해 on_recv() 호출
	result test_recv(const uint8_t* data, std::size_t len);

	/// 디버그, 테스트 용도
	msg::ptr get_last_msg()
	{
		return last_msg_;
	}

	int get_recv_count() const
	{
		return recv_count_;
	}

	int get_send_count() const
	{
		return recv_count_;
	}

	std::size_t get_buf_size() const
	{
		return buf_.size();
	}

private:
	void on_created() override;

	/// accumulate and parse. then pass a message to application
	result on_recv(const uint8_t* data, std::size_t len) override;

	/// nothing to do
	result on_send(uint8_t* data, std::size_t len)  override;

	/// pass an error to application  
	void on_error(const asio::error_code& ec) override;

	/// try to parse from buf
	void parse_json();

	/// build a message from jbegin and jend. returns processed size
	std::size_t build_msg(std::size_t jbegin, std::size_t jend);

private: 
	segment<8 * 1024>	buf_;
	asio::error_code	ec_;
	std::size_t			last_parsed_pos_ = 0;
	msg::ptr			last_msg_;	// to debug

	// 통계
	mutable std::atomic<int>	recv_count_ = 0;
	mutable std::atomic<int>	send_count_ = 0;
};

} // net
} // lax

#pragma once 

#include <lax/net/service.h>
#include <lax/net/protocol/msgpack/msg_pack.h>

namespace lax
{
namespace net
{
/// msgpack binary transfer protocol w/ encryption
/** 
 * length(32) | domain(16) | type(16) | payload 
 *  - length는 length 필드를 포함한 전체 길이
 *  - domaij / type은 애플리케이션에서 결정해서 사용
 *  - payload는 msg_pack 하위 클래스의 내용
 */
class session_pack : public session
{
public: 
	using buffer = msg_pack::buffer;

public:
	session_pack(
		service& svc,
		const id& id,
		tcp::socket&& soc,
		bool accepted);

	~session_pack();

	/// send msg_pack
	/** 
	 * 길이 필드를 추가하고 pack 해서 메세지 전송 
	 * 
	 * mutlicast의 경우:
	 *  - msg_pack::pack()
	 *  - for sessions... 
	 *    - call send(buf.data(), buf.size());
	 */
	result send(msg::ptr m);

	/// 디버그, 테스트 용도
	msg::ptr get_last_msg()
	{
		return last_msg_;
	}

	/// 편의 함수
	template <typename Msg>
	std::shared_ptr<Msg> get_last_msg()
	{
		return std::static_pointer_cast<Msg>(last_msg_);
	}

	/// 수신한 이벤트 개수
	int get_recv_count() const
	{
		return recv_count_;
	}

	/// 송신한 이벤트 개수
	int get_send_count() const
	{
		return recv_count_;
	}

	/// 수신 버퍼 현재 크기
	std::size_t get_buf_size() const
	{
		return buf_.size();
	}

	/// 프로토콜 처리 테스트를 위해 on_recv() 호출. 
	/** 
	 * length 추가 안 하므로 직접 호출 시는 처리해야 함
	 */
	result test_recv(const uint8_t* data, std::size_t len);

	/// 편의 함수
	result test_recv(const char* data, std::size_t len)
	{
		return test_recv((const uint8_t*)data, len);
	}

	/// 메세지 오브젝트에서 받은 것처럼 처리해서 테스트 진행
	template <typename Msg> result test_recv(Msg& m, buffer& buf); 

private:
	/// called when created
	void on_created() override;

	/// accumulate and parse. then pass a message to application
	result on_recv(const uint8_t* data, std::size_t len) override;

	/// nothing to do
	result on_send(uint8_t* data, std::size_t len)  override;

	/// pass an error to application  
	void on_error(const asio::error_code& ec) override;

	/// 버퍼에서 메세지들 추출
	void parse_msgs();

private: 
	const uint32_t header_length = 4 + 4;		// len, key
	const uint32_t max_packet_length = 256 * 1024;	// 256K MAX. TODO: make it configurable

	msg_pack::buffer	buf_;
	asio::error_code	ec_;
	msg::ptr			last_msg_;	// to debug

	// 통계
	mutable std::atomic<int>	recv_count_ = 0;
	mutable std::atomic<int>	send_count_ = 0;
};

/// inline / template 구현

template <typename Msg>
session_pack::result session_pack::test_recv(Msg& m, buffer& buf)
{
	uint32_t len = 0;
	uint32_t key = m.get_key();

	// reserve space
	buf.write((const char*)&len, sizeof(len));
	buf.write((const char*)&key, sizeof(key));

	msgpack::pack(buf, m);

	len = static_cast<uint32_t>(buf.size());

	// endian-safe write
	char* dp = buf.data();
	dp[3] = len & 0xff;
	dp[2] = (len >> 8) & 0xff;
	dp[1] = (len >> 16) & 0xff;
	dp[0] = (len >> 24) & 0xff;

	dp[7] = key & 0xff;
	dp[6] = (key >> 8) & 0xff;
	dp[5] = (key >> 16) & 0xff;
	dp[4] = (key >> 24) & 0xff;

	return result(true, reason::success);
}

} // net
} // lax

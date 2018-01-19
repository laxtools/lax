#pragma once 

#include <lax/channel/channel.hpp>
#include <lax/net/protocol/protocol.hpp>
#include <lax/net/reason.hpp>
#include <lax/net/message.hpp>
#include <lax/net/detail/buffer/segment_buffer.hpp>
#include <lax/util/macros.hpp>
#include <lax/util/result.hpp>

#include <memory>
#include <mutex>

using namespace asio::ip;

namespace lax
{
namespace net
{

class service;

/// socket wrapper to use with asio
/** 
 * 바이트 송수신 처리 세션. 
 * - 각 프로토콜 처리는 protocol 클래스에서 처리
 *
 */
class session final : public std::enable_shared_from_this<session>
{
public:
	using ptr = std::shared_ptr<session>;
	using result = util::result<bool, reason>;
	using key_t = channel::sub::key_t;
	using cond_t = channel::channel::cond_t;
	using cb_t = channel::channel::cb_t;

	friend class protocol; // to use send and error

public:
	union id
	{
		uint32_t value_ = 0;

		struct
		{
			uint16_t seq_;
			uint16_t index_;
		};

		explicit id(uint32_t value = 0);
		id(uint16_t seq, uint16_t index);

		/// seq_ > 0 
		bool is_valid() const;

		const uint32_t get_value() const;
		const uint16_t get_seq() const;
		const uint16_t get_index() const;

		bool operator==(const id& rhs) const;
		bool operator!=(const id& rhs) const;
		bool operator < (const id& rhs) const;
	};

	/// ref to session to use it for send, close callback
	class ref
	{
	public:
		ref(ptr ss = ptr());

		result send(message::ptr m);

		bool is_valid() const;

		const std::string& get_desc() const;

		void close();

	private:
		ptr session_;
	};

public:
	/// setup 
	session(
		const id& id,
		tcp::socket&& soc,
		bool accepted,
		const std::string& protocol);

	/// clean up 
	virtual ~session();

	/// subscribe to a topic with a condition 
	/**
	* direct mode subscription only. message is posted immediately
	*/
	static key_t sub(
		const message::topic_t& topic,
		cond_t cond,
		cb_t cb
	);

	/// subscribe to a topic without condition 
	/**
	* direct mode subscription only. message is posted immediately
	*/
	static key_t sub(
		const message::topic_t& topic,
		cb_t cb
	);

	/// unsubscribe 
	static bool unsub(key_t key);

	/// 만들어진 메세지를 channel로 전송
	static void push(message::ptr m);

	/// send through a protocol. call following send
	result send(message::ptr m);

	/// close socket (shutdown and close) 
	void close();

	/// check 
	bool is_open() const
	{
		return socket_.is_open();
	}

	/// get internal id
	const id& get_id() const;

	/// local endpoint address
	const std::string& get_local_addr() const
	{
		return local_addr_;
	}

	/// remote endpoint address
	const std::string& get_remote_addr() const
	{
		return remote_addr_;
	}

	const std::string& get_desc() const
	{
		return desc_;
	}

	/// accept 여부
	bool is_accepted() const
	{
		return accepted_;
	}

protected:
	// get last error (for debug / test purpose)
	const asio::error_code& get_last_error() const
	{
		return last_error_;
	}

private:
	/// send a message to socket
	result send(uint8_t* data, std::size_t len);


	/// 에러 처리 함수
	void error(const asio::error_code& ec);

	/// recv request
	result request_recv();

	/// send request
	result request_send();

	/// callback on recv
	void on_recv_completed(asio::error_code& ec, std::size_t len);

	/// callback on send 
	void on_send_completed(asio::error_code& ec, std::size_t len);

private:
	using segment_buffer = segment_buffer<32 * 1024>;
	using seg = typename segment_buffer::seg;

	static channel::channel			channel_;		/// channel to communicate msgs

	tcp::socket						socket_;
	id								id_;
	bool							accepted_ = false;
	protocol::ptr					protocol_;
	std::string						local_addr_;
	std::string						remote_addr_;
	std::string						desc_;

	std::recursive_mutex			session_mutex_;
	bool							recving_ = false;
	bool							sending_ = false;
	std::array<uint8_t, 32 * 1024>	recv_buf_;

	std::recursive_mutex			send_segs_mutex_;
	segment_buffer					send_buffer_;
	std::size_t						send_request_size_ = 0;
	std::vector<seg*>				sending_segs_;
	std::vector<asio::const_buffer> sending_bufs_;

	asio::error_code				last_error_;
};

} // net
} // lax

#include "session.inl"

#include <unordered_map>

  // hash function for id
namespace std
{
template <>
struct hash<lax::net::session::id>
{
	std::size_t operator() (const lax::net::session::id& id) const
	{
		return std::hash<std::size_t>()(id.get_value());
	}
};
}

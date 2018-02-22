#pragma once 

#include <lax/net/detail/subs_close.hpp>
#include <lax/channel/channel.hpp>
#include <lax/net/protocol/protocol.hpp>
#include <lax/net/reason.hpp>
#include <lax/net/packet.hpp>
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

		struct full
		{
			uint16_t seq_;
			uint16_t index_;
		} full_;

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

		~ref();

		// TODO: add move constructor, operator

		/// send through a protocol
		result send(packet::ptr m);

		/// subscribe for close callback
		bool sub_close(cb_t cb);

		/// close session
		void close();

		/// check validity 
		bool is_valid() const;

		/// get desc
		const std::string& get_desc() const;

		/// get id to keept in map or hash_map
		packet::sid_t get_id() const
		{
			return session_->get_id().get_value();
		}

		// operators: ==, !=, <, > defined

	private:
		ptr session_;
		key_t key_ = 0;
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
	* direct mode subscription only. packet is posted immediately
	*/
	static key_t sub(
		const packet::topic_t& topic,
		cond_t cond,
		cb_t cb
	);

	static key_t sub(
		const packet::topic_t::key_t key,
		cond_t cond,
		cb_t cb
	)
	{
		return sub(packet::topic_t(key), cond, cb);
	}

	/// subscribe to a topic without condition 
	/**
	* direct mode subscription only. packet is posted immediately
	*/
	static key_t sub(
		const packet::topic_t& topic,
		cb_t cb
	);

	static key_t sub(
		const packet::topic_t::key_t key,
		cb_t cb
	)
	{
		return sub(packet::topic_t(key), cb);
	}

	/// unsubscribe 
	static bool unsub(key_t key);

	/// 만들어진 메세지를 channel로 전송
	static void post(packet::ptr m);

	/// session::id에 대해 통지
	static key_t sub_close(
		subs_close::sid_t id, 
		subs_close::cb_t cb
	);

	/// always called from ref when subscribed
	static void unsub_close(key_t key);

	/// send through a protocol. call following send
	result send(packet::ptr m);

	/// 미리 serialize 된 패킷을 보낸다. m은 메세지 생략 못 하도록 추가 
	result send(packet::ptr m, const uint8_t* data, std::size_t len);

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
	/// send a packet to socket
	result send(const uint8_t* const data, std::size_t len);

	/// 에러 처리 함수
	void error(const asio::error_code& ec);

	/// close외에 추가 개념. close되고 send/recv 없을 때 가능. 통지까지
	void destroy(const asio::error_code& ec = asio::error::shut_down);

	/// recv request
	result request_recv();

	/// send request
	result request_send();

	/// callback on recv
	void on_recv_completed(asio::error_code& ec, std::size_t len);

	/// callback on send 
	void on_send_completed(asio::error_code& ec, std::size_t len);

	/// 준비됨 통지
	void notify_session_ready();

	/// 메세지 통지
	void notify_session_closed(const asio::error_code& ec);

private:
	using segment_buffer = segment_buffer<32 * 1024>;
	using seg = typename segment_buffer::seg;

	static channel::channel			channel_;		/// channel to communicate msgs
	static subs_close				subs_close_;

	tcp::socket						socket_;
	id								id_;
	bool							accepted_ = false;
	protocol::ptr					protocol_;
	std::string						local_addr_;
	std::string						remote_addr_;
	std::string						desc_;

	std::recursive_mutex			session_mutex_;
	bool							recving_ = false; /// sending과 recving은 항상 session_mutex_ 걸고 처리
	bool							sending_ = false;
	std::array<uint8_t, 32 * 1024>	recv_buf_;

	std::recursive_mutex			send_segs_mutex_;
	segment_buffer					send_buffer_;
	std::size_t						send_request_size_ = 0;
	std::vector<seg*>				sending_segs_;
	std::vector<asio::const_buffer> sending_bufs_;

	asio::error_code				last_error_;
	std::atomic<bool>				destroyed_ = false;
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

#pragma once 

#include <lax/util/macros.hpp>
#include <lax/util/result.hpp>
#include <lax/net/reason.hpp>
#include <lax/net/message.hpp>
#include <lax/net/detail/buffer/send_buffer.hpp>

#include <asio.hpp>
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
 * ����Ʈ �ۼ��� ó�� ����. 
 * �� �������� ó�� ������ session���� ��� �޾Ƽ� ����. 
 * 
 * ������ ��: 
 *  - �����ڴ� unique �� �ɸ� ���¿��� ȣ���. 
 *     - �ʿ��� ��� on_created���� �ʱ�ȭ ���� ���� 
 */
class session
{
public:
	using ptr = std::shared_ptr<session>;
	using result = util::result<bool, reason>;

public:
	union id
	{
		uint32_t value_ = 0;

		struct
		{
			uint16_t seq_;
			uint16_t index_;
		};

		explicit id(uint32_t value = 0)
			: value_(value)
		{
		}

		id(uint16_t seq, uint16_t index)
			: seq_(seq)
			, index_(index)
		{
			check(is_valid());
		}

		/// seq_ > 0 
		bool is_valid() const;

		const uint32_t get_value() const;

		const uint16_t get_seq() const;

		const uint16_t get_index() const;

		bool operator==(const id& rhs) const
		{
			return value_ == rhs.value_;
		}

		bool operator!=(const id& rhs) const
		{
			return value_ != rhs.value_;
		}

		bool operator < (const id& rhs) const
		{
			return value_ < rhs.value_;
		}
	};

public:
	/// setup 
	session(
		const id& id,
		service& svc,
		tcp::socket&& soc,
		bool accepted);

	/// clean up 
	virtual ~session();

	/// send a message to socket
	result send(uint8_t* data, std::size_t len);

	/// close socket (shutdown and close) 
	void close();

	/// check 
	bool is_open() const
	{
		return socket_.is_open();
	}

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

	/// accept ����
	bool is_accepted() const
	{
		return accepted_;
	}

protected:
	/// service ��������. 
	service& get_svc();

	// get last error (for debug / test purpose)
	const asio::error_code& get_last_error() const
	{
		return last_error_;
	}

private:
	/// ���� ó�� �Լ�
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
	using segs = send_buffer<32 * 1024>;	// TODO: size configurable
	using seg = typename segs::seg;

	service&						svc_;
	tcp::socket						socket_;
	id								id_;
	bool							accepted_ = false;
	std::string						local_addr_;
	std::string						remote_addr_;

	std::recursive_mutex			session_mutex_;
	bool							recving_ = false;
	bool							sending_ = false;
	std::array<uint8_t, 32*1024>	recv_buf_;	// TODO: size configurable

	std::recursive_mutex			send_segs_mutex_;
	segs							send_segs_;
	std::size_t						send_request_size_ = 0;
	std::vector<segs::seg*>			sending_segs_;
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

#include "stdafx.h"

#include <catch.hpp>
#include <functional>
#include <memory>

#define ASIO_STANDALONE
#include <asio.hpp>

namespace
{

class service;


class msg
{
public:
	using ptr = std::shared_ptr<msg>;

public:
	msg(); 
	virtual ~msg();

	// buffer 구현 후에 구현. 각 프로토콜에서 구현
	// msg_json에서 먼저 테스트 
	virtual bool pack() = 0;
	virtual bool unpack() = 0;
};

class session 
{
public: 
	using ptr = std::shared_ptr<session>;

	struct listener
	{
		virtual void notify(msg::ptr m) = 0;
	};

	struct id
	{
		union
		{
			uint64_t value = 0;

			struct
			{
				uint32_t seq = 0;
				uint16_t index = 0;
			};
		};

		id() = default; 
		id(uint32_t sq, uint16_t idx)
			: seq(sq)
			, index(idx)
		{}

		bool valid() const
		{
			return seq > 0 && index >= 0;
		}

		const uint64_t value() const
		{
			return value;
		}

		const uint32_t seq() const
		{
			return seq;
		}
	};

public: 
	session(service* svc, asio::ip::tcp::socket soc);

	virtual int send(msg::ptr m);

private: 
	service* svc_;
	asio::ip::tcp socket_;
};

class proto
{
public: 
	using ptr = std::shared_ptr<proto>;

	virtual session::ptr create() = 0;
};

class service
{
public:
	using creator = std::function <session::ptr(asio::ip::tcp::socket soc)>;

public: 
	service();
	~service();

	int listen(uint16_t port, proto::ptr pro);
	int connect(const std::string& addr);
	int send(session::id id, msg::ptr m);
	int close(session::id id);
};

} // noname

TEST_CASE("design slim net")
{
	

	SECTION("implementation")
	{
		


		SECTION("service")
		{

		}

		SECTION("session")
		{
			
		}

		SECTION("buffer")
		{
			// accumulate and send 
			// vector version buffer
		}
	}

	SECTION("direction")
	{
		SECTION("session")
		{
			// session derived class
			// - session_json for example 
			// - concurrent queue to send 

			// msg
			// - session
			// - recv_ep 

			// service::send( id, msg_ptr )	
			//  session::send( msg_ptr ) 
			//   vec_.push(msg_ptr)
			//   async_send(vec_)
		}

		SECTION("service")
		{
			// manage sessions 
			// - listen / connect
			// - create with a provided proto
			// - cleanup on error and close (destructor can do more cleanup)

			// service는 singleton이 아니다. 

			// session management 

			// detail::acceptor 
			//   - socket, asio::ip::tcp::acceptor, creator function
			//   - create a sessin and pass it to service

			// detail::connector
			//   - socket, asio::ip::tcp::acceptor, creator function
			//   - create a sessin and pass it to service

			// session( service& svc, socket skt )
			// - set_listener(svc.get_listener());
			// session::on_accepted()
			// session::on_connected()
			// session::on_error() 
			//  - svc.reset(id()) 

			// rw lock을 사용. 
			// - vector<session_ptr> 을 사용 
			// - write lock 
			//   - resize()
			//   - vector[index].reset()

			// send 등에서 read lock	 
			// - vector의 container 락이다. 
		}
	}
}
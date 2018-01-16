#pragma once 

#include <lax/channel/channel.hpp>
#include <lax/net/message.hpp>
#include <lax/net/session.hpp>
#include <lax/util/result.hpp>

namespace lax
{
namespace net
{

class protocol
{
public: 
	using ptr = std::shared_ptr<protocol>;
	using key_t = channel::channel::key_t;
	using cond_t = channel::channel::cond_t;
	using cb_t = channel::channel::cb_t;

	friend class session;

	using result = util::result<bool, reason>;

	/// ref to protocol to use it for send, close callback
	class ref 
	{
	public: 
		
	};

	/// constructor
	protocol(session::ptr ss);

	/// destructor
	virtual ~protocol();

	/// subscribe to a topic with a condition 
	/** 
	 * direct mode subscription only. message is posted immediately
	 */
	key_t sub(
		const message::topic_t& topic,
		cond_t cond,
		cb_t cb
	);

	/// subscribe to a topic without condition 
	/** 
	 * direct mode subscription only. message is posted immediately
	 */
	key_t sub(
		const message::topic_t& topic,
		cb_t cb
	);

	/// unsubscribe 
	bool unsub(key_t key);

	/// acquire ref to a protocol, which has a session inside 
	ref acquire_new_ref() const;	

	/// used to log remote:sid:socket
	virtual const std::string& get_desc() const;

	/// send to a session after processing message
	virtual result send(message::ptr m) = 0; 

protected: 
	/// 만들어진 메세지를 channel로 전송
	void post(message::ptr m);

private:
	/// session calls this when received data
	virtual void on_recv(uint8_t* bytes, std::size_t len) = 0;

	/// session calls this when sent data 
	virtual void on_send(std::size_t len) = 0;

	/// session calls this when error ocurrs
	virtual void on_error() = 0;

private:
  	static channel::channel channel_; /// channel to communicate msgs
	session::ptr			session_; /// session
};

} // net 
} // lax

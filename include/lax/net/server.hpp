#pragma once 

#include <lax/net/addr.hpp>
#include <lax/net/session.hpp>
#include <string>

namespace lax
{
namespace net
{

/// server
/** 
 * interface to start networking app. 
 * - starts listening on protocols configured in xml 
 * - tries to connect to other servers configured in xml
 * 
 * manages other servers 
 * - heartbeat
 * - state check
 */
class server 
{
public: 
	struct config
	{
		std::string xml;
		bool install_seh_handler = true;
	};

	struct peer
	{
		int				id; 
		addr			location;
		session::ref	ref;

		bool is_valid() const
		{
			return id > 0 && ref.is_valid();
		}
	};

public:
	static server& inst();

	virtual ~server();

	/// initialize 
	bool init(const config& cfg, bool auto_reconnect);

	/// called from outside
	void run();

	/// add another server with addr
	bool add(int id, const addr& peer);

	/// finish
	void fini();

	/// acquire peer to communicate
	peer acquire(int id) const;

	/// check server aquaintance
	bool is_up(int id) const;

	/// do i know the peer 
	bool is_known(int id) const;

	/// ! is_up() && ! is_known()
	bool is_down(int id) const;

protected:
	virtual bool on_init() = 0;

	virtual void on_run() = 0;

	virtual void on_fini() = 0;

protected: 
	// peers (protected with a lock)
	// 
};

} // net
} // lax 
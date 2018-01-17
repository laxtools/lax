#pragma once 

#include <lax/net/addr.hpp>
#include <lax/net/session.hpp>
#include <string>

namespace lax
{
namespace net
{

/// client
/**
* interface to start networking app.
* - just an interface 
* - do what you want inside
*/
class client
{
public:
	struct config
	{
		std::string xml;
	};

	struct server
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
	static client& inst();

	virtual ~client();

	/// initialize 
	bool init(const config& cfg);

	/// called from outside
	void run();

	/// add server with addr. starts connecting
	bool add(int id, const addr& peer, bool auto_reconnect);

	/// finish
	void fini();

	/// acquire peer to communicate
	server acquire(int id) const;

	/// check client aquaintance
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
	// servers (protected with a lock)
	// 
};

} // net
} // lax 
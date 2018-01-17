#pragma once 

#include <lax/net/addr.hpp>
#include <lax/net/session.hpp>
#include <string>

namespace lax
{
namespace net
{

/// node
/** 
 * interface to start networking app. 
 * - starts listening on protocols configured in xml 
 * - tries to connect to other servers configured in xml
 * 
 * manages other servers 
 * - heartbeat
 * - state check
 */
class node 
{
public: 
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
	static node& inst();

	virtual ~node();

	/// initialize 
	bool init();

	/// called from outside
	void run();

	/// add another node with addr
	bool add(int id, const addr& peer);

	/// finish
	void fini();

	/// acquire peer to communicate
	peer acquire(int id) const;

	/// check node aquaintance
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
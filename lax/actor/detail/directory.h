#pragma once 

#include <lax/actor/detail/ref.h>
#include <lax/actor/detail/index.h>
#include <lax/actor/detail/multi_index.h>

#include <vector>

namespace lax
{
namespace actor
{

/// actor directory for local, remote, child and client
/**
 * usage: 
 *   auto world_runners = dir.find( world_runner ); 
 *   ... find a proper world runner 
 *   auto my_world_ = world_runners[3];
 *
 *   use the saved ptr to further process. 
 *   thread-safety: 
 *     - per cluster_actor only 
 * 
 */
class directory
{
public: 
	using refs = std::vector<ref::ptr>;

	directory() = default; 
	~directory() = default; 

	/// update entry. add if not exist.  
	bool update(ref::ptr ptr);

	/// find a list of ref with a role
	ref::ptr find(id aid) const;

	/// find a list of ref with a role. a little slow. save the ptr to re-use.
	refs find_role(ref::role role) const;

	/// find a node
	ref::ptr find_node(ref::node node) const;
	
private: 
	using map = std::unordered_map<id, ref::ptr>;
	using node_index = index<ref::node, id>;
	using role_index = multi_index<ref::role, id>;

private:
	map dir_;
	node_index node_index_;
	role_index role_index_;
};

} // actor 
} // lax

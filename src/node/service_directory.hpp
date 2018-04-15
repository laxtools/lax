#pragma once

namespace lax
{
namespace server
{

/// service directory manages services for all cluster
/** 
 * manages local and remote service reference
 * - adding a new entry 
 * - changing state 
 * - post state message to references
 *
 * usage: 
 * - 
 */
class service_directory final
{
public: 
	service_directory(); 

	~service_directory();

	//
	// acquire / find / 
	// use ref to communicate 
	// 
	// bcast, mcast to all or some role(s)
	//

private: 
	// index: role based 
	//
};

} // server
} // lax
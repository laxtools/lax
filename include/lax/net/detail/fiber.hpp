#pragma once 

#include <lax/net/msg.hpp>
#include <lax/util/result.hpp>

namespace lax
{
namespace net
{

/// �ٹ߿��� ���� ����. ��� ä��.
/** 
 * 
 */
class fiber
{
public: 
	enum error
	{
		success, 

	};

	using result = util::result<bool, error>;
	
public: 
	fiber(); 
	~fiber();

	result send(msg::ptr m);
};

} // net
} // lax
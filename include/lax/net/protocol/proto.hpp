#pragma once 

#include <lax/channel/channel.hpp>

namespace lax
{
namespace net
{

class proto
{
public: 
	using ptr = std::shared_ptr<proto>;

	/// ref to proto to use it for send, close callback
	class ref 
	{
	public: 
		
	};

	struct result 
	{
	 	enum error 
		{
		}

		result(); 

		result(error e); 

		bool operator bool() const 
		{
		}
	};


	/// channel to communicate msgs
  	static channel hub; 

	proto(); 

	virtual ~proto();

	ref get_ref() const;	

	result send(msg::ptr m); 

	virtual ... on_recv(bytes );
	virtual ... on_send(bytes );
};

} // net 
} // lax

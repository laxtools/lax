#pragma once 

#include <lax/net/session.hpp>
#include <lax/net/reason.hpp>
#include <lax/net/protocol_creator.hpp>

#include <functional>

namespace lax
{
namespace net
{
class service_impl;

class service
{
public: 
	using result = util::result<bool, reason>;

public:
	service();
	~service();

	/// addr에서 listen. addr은 ip:port 형식. 
	result listen(const std::string& addr, protocol_creator::ptr c);

	/// connect to a addr. addr은 ip:port 형식
	result connect(const std::string& addr, protocol_creator::ptr c);

	/// session에서 에러 발생 시 호출
	void error(const session::id& id);

	/// internal use only
	service_impl* impl() { return impl_.get(); }

private: 
	std::unique_ptr<service_impl>	impl_;
};

} // net
} // lax

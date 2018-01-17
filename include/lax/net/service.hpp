#pragma once 

#include <lax/net/session.hpp>
#include <lax/net/reason.hpp>

#include <functional>

namespace lax
{
namespace net
{
class service_impl;

/// a singleton to provide networking service
class service final
{
public: 
	using result = util::result<bool, reason>;

public:
	static service& inst();

	~service();

	/// addr에서 listen. addr은 ip:port 형식. 
	result listen(const std::string& addr, const std::string& protocol);

	/// connect to a addr. addr은 ip:port 형식
	result connect(const std::string& addr, const std::string& protocol);

	/// get a ref to send and subscribe for close / error
	session::ref acquire(const session::id& id);

	/// session에서 에러 발생 시 호출
	void error(const session::id& id);

	/// internal use only
	service_impl* impl() { return impl_.get(); }

private: 
	service();

private: 
	std::unique_ptr<service_impl>	impl_;
};

} // net
} // lax

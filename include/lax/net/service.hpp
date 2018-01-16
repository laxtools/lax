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

	/// addr���� listen. addr�� ip:port ����. 
	result listen(const std::string& addr, protocol_creator::ptr c);

	/// connect to a addr. addr�� ip:port ����
	result connect(const std::string& addr, protocol_creator::ptr c);

	/// session���� ���� �߻� �� ȣ��
	void error(const session::id& id);

	/// internal use only
	service_impl* impl() { return impl_.get(); }

private: 
	std::unique_ptr<service_impl>	impl_;
};

} // net
} // lax

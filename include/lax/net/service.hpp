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
/** 
 * usage: 
 *  - init, fini to start and stop
 *  - listen to start listen on an address with a protocol 
 *  - connect to connect to an address with a protocol 
 *  - session::sub for messages 
 *    - sys_session_ready / sys_session_closed for sessions
 *    - sys_connect_failed to detect failure to connect to servers   
 *  - acquire to get a session::ref 
 *    - this is the only way to send to a session
 *    - id based access becomes hell in communication
 *      - lots of timing / duration issues with id reuse
 *    - ref::send, ref::close
 *  - session::sub for a session close through session::ref
 */
class service final
{
public: 
	using result = util::result<bool, reason>;

	struct config
	{
		bool use_hardware_concurreny = true;
		int concurreny_level = 1;				/// core개수 사용하지 않을 경우
		bool enable_detail_log = false;			/// 자세한 로그 남길지 여부
	};

	static config cfg;

public:
	static service& inst();

	~service();

	/// start
	bool start();

	/// finish
	void finish();

	/// wait for ms
	void wait(unsigned int ms);

	/// addr에서 listen. addr은 ip:port 형식. 
	result listen(const std::string& addr, const std::string& protocol);

	/// connect to a addr. addr은 ip:port 형식
	result connect(const std::string& addr, const std::string& protocol);

	/// get a ref to send and subscribe for close / error
	session::ref acquire(const session::id& id);

	/// check whether serivce is started
	bool is_running() const;

	/// get acceptor count. 
	uint16_t get_acceptor_count() const;

	/// get connector count. 
	uint16_t get_connector_count() const;

	/// get # of sessions (for debug and test)
	std::size_t get_session_count() const;

	/// session에서 에러 발생 시 호출
	void error(const session::id& id);

	/// internal use only
	service_impl& impl() { return *impl_.get(); }

private: 
	service();

private: 
	std::unique_ptr<service_impl>	impl_;
};

} // net
} // lax

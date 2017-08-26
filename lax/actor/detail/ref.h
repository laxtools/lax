#pragma once 

#include <lax/actor/id.h>
#include <lax/net/session.h>
#include <lax/util/json.hpp>

namespace lax
{
namespace actor
{

/// 액터 참조, 주소, sub 기반으로 모든 통신이 가능하다.
/**
 * app actor들은 부모 액터내에서만 활동한다. 
 *  - 직접 주소로 통신하지 않는다
 *  - cluster actor인 부모가 post 한다. 
 *
 * feature: 
 *  - id (cluster)
 *  - role 
 *  - state  (up / down)
 *  - data (json object. holds more data)
 * 
 * msg_actor_state
 *  - up, down
 *  - data (json string)
 *  - broadcast to all cluster actors
 */
class ref
{
public:
	using ptr = std::shared_ptr<ref>;
	using node = uint16_t; 
	using role = uint16_t;

	enum state : uint8_t
	{
		unknown,
		up, 
		down, 
	};

	/// 액터 아이디만 갖고 초기화
	explicit ref(id actor_id)
		: id_(actor_id)
	{
	}

	/// 클러스터 액터 정보 업데이트. state::start로 오면 시작 시간 기록
	void update(float tick, const net::session::id& sid, role role, state astatus)
	{
		if (sid != sid_)
		{
			// TODO: rebind 처리. 
		}

		check(sid.is_valid());
		check(tick > 0.f);
		check(tick > this->get_last_update_tick());

		update_tick_ = tick;
		sid_ = sid;
		role_ = role;
		state_ = astatus;

		if (astatus == state::up)
		{
			this->up_tick_ = tick;
		}

		if (astatus == state::down)
		{
			this->down_tick_ = tick;
		}
	}

	/// 살아 있는 지 확인. ttl은 초 단위. 
	void update(float current_tick, float ttl = 30.f) 
	{
		check(current_tick > 0);
		check(update_tick_ < current_tick);
		check(ttl > 0.f);

		// ttl 동안 응답 없으면 down 
		if (current_tick > update_tick_ + ttl)
		{
			state_ = state::down;
		}
	}

	id get_id() const
	{
		return id_;
	}

	net::session::id get_session_id() const
	{
		return sid_;
	}

	/// 마지막 업데이트 시간
	float get_last_update_tick() const
	{
		return update_tick_;
	}

	/// 역할
	role get_role() const
	{
		return role_;
	}

	/// 상태. ttl 내에 start / up 받으면 alive 
	state get_state() const
	{
		return state_;
	}

	/// 살아 있는 지 확인. ttl은 초 단위. 
	bool is_up(float current_tick, float ttl = 30.f) const 
	{
		return state_ == state::up;
	}

	/// 서버 살아있었던 시간
	float get_up_time() const
	{
		return update_tick_ - up_tick_;
	}

	/// disable copy / assignment. use ptr to share
	ref(const ref& rhs) = delete;
	ref& operator=(ref& rhs) = delete;

private:
	id id_;
	net::session::id sid_;

	float up_tick_ = 0.f;
	float down_tick_ = 0.f;
	float update_tick_ = 0.f;

	role role_ = 0;
	state state_ = state::unknown;

	// 28 bytes. keep it small. 
};

} // actor
} // lax

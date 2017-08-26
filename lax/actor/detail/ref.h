#pragma once 

#include <lax/actor/id.h>
#include <lax/net/session.h>
#include <lax/util/json.hpp>

namespace lax
{
namespace actor
{

/// ���� ����, �ּ�, sub ������� ��� ����� �����ϴ�.
/**
 * app actor���� �θ� ���ͳ������� Ȱ���Ѵ�. 
 *  - ���� �ּҷ� ������� �ʴ´�
 *  - cluster actor�� �θ� post �Ѵ�. 
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

	/// ���� ���̵� ���� �ʱ�ȭ
	explicit ref(id actor_id)
		: id_(actor_id)
	{
	}

	/// Ŭ������ ���� ���� ������Ʈ. state::start�� ���� ���� �ð� ���
	void update(float tick, const net::session::id& sid, role role, state astatus)
	{
		if (sid != sid_)
		{
			// TODO: rebind ó��. 
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

	/// ��� �ִ� �� Ȯ��. ttl�� �� ����. 
	void update(float current_tick, float ttl = 30.f) 
	{
		check(current_tick > 0);
		check(update_tick_ < current_tick);
		check(ttl > 0.f);

		// ttl ���� ���� ������ down 
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

	/// ������ ������Ʈ �ð�
	float get_last_update_tick() const
	{
		return update_tick_;
	}

	/// ����
	role get_role() const
	{
		return role_;
	}

	/// ����. ttl ���� start / up ������ alive 
	state get_state() const
	{
		return state_;
	}

	/// ��� �ִ� �� Ȯ��. ttl�� �� ����. 
	bool is_up(float current_tick, float ttl = 30.f) const 
	{
		return state_ == state::up;
	}

	/// ���� ����־��� �ð�
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

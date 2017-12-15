#pragma once 

#include <lax/util/macros.hpp>

#include <cassert>
#include <map>
#include <memory>
#include <vector>

namespace lax
{
namespace util
{

// forward decl.
template <typename Key> class state_machine;

/// 상태기계와 함께 동작할 클래스 
/** 
 * 클래스 기반 상태 기계의 상태 추상 클래스로 
 * on_enter, on_execute, on_exit를 재정의 하여 사용한다. 
 */
template <typename Key>
class state : public std::enable_shared_from_this<state<Key>>
{
public:

	typedef state_machine<Key>					state_machine;
	typedef std::shared_ptr<state<Key>>			state_ptr;
	typedef std::map<Key, state_ptr>			state_map;

	friend class state_machine;		// to access internal impl.

	/// 상태 전환을 위한 동작 값 갖는 노드
	struct transition_node
	{
		enum code 
		{
			noop = 1			// nothing to do
			, exit				// exit the state
			, enter				// enter the state
			, target			// target state
			, target_noop		// target이 이미 실행 중일 때
		};

		code		code_;
		state_ptr	state_;
	};

	/// 간단한 디버깅용 통계 정보. overflow시 0으로 초기화
	struct stats
	{
		uint32_t enter_count_		= 0;
		uint32_t exit_count_		= 0;
		uint32_t execution_count_	= 0;

		uint32_t enter_count() const { return enter_count_; }
		uint32_t exit_count() const { return exit_count_; }
		uint32_t execution_count() const { return execution_count_; }
	};

public: 
	state(Key key, state_machine& machine)
		: machine_(machine)
		, key_(key)
	{
	}

	virtual ~state()
	{
	}

	bool add(state_ptr state)
	{
		check(state);
		return_if(!state, false);

		check(!has(state->key()));
		return_if(has(state->key()), false);

		childs_.insert(state_map::value_type(state->key(), state));

		state->parent(shared_from_this());

		return true;
	}

	void enter()
	{
		check(!active());
		active_ = true;

		if (parent_)
		{
			check(parent_->active());
			check(parent_->active_child_.get() != this);

			parent_->active_child_ = shared_from_this();
		}

		stats_.enter_count_++;

		on_enter();
	}

	/// 상태 실행 처리
	void execute()
	{
		check(active());
		return_if(!active());

		on_execute();

		if (active_child_)
		{
			active_child_->execute();
		}

		stats_.execution_count_++;
	}

	/// 상태 나가기 처리
	void exit()
	{
		check(active());

		active_ = false;

		if (parent_)
		{
			if (parent_->active_child_.get() == this)
			{
				parent_->active_child_.reset();
			}
		}

		on_exit();

		stats_.exit_count_++;
	}

public: // accessors 

	state_machine& machine() const 
	{
		return machine_;
	}

	state_ptr parent()
	{
		return parent_;
	}

	state_ptr find(Key key) 
	{
		if (key_ == key)
		{
			return shared_from_this();
		}

		for (auto& pair : childs_)
		{
			auto target = pair.second->find(key);

			if (target)
			{
				return target;
			}
		}

		return state_ptr();
	}

	bool has(Key key) const
	{
		if (key_ == key)
		{
			return true;
		}

		for (auto& pair : childs_)
		{
			if (pair.second->has(key))
			{
				return true;
			}
		}

		return false;
	}

	Key key() const
	{
		return key_;
	}

	bool active() const
	{
		return active_;
	}

	const stats& stat() const
	{
		return stats_;
	}

protected: 
	virtual void on_enter() {}
	virtual void on_execute() {}
	virtual void on_exit() {}

	void parent(state_ptr st)
	{
		parent_ = st;
	}

	state_ptr parent() const
	{
		return parent_;
	}

	bool get_transition_path(Key target, std::vector<transition_node>& path);
	bool get_child_path(Key target, std::vector<transition_node>& path);

private: 
	state_machine&			machine_;
	state_map				childs_;
	Key						key_			= Key();
	state_ptr				parent_;
	bool					active_			= false;
	state_ptr				active_child_;
	stats					stats_;
};


/// 목표 상태로 가는 전환 경로 가져오기. StateMachine 내부 함수
template <typename Key>
bool state<Key>::get_transition_path(Key target, std::vector<transition_node>& path) 
{
	if (has(target))
	{
		if (target == key())
		{
			transition_node n;

			n.state_ = shared_from_this();

			if (active())
			{
				n.code_ = transition_node::code::noop;
			}
			else
			{
				n.code_ = transition_node::code::target;
			}

			path.push_back(n);

			return true;
		}
		else
		{
			transition_node n;

			n.state_ = shared_from_this();;

			if (active())
			{
				n.code_ = transition_node::code::noop;
			}
			else
			{
				n.code_ = transition_node::code::enter;
			}

			path.push_back(n);

			return get_child_path(target, path);
		}
	}
	else
	{
		if (parent_)
		{
			transition_node n;

			n.state_ = shared_from_this();
			n.code_ = transition_node::code::exit;

			path.push_back(n);

			return parent_->get_transition_path(target, path);
		}
	}

	return false;
}

/// 자식 부터 시작하는 전이 경로 찾기 
template <typename Key>
bool state<Key>::get_child_path(Key target, std::vector<transition_node>& path) 
{
	for(auto& pair : childs_ )
	{
		auto s = pair.second;

		if (s->has(target))
		{
			return s->get_transition_path(target, path);
		}
	}

	return false; // No child is in the path
}

} // util 
} // lax

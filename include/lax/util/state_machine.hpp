#pragma once 

#include <lax/util/state.hpp>
#include <deque>

namespace lax
{
namespace util
{
template <typename Key> 
class state_machine
{
public:  // types
	typedef typename state<Key>::state_ptr state_ptr; 

	enum transition_option
	{
		safe, 
		immediate
	};

	struct transition
	{
		Key target = Key(); 
		transition_option option = transition_option::safe;
	};

public: 
	state_machine()
	{
		root_ = std::make_shared<state<Key>>(0, *this);
		root_->enter();
		current_ = root_;
	}

	void execute()
	{
		process_transitions();

		// active_child를 따라 실행
		root_->execute();
	}

	void tran(Key target, transition_option opt = transition_option::safe)
	{
		if (opt == transition_option::immediate)
		{
			tran_to(target);
		}
		else
		{
			transition ts;

			ts.target = target;
			ts.option = transition_option::safe;

			q_.push_back(ts);
		}
	}

	/// root 상태에 자식 추가
	state_machine& add(state_ptr child)
	{
		return add(0, child);
	}

	state_machine& add(Key parent, state_ptr child)
	{
		auto target = root_->find(parent);

		check(target.get() != nullptr); 
		if (target)
		{
			target->add(child);
		}
		
		return *this;
	}

	state_ptr find(Key key)
	{
		return root_->find(key);
	}

	bool has(Key key)
	{
		return root_->has(key);
	}

public:  // accessors
	state_ptr current() const
	{
		return current_;
	}

	state_ptr root() const
	{
		return root_;
	}

private: 
	void process_transitions(); 
	void tran_to(Key target);

private: 
	state_ptr					current_;
	state_ptr					root_; 
	std::deque<transition>		q_;
};

template <typename Key>
void state_machine<Key>::process_transitions()
{
	while (!q_.empty())
	{
		auto& tran = q_.front();

		tran_to(tran.target);

		q_.pop_front();
	}
}

template <typename Key>
void state_machine<Key>::tran_to(Key target)
{
	check(has(target));
	check(current_);
	check(current_->active());

	if (current_->key() == target)
	{
		// 같은 상태로는 전환되지 않도록 한다. 
		return;
	}

	std::vector<state<Key>::transition_node> path;

	// Target State 까지 패스를 만들어 리스트를 가져온다.
	auto rc = current_->get_transition_path(target, path);

	if (!rc)
	{
		return;
	}

	for(auto& n : path)
	{
		// 패스 목록을 돌면서 상태를 Exit 하고 Enter를 한다. Target이 최종목표이고 noop는 Root다
		switch (n.code_)
		{
		case state<Key>::transition_node::code::enter:
		{
			n.state_->enter();
		}
		break;
		case state<Key>::transition_node::code::target:
		{
			n.state_->enter();

			current_ = n.state_;
		}
		break;
		case state<Key>::transition_node::code::exit:
		{
			n.state_->exit();
		}
		break;
		case state<Key>::transition_node::code::noop:
		{
			current_ = n.state_;
		}
		break;
		default:
			break;
		}
	}
}

} // util
} // lax

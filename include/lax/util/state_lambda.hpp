#pragma once 

#include <lax/util/state.hpp>
#include <functional>

namespace lax
{
namespace util
{

template <class Key>
class state_lambda : public state<Key>
{
public: 
	using action = std::function<void()>;

public:
	state_lambda(
		Key key
		, state_machine& machine)
		: state<Key>(key, machine)
		, enter_(noop)
		, exec_(noop)
		, exit_(noop)
	{
	}

	state_lambda& set_enter(action act)
	{
		check(!enter_);
		enter_ = act;
		return *this;
	}

	state_lambda& set_execute(action act)
	{
		check(!exec_);
		exec_ = act;
		return *this;
	}

	state_lambda& set_exit(action act)
	{
		check(!exit_);
		exit_ = act;
		return *this;
	}

private:
	virtual void on_enter() override
	{
		enter_();
	}

	virtual void on_execute() override
	{
		exec_();
	}

	virtual void on_exit() override
	{
		exit_();
	}

	static void noop() {}

private: 
	action enter_;
	action exec_;
	action exit_;
};

} // util 
} // lax

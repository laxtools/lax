#pragma once 

#include "state.h"

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

	action& set_enter(action act)
	{
		enter_ = act;
		return enter_;
	}

	action& set_execute(action act)
	{
		exec_ = act;
		return exec_;
	}

	action& set_exit(action act)
	{
		exit_ = act;
		return exit_;
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

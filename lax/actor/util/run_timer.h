#pragma once 

namespace lax
{
namespace actor
{

class run_timer
{
public: 
	run_timer() = default;

	/// �ð� ����. 
	void start(float sec)
	{
		start_ = sec;
	}

	/// get elapsed time
	float elapsed(float current) const
	{
		return current - start_;
	}

	/// check elapsed time passed duration
	/** 
	 * usage: 
	 *
	 *  return_if( dt.wait(10033.07, 30) );
	 *  �������� ���� �ð� ���� �ð����� �ʱ�ȭ
	 */
	bool wait(float current, float duration, bool reset = true) const
	{
		auto rc = elapsed(current) < duration;

		if (!rc && reset)
		{
			// reset when duration passed 
			start_ = current;
		}

		return rc;
	}

private: 
	mutable float start_ = 0.f;
};

} // actor 
} // lax

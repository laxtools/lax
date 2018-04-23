#pragma once

namespace lax
{
namespace actor
{

class tick
{
public:
	explicit tick(float elapsed)
		: elapsed_(elapsed)
	{
	}

	float operator-(const tick& rhs) const
	{
		return  (elapsed_ - rhs.elapsed_);
	}

	float get() const
	{
		return elapsed_;
	}

	float operator * () const
	{
		return get();
	}

private:
	float elapsed_ = 0;
};


} // actor
} // lax
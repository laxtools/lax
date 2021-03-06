#pragma once 

#include <lax/util/macros.hpp>
#include <lax/util/null_mutex.hpp>

#include <algorithm>
#include <deque>
#include <exception>
#include <mutex>

namespace lax
{
namespace util
{

/// id pool to reuse ids. 
/**
 * 사용 후 release로 재사용 가능하게 해야 함 
 * 
 * Seq - number type 
 * Mutex - Lockable 
 */
template <typename Seq, typename Mutex = null_mutex>
class sequence
{
public:
	sequence(Seq begin, Seq end, Seq reserve = default_reserve)
		: begin_(begin)
		, end_(end)
		, reserve_(reserve)
		, current_(begin)
	{
		VERIFY(begin_ < end_);
		VERIFY(current_ == begin_);
		VERIFY(reserve_ < (end_ - begin_ + 1));

		// clamp 
		reserve_ = std::min<Seq>(end - begin, reserve_);

		acquire(reserve_);

		VERIFY(!seqs_.empty());
	}

	/// 사용 가능한 다음 차례의 숫자 돌려줌. 
	/** 
	 *  
	 * @return - 다음 차례 숫자. 부족하면 exception 
	 */
	Seq next()
	{
		std::lock_guard<Mutex> lock(mutex_);

		if ( seqs_.size() < reserve_ )
		{
		  	acquire(reserve_ - seqs_.size());
		}

		Seq value = Seq();

		if (!seqs_.empty())
		{
			value = seqs_.front();

			seqs_.pop_front();
		}
		else
		{
			if (current_ == end_)
			{
				throw std::exception("no sequence to use");
			}

			value = current_++;
		}

		return value;
	}

	void release(Seq seq)
	{
		VERIFY(begin_ <= seq && seq < end_);
		RETURN_IF(!(begin_ <= seq && seq < end_));

		std::lock_guard<Mutex> lock(mutex_);
		seqs_.push_back(seq);
	}

	/// 테스트 용으로만 사용
	Seq current() const
	{
		std::lock_guard<Mutex> lock(mutex_);
		return current_;
	}

	std::size_t queue_size() const
	{
		std::lock_guard<Mutex> lock(mutex_);
		return seqs_.size();
	}

private: 
  	void acquire(std::size_t count)
	{
		for (std::size_t s = 0; s < count; ++s)
		{
			if ( current_ < end_ )
			{
				seqs_.push_back(current_++);
			}
		}
	}

private: 
	static constexpr Seq default_reserve = 100;

	mutable Mutex	mutex_;
	Seq				begin_;
	Seq				end_;
	Seq				current_;
	Seq				reserve_;
	std::deque<Seq> seqs_;
};
} // util
} // lax

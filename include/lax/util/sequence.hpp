#pragma once 

#include <lax/util/macros.hpp>
#include <lax/util/null_mutex.hpp>

#include <deque>
#include <exception>
#include <mutex>

namespace lax
{
namespace util
{

/// id pool to reuse ids. 
/**
 * ��� �� release�� ���� �����ϰ� �ؾ� �� 
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
		check(begin_ < end_);
		check(current_ == begin_);

		// clamp 
		reserve_ = std::min<Seq>(end - begin, reserve_);

		for (Seq s = begin_; s < (begin_ + reserve_); ++s)
		{
			seqs_.push_back(current_++);
		}

		check(!seqs_.empty());
	}

	/// ��� ������ ���� ������ ���� ������. 
	/** 
	 *  
	 * @return - ���� ���� ����. �����ϸ� exception 
	 */
	Seq next()
	{
		std::lock_guard<Mutex> lock(mutex_);

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
		check(begin_ <= seq && seq < end_);
		return_if(!(begin_ <= seq && seq < end_));

		std::lock_guard<Mutex> lock(mutex_);
		seqs_.push_back(seq);
	}

	/// �׽�Ʈ �����θ� ���
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
	static const Seq default_reserve = 100;

	mutable Mutex	mutex_;
	Seq				begin_;
	Seq				end_;
	Seq				current_;
	Seq				reserve_;
	std::deque<Seq> seqs_;
};
} // util
} // lax

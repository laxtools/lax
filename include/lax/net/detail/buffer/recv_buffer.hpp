#pragma once 

#include <lax/net/detail/multiple_size_buffer_pool.hpp>
#include <lax/util/macros.hpp>
#include <vector>

namespace lax
{
namespace net
{

/// increasing sized buffer
/** 
 * not thread-safe. 
 * - needs to use a lock if used from multiple threads 
 * - if 1-recv is used, then lock is not required
 */
class recv_buffer 
{
public: 
	using pool = multiple_size_buffer_pool;

public: 
	recv_buffer() 
	{
		expect(pos_ == 0);
		expect(!buf_);
	}

	~recv_buffer()
	{
		if (buf_)
		{
			pool_.release(buf_);
		}
	}

	/// len��ŭ ����. seg �����ϸ� Ǯ���� �Ҵ�
	std::size_t append(const uint8_t* p, std::size_t len)
	{
		expect(p != nullptr);
		expect(len > 0);

		reserve(len);

		check(buf_);
		check(buf_->capacity() >= pos_ + len);

		::memcpy_s(buf_->data() + pos_, buf_->capacity() - pos_, p, len);

		pos_ += len;

		ensure(pos_ <= buf_->capacity());

		return len;
	}
	
	/// ���� �Լ�.
	std::size_t append(const char* p, std::size_t len)
	{
		return append((uint8_t*)p, len);
	}

	/// ���� �Լ�
	std::size_t append(const void* p, std::size_t len)
	{
		return append((uint8_t*)p, len);
	}

	/// ��ü ����Ʈ ũ�� ������
	std::size_t size() const
	{
		return pos_;
	}

	const uint8_t* data() const
	{
		return buf_->data();
	}

	/// ���׸�Ʈ ������ ������.
	void rewind()
	{
		pos_ = 0;
	}

	/// block���� count��ŭ ���� 
	/** 
	 * memmove�� ����ϰ� ���� �� �����Ƿ� 
	 * ȣ���� ���� �ʵ��� ����ؾ� ��
	 */
	void pop_front(std::size_t count)
	{
		count = std::min(count, size());

		::memmove_s(
			buf_->data(), buf_->capacity(), 
			buf_->data() + count, pos_ - count
		);

		pos_ -= count;
	}

	std::size_t capacity() const
	{
		return buf_->capacity();
	}

	/// for test and debugging
	const fixed_size_buffer_pool* get_pool(std::size_t size) const
	{
		return pool_.get_pool(size);
	}

private: 
	void reserve(std::size_t len)
	{
		expect(len > 0); 
		if (!buf_)
		{
			check(pos_ == 0);
			buf_ = pool_.alloc(len);
		}
		else
		{
			if (buf_->capacity() < (pos_ + len))
			{
				auto nbuf = pool_.alloc(pos_ + len);

				::memcpy_s(nbuf->data(), nbuf->capacity(), buf_->data(), pos_);

				pool_.release(buf_);

				buf_ = nbuf;
			}
		}
	}

private: 
	static pool				pool_;
	pool::buffer::ptr		buf_;
	std::size_t				pos_ = 0;
};

} // net 
} // lax


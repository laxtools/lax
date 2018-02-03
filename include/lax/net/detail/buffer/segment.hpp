#pragma once 

#include <lax/util/macros.hpp>
#include <array>

namespace lax
{
namespace net
{

/// buffer segment
template <std::size_t Length>
class segment
{
	using container = std::array<uint8_t, Length>;

	class ptr
	{
	public:
		ptr(const uint8_t* data, std::size_t len)
			: data_(data), len_(len) 
		{
			VERIFY(data_);
		}

		const uint8_t* data() const { return data_; }
		std::size_t size() const { return len_; }

	private:
		const uint8_t* data_ = nullptr;
		std::size_t len_ = 0;
	};

public: 
	segment() {}
	~segment() {}

	/// p���� len ��ŭ ������ �õ��ϰ� �� ���̸�ŭ �����ش�. 
	std::size_t append(const uint8_t* p, std::size_t len)
	{
		auto possible_len = std::min(len, Length - pos_);

		RETURN_IF(possible_len == 0, 0);

		uint8_t* dest = buf_.data() + pos_;

		::memcpy(dest, p, possible_len);

		advance(possible_len);

		return possible_len;
	}
	
	std::size_t append(const char* p, std::size_t len)
	{
		return append((uint8_t*)p, len);
	}

	std::size_t append(const void* p, std::size_t len)
	{
		return append((uint8_t*)p, len);
	}

	std::size_t write(const char* p, std::size_t len)
	{
		return append(p, len);
	}

	/// get pointer and length
	ptr get() const
	{
		return ptr(buf_.data(), pos_);
	}

	ptr get() 
	{
		return ptr(buf_.data(), pos_);
	}

	const uint8_t* data() const
	{
		return buf_.data();
	}
	
	/// ���� ������ ũ��
	std::size_t size() const
	{
		return pos_;
	}

	/// ���׸�Ʈ ������ ������.
	void rewind()
	{
		pos_ = 0;
	}

	std::size_t capacity() const
	{
		return Length;
	}

	void pop_front(std::size_t count)
	{
		// ��������
		count = std::min(count, size());

		::memmove_s(
			(void*)buf_.data(),	buf_.size(), 
			(void*)(buf_.data() + count), pos_ - count
		);

		pos_ -= count;
	}

private: 
	void advance(std::size_t len)
	{
		pos_ += len;

		VERIFY(pos_ <= Length);
	}

private:
	container buf_;
	std::size_t pos_ = 0;
};

} // net 
} // lax

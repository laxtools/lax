#pragma once 

#include <lax/net/detail/segment.hpp>
#include <lax/util/object_pool.hpp>

#include <vector>

namespace lax
{
namespace net
{

/// send buffer 
/** 
 * not thread safe 
 * - needs to use a lock if used from multiple threads
 * - if 1-send per socket is used, then it's not required to use a lock
 */
template <std::size_t Length>
class send_buffer
{
public: 
	using seg = segment<Length>;
	using pool = util::object_pool<seg>;

public: 
	send_buffer() 
	{
		expect(Length > 0);
		expect(pos_ == 0);
		expect(segs_.empty());
	}

	~send_buffer()
	{
		cleanup();
	}

	/// len��ŭ ����. seg �����ϸ� Ǯ���� �Ҵ�
	std::size_t append(const uint8_t* p, std::size_t len)
	{
		expect(p != nullptr);
		expect(len > 0);

		// ���׸�Ʈ �����ϸ� Ȯ��
		reserve(len);	

		std::size_t written = 0;

		while (written < len)
		{
			auto* seg = segs_[get_write_seg()];

			if (seg->size() == Length)
			{
				check(get_write_seg() + 1 < segs_.size());

				// ���� ���׸�Ʈ�� ������ �����ϸ� �������� �̵�
				seg = segs_[get_write_seg() + 1];
			}
			
			auto wl = seg->append((p + written), len - written);
			check(wl > 0);

			advance(wl);
			
			written += wl;
		}

		ensure(written == len);

		return written;
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

	/// ���� ������ �ϴ� ���׸�Ʈ 
	std::size_t get_write_seg() const
	{
		return pos_ / Length;
	}	

	/// �����Ͱ� �ִ� ������ ���׸�Ʈ
	std::size_t get_data_seg() const
	{
		auto pos = pos_ > 0 ? pos_ - 1 : 0;

		return pos / Length;
	}	

	/// ���׸�Ʈ ������ ������.
	void rewind()
	{
		for (auto& s : segs_) { s->rewind(); }

		pos_ = 0;
	}

	/// �����͸� ���� �ִ� �͵鸸 ����
	std::vector<seg*> transfer()
	{
		// no available data
		if (pos_ == 0)
		{
			return std::vector<seg*>();
		}

		auto last = get_data_seg();

		auto end = segs_.begin();

		for (std::size_t i = 0; i < last + 1; ++i)
		{
			if (end != segs_.end())
			{
				++end;
			}
		}

		std::vector<seg*> lst(segs_.begin(), end);
		segs_.erase(segs_.begin(), end);

		rewind();

		return lst; // rvo 
	}

	/// ���� �͵� �� ����ϸ� ����
	void release(std::vector<seg*>& segs)
	{
		for (auto& s : segs)
		{
			pool_.destroy(s);
		}
	}

private: 
	void advance(std::size_t len)
	{
		pos_ += len;
	}

	void reserve(std::size_t len)
	{
		expect(len > 0); 

		auto total_segs = (pos_ + len) / Length + 1;
		auto required_segs = total_segs - segs_.size();

		for (std::size_t i = 0; i < required_segs; ++i)
		{
			segs_.push_back(pool_.construct_raw());
		}
	}

	void cleanup()
	{
		for (auto& s : segs_)
		{
			pool_.destroy(s);
		}
	}

private: 
	static pool			pool_;
	std::vector<seg*>	segs_;
	std::size_t			pos_ = 0;
};

 // per class pool instance
template <std::size_t Length>
typename send_buffer<Length>::pool send_buffer<Length>::pool_;

} // net 
} // lax


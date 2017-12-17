#pragma once 

#include <lax/net/detail/segment.h>
#include <lax/util/object_pool.h>

#include <vector>

namespace lax
{
namespace net
{

/// buffer segment
template <std::size_t Length>
class segment_buffer
{
public: 
	using seg = segment<Length>;
	using pool = util::object_pool<seg>;

public: 
	segment_buffer() {}
	~segment_buffer()
	{
		cleanup();
	}

	/// len만큼 쓴다. seg 부족하면 풀에서 할당
	std::size_t append(const uint8_t* p, std::size_t len)
	{
		// 세그먼트 부족하면 확보
		reserve(len);	

		std::size_t written = 0;

		while (written < len)
		{
			auto* seg = segs_[get_write_seg()];

			if (seg->size() == Length)
			{
				check(get_write_seg() + 1 < segs_.size());

				// 현재 세그먼트에 공간이 부족하면 다음으로 이동
				seg = segs_[get_write_seg() + 1];
			}
			
			auto wl = seg->append((p + written), len - written);
			check(wl > 0);

			advance(wl);
			
			written += wl;
		}

		check(written == len);

		return written;
	}
	
	/// 편의 함수.
	std::size_t append(const char* p, std::size_t len)
	{
		return append((uint8_t*)p, len);
	}

	/// 편의 함수
	std::size_t append(const void* p, std::size_t len)
	{
		return append((uint8_t*)p, len);
	}

	/// 전체 바이트 크기 돌려줌
	std::size_t size() const
	{
		return pos_;
	}

	/// 현재 쓰려고 하는 세그먼트 
	std::size_t get_write_seg() const
	{
		return pos_ / Length;
	}	

	/// 데이터가 있는 마지막 세그먼트
	std::size_t get_data_seg() const
	{
		auto pos = pos_ > 0 ? pos_ - 1 : 0;

		return pos / Length;
	}	

	/// 세그먼트 앞으로 돌린다.
	void rewind()
	{
		for (auto& s : segs_) { s->rewind(); }

		pos_ = 0;
	}

	/// 데이터를 갖고 있는 것들만 넣음
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

		return lst;
	}

	/// 얻은 것들 다 사용하면 해제
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
	std::size_t			pos_;
};

 // per class pool instance
template <std::size_t Length>
typename segment_buffer<Length>::pool segment_buffer<Length>::pool_;

} // net 
} // lax


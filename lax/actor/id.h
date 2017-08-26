#pragma once 

namespace lax
{
namespace actor
{

///  globally unique actor id
/**
 * node container / service actors
 *   -  cluster 64 bit : flag (8) node(16) unique (32)
 *      
 * application actors (entity)
 *  - application specific
 *
 * msg_actor : msg_pack
 *   - len / key / src / dst / payload
 *   
 * can be used as key for map and unordered_map
 */
class id
{
public:
	static const uint64_t invalid = 0;

	id() = default;

	id(uint64_t val)
		: value(val)
	{
	}

	id(uint8_t aflag, uint16_t anode, uint32_t alocal)
		: flag(aflag)
		, node(anode)
		, local(alocal)
	{
	}
	
	id(uint16_t anode, uint32_t alocal)
		: flag(0)
		, node(anode)
		, local(alocal)
	{
	}

	bool operator == (const id& rhs) const
	{
		return value == rhs.value;
	}

	bool operator != (const id& rhs) const
	{
		return value != rhs.value;
	}

	bool operator < (const id& rhs) const
	{
		return value < rhs.value;
	}

	// copy / assignment: default

	/// simple validity check
	bool is_valid() const
	{
		return value != invalid;
	}

	/// node id
	bool is_node() const
	{
		return node > 0 && local == 0;
	}

	/// data
	union
	{
		uint64_t value = invalid;

		struct
		{
			uint8_t  flag;		// 비트 플래그 
			uint16_t node;		// node id. 정적으로 구성
			uint32_t local;		// local unique part 
		};
	};

	MSGPACK_DEFINE(value);
};

} // actor
} // lax

#include <unordered_map>

// hash function for id
namespace std
{
	template <> 
	struct hash<lax::actor::id>
	{
		uint64_t operator() (const lax::actor::id& id) const
		{
			return std::hash<uint64_t>()(id.value);
		}
	};
}
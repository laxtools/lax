#pragma once 

#include <lax/util/concurrent_queue.hpp>
#include <lax/util/macros.hpp>

namespace lax
{
namespace util
{

/// simple object poo. thread-safe with a cocurrent queue
/** 
 * todo: set free count limit to return memory to os
 */
template <typename Obj, typename Allocator = std::allocator<Obj>>
class object_pool 
{
public: 
	using ptr = Obj*;

	/// simple convenience uqniue ptr
	class ref 
	{
	public:
		ref(ptr p, object_pool* pool);
		~ref();
		
		ptr operator->();
		ptr operator *();
		ptr get() { return operator*(); }

		// note: assignment invalidates rhs object
		ref(ref& rhs) { move(rhs); }
		ref& operator=(ref& rhs) { return move(rhs); }

	private: 
		ref& move(ref& rhs);

	private:
		object_pool* pool_ = nullptr;
		ptr p_ = nullptr;
	};

	using shared_ref = std::shared_ptr<ref>;

	ptr get(shared_ref& sr) { return sr->get(); }

public:
	object_pool() {}

	~object_pool()
	{
		cleanup();
	}

	template<class... Types>
	ptr construct_raw(Types&&... args)
	{
		ptr obj = alloc();

		::new ((void *)obj) Obj(std::forward<Types>(args)...);

		return obj;
	}

	template<class... Types>
	ref construct(Types&&... args)
	{	
		ptr obj = alloc();

		::new ((void *)obj) Obj(std::forward<Types>(args)...);

		return ref(obj, this);
	}

	template<class... Types>
	shared_ref construct_shared(Types&&... args)
	{	
		ptr obj = alloc();

		::new ((void *)obj) Obj(std::forward<Types>(args)...);

		return std::make_shared<ref>(obj, this);
	}

	void destroy(ptr obj)
	{
		(obj)->~Obj(); 

		dealloc(obj);
	}

	std::size_t unsafe_size() const
	{
		return q_.unsafe_size();
	}

private: 
	ptr alloc()
	{
		ptr obj; 

		if (q_.pop(obj))
		{
			return obj;
		}

		return allocator_.allocate(1);
	}

	void dealloc(ptr obj)
	{
		q_.push(obj);
	}

	void cleanup()
	{
		ptr obj; 

		while (q_.pop(obj))
		{
			allocator_.deallocate(obj, 1);
		}
	}

private: 
	using queue = concurrent_queue<ptr>;

private:
	queue q_;
	Allocator allocator_;
};

template <typename Obj, typename Allocator>
object_pool<Obj, Allocator>::ref::ref(ptr p, object_pool* pool)
	: p_(p)
	, pool_(pool)
{
	check(pool_);
	check(p_);
}

template <typename Obj, typename Allocator>
object_pool<Obj, Allocator>::ref::~ref()
{
	if (pool_ && p_)
	{
		pool_->destroy(p_);
	}
}

template <typename Obj, typename Allocator>
typename object_pool<Obj, Allocator>::ptr 
object_pool<Obj, Allocator>::ref::operator->()
{
	return p_;
}

template <typename Obj, typename Allocator>
typename object_pool<Obj, Allocator>::ptr
object_pool<Obj, Allocator>::ref::operator*()
{
	return p_;
}

template <typename Obj, typename Allocator>
typename object_pool<Obj, Allocator>::ref&
object_pool<Obj, Allocator>::ref::move(ref& rhs)
{
	if (this == &rhs) return *this;

	this->pool_ = rhs.pool_;
	this->p_ = rhs.p_;

	rhs.pool_ = nullptr;
	rhs.p_ = nullptr;

	return *this;
}
} // util 
} // lax

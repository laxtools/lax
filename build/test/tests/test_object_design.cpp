#include "stdafx.h"

#include <catch.hpp>
#include <atomic>

namespace
{

/// from nanogui implementation
class object {
public:
	/// Default constructor
	object() { }

	/// Copy constructor
	object(const object &) : m_refCount(0) {}

	/// Return the current reference count
	int getRefCount() const { return m_refCount; };

	/// Increase the object's reference count by one
	void incRef() const { ++m_refCount; }

	/** \brief Decrease the reference count of
	* the object and possibly deallocate it.
	*
	* The object will automatically be deallocated once
	* the reference count reaches zero.
	*/
	void decRef(bool dealloc = true) const noexcept;

protected:
	/** \brief Virtual protected deconstructor.
	* (Will only be called by \ref ref)
	*/
	virtual ~object();

private:
	mutable std::atomic<int> m_refCount{ 0 };
};

void object::decRef(bool dealloc) const noexcept {
	--m_refCount;
	if (m_refCount == 0 && dealloc) {
		delete this;
	}
	else if (m_refCount < 0) {
		fprintf(stderr, "Internal error: Object reference count < 0!\n");
		abort();
	}
}

object::~object() { }

/**
* \class ref object.h nanogui/object.h
*
* \brief Reference counting helper.
*
* The \a ref template is a simple wrapper to store a pointer to an object. It
* takes care of increasing and decreasing the object's reference count as
* needed. When the last reference goes out of scope, the associated object
* will be deallocated.
*
* The advantage over C++ solutions such as ``std::shared_ptr`` is that
* the reference count is very compactly integrated into the base object
* itself.
*/
template <typename T> class ref {
public:
	/// Create a ``nullptr``-valued reference
	ref() { }

	/// Construct a reference from a pointer
	explicit ref(T *ptr) : m_ptr(ptr) 
	{
		if (m_ptr)
		{
			((object *)m_ptr)->incRef();
		}
	}

	/// Copy constructor
	ref(const ref &r) : m_ptr(r.m_ptr) 
	{
		if (m_ptr)
		{
			((object *)m_ptr)->incRef();
		}
	}

	/// Move constructor
	ref(ref &&r) noexcept : m_ptr(r.m_ptr) 
	{
		r.m_ptr = nullptr;
	}

	/// Destroy this reference
	~ref() 
	{
		if (m_ptr)
		{
			((object *)m_ptr)->decRef();
		}
	}

	/// Move another reference into the current one
	ref& operator=(ref&& r) noexcept 
	{
		if (&r != this) 
		{
			if (m_ptr)
			{
				((object *)m_ptr)->decRef();
			}

			m_ptr = r.m_ptr;
			r.m_ptr = nullptr;
		}
		return *this;
	}

	/// Overwrite this reference with another reference
	ref& operator=(const ref& r) noexcept 
	{
		if (m_ptr != r.m_ptr) 
		{
			if (r.m_ptr)
			{
				((object *)r.m_ptr)->incRef();
			}

			if (m_ptr)
				((object *)m_ptr)->decRef();
			m_ptr = r.m_ptr;
		}
		return *this;
	}

	/// Overwrite this reference with a pointer to another object
	ref& operator=(T *ptr) noexcept 
	{
		if (m_ptr != ptr) 
		{
			if (ptr)
			{
				((object *)ptr)->incRef();
			}
			if (m_ptr)
			{
				((object *)m_ptr)->decRef();
			}
			m_ptr = ptr;
		}
		return *this;
	}

	/// Compare this reference with another reference
	bool operator==(const ref &r) const { return m_ptr == r.m_ptr; }

	/// Compare this reference with another reference
	bool operator!=(const ref &r) const { return m_ptr != r.m_ptr; }

	/// Compare this reference with a pointer
	bool operator==(const T* ptr) const { return m_ptr == ptr; }

	/// Compare this reference with a pointer
	bool operator!=(const T* ptr) const { return m_ptr != ptr; }

	/// Access the object referenced by this reference
	T* operator->() { return m_ptr; }

	/// Access the object referenced by this reference
	const T* operator->() const { return m_ptr; }

	/// Return a C++ reference to the referenced object
	T& operator*() { return *m_ptr; }

	/// Return a const C++ reference to the referenced object
	const T& operator*() const { return *m_ptr; }

	/// Return a pointer to the referenced object
	operator T* () { return m_ptr; }

	/// Return a const pointer to the referenced object
	T* get() { return m_ptr; }

	/// Return a pointer to the referenced object
	const T* get() const { return m_ptr; }

	/// Check if the object is defined
	operator bool() const { return m_ptr != nullptr; }
private:
	T *m_ptr = nullptr;
};

struct test : public object
{
	int v = 3;
};

} // noname

TEST_CASE("object ref design")
{
	// std::atomic based intrusive shared pointer
	
	SECTION("usage")
	{
		ref<test> r1(new test());

		// ref<object> r2 = r1;
		// ref<test> r3 = r2.cast<test>();

		// ref<object> 로 변환 될 수 있는가? 
		// 위와 같이 자유롭게 되어야 하는데 
		// shared_ptr은 가능하고 위의 구현은 불가능하다. 
		// 아마 구현은 할 수 있겠으나 큰 이득이 없을 듯.

		// 내부적으로 shared_ptr 구현이 더 견고하다. 
		// thread safety는 사실 고려해야 할 점이 무척 많다. 
	}
}

TEST_CASE("object ref w/ shared_ptr")
{
	// object::ptr로 접근 

	// cyclic reference 이슈를 해결할 수 있는 방법은? 
	// 단위별 컨벤션을 잘 잡아야 한다. 소유권과 참조를 구분한다. 
	// weak_ptr로 해결하기 어렵다. 
}
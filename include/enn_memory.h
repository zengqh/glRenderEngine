/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/25
* File: enn_memory.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include <enn_config.h>
#include <enn_std_header.h>

#if ENN_MEMORY_TRACKER
#define ENN_MALLOC(bytes)         ::enn::Memory::allocBytes(bytes, __FILE__, __LINE__, __FUNCTION__)
#define ENN_MALLOC_T(T)           static_cast<T*>(::enn::Memory::allocBytes(sizeof(T), __FILE__, __LINE__, __FUNCTION__))
#define ENN_MALLOC_ARRAY_T(T, n)  static_cast<T*>(::enn::Memory::allocBytes(sizeof(T)*(n), __FILE__, __LINE__, __FUNCTION__))
#define ENN_FREE(ptr)             ::enn::Memory::freeBytes(ptr)

#define ENN_NEW_T(T)          new (::enn::Memory::allocBytes(sizeof(T), __FILE__, __LINE__, __FUNCTION__)) T
#define ENN_DELETE_T(ptr)     { ::enn::destruct1(ptr); ::enn::Memory::freeBytes(ptr); }

#define ENN_NEW_ARRAY_T(T, n)         ::enn::constructN(static_cast<T*>(::enn::Memory::allocBytes(sizeof(T)*(n), __FILE__, __LINE__, __FUNCTION__)), n)
#define ENN_DELETE_ARRAY_T(ptr, n)    { ::enn::destructN(ptr, n); ::enn::Memory::freeBytes(ptr); }

#define ENN_NEW       new (__FILE__, __LINE__, __FUNCTION__)
#define ENN_DELETE    delete
#else
#define ENN_MALLOC(bytes)         ::enn::Memory::allocBytes(bytes)
#define ENN_MALLOC_T(T)           static_cast<T*>(::enn::Memory::allocBytes(sizeof(T)))
#define ENN_MALLOC_ARRAY_T(T, n)  static_cast<T*>(::enn::Memory::allocBytes(sizeof(T)*(n)))
#define ENN_FREE(ptr)             ::enn::Memory::freeBytes(ptr)

#define ENN_NEW_T(T)          new (::enn::Memory::allocBytes(sizeof(T))) T
#define ENN_DELETE_T(ptr)     { ::enn::destruct1(ptr); ::enn::Memory::freeBytes(ptr); }

#define ENN_NEW_ARRAY_T(T, n)         ::enn::constructN(static_cast<T*>(::enn::Memory::allocBytes(sizeof(T)*(n))), n)
#define ENN_DELETE_ARRAY_T(ptr, n)    { ::enn::destructN(ptr, n); ::enn::Memory::freeBytes(ptr); }

#define ENN_NEW       new
#define ENN_DELETE    delete
#endif

namespace enn
{

// 智能指针释放行为
enum SmartPtrFreeMethod
{
	SPFM_FREE,      // 平数据
	SPFM_DELETE_T,  // 普通类
	SPFM_DELETE,    // 继承自AllocatedObject的类
};

//////////////////////////////////////////////////////////////////////////
class Memory
{
public:
	static void*  allocBytes( size_t size );
	static void*  allocBytes( size_t size, const char* file, int line, const char* func );
	static void   freeBytes( void* p );
	static size_t getMaxAllocBytes();
};

//////////////////////////////////////////////////////////////////////////
class AllocatedObject
{
public:
	void* operator new( size_t sz )
	{
		return Memory::allocBytes( sz );
	}

	void* operator new( size_t sz, const char* file, int line, const char* func )
	{
		return Memory::allocBytes( sz, file, line, func );
	}

	void* operator new( size_t, void* ptr )
	{
		return ptr;
	}

	void* operator new[]( size_t sz )
	{
		return Memory::allocBytes( sz );
	}

	void* operator new[]( size_t sz, const char* file, int line, const char* func )
	{
		return Memory::allocBytes( sz, file, line, func );
	}

	void operator delete( void* ptr )
	{
		Memory::freeBytes( ptr );
	}

	void operator delete( void* ptr, void* )
	{
		Memory::freeBytes( ptr );
	}

	void operator delete( void* ptr, const char*, int, const char* )
	{
		Memory::freeBytes( ptr );
	}

	void operator delete[]( void* ptr )
	{
		Memory::freeBytes( ptr );
	}

	void operator delete[]( void* ptr, const char*, int, const char* )
	{
		Memory::freeBytes( ptr );
	}
};

//////////////////////////////////////////////////////////////////////////
template<class T>
struct STLAllocatorBase
{
	typedef T value_type;
};

template<class T>
struct STLAllocatorBase<const T>
{
	typedef T value_type;
};

template<class T>
class STLAllocator : public STLAllocatorBase<T>
{
public:
	typedef STLAllocatorBase<T>			Base;
	typedef typename Base::value_type	value_type;
	typedef value_type*					pointer;
	typedef const value_type*			const_pointer;
	typedef value_type&					reference;
	typedef const value_type&			const_reference;
	typedef std::size_t					size_type;
	typedef std::ptrdiff_t				difference_type;

	template<typename U>
	struct rebind
	{
		typedef STLAllocator<U> other;
	};

public:
	STLAllocator() {}

	STLAllocator( const STLAllocator& ) {}

	template <class U>
	STLAllocator( const STLAllocator<U>& ) {}

	virtual ~STLAllocator(){}

public:
	pointer allocate( size_type count, typename std::allocator<void>::const_pointer ptr = 0 )
	{
		size_type sz = count * sizeof(T);
		return static_cast<pointer>(Memory::allocBytes(sz)); // stl不跟踪内存泄露
	}

	void deallocate( pointer ptr, size_type )
	{
		Memory::freeBytes(ptr);
	}

	pointer address( reference x ) const
	{
		return &x;
	}

	const_pointer address( const_reference x ) const
	{
		return &x;
	}

	size_type max_size() const throw()
	{
		return Memory::getMaxAllocBytes();
	}

	void construct( pointer p )
	{
		new (p)T();
	}

	void construct( pointer p, const T& val )
	{
		new (p)T(val);
	}

	void destroy( pointer p )
	{
		p->~T();
	}
};

template<class T, class T2>
bool operator==( const STLAllocator<T>&, const STLAllocator<T2>& )
{
	return true;
}

template<class T, class OtherAllocator>
bool operator==( const STLAllocator<T>&, const OtherAllocator& )
{
	return false;
}

template<class T, class T2>
bool operator!=( const STLAllocator<T>&, const STLAllocator<T2>& )
{
	return false;
}

template<class T, class OtherAllocator>
bool operator!=( const STLAllocator<T>&, const OtherAllocator& )
{
	return true;
}

//////////////////////////////////////////////////////////////////////////
template<class T>
T* constructN( T* p, size_t n )
{
	for ( size_t i = 0; i < n; ++i )
	{
		new (p+i) T();
	}
	return p;
}

template<class T>
void destruct1( T* p )
{
	if ( p )
	{
		p->~T();
	}
}

template<class T>
void destructN( T* p, size_t n )
{
	if ( p )
	{
		for ( size_t i = 0; i < n; ++i )
		{
			p[i].~T();
		}
	}
}

}

#define ENN_PP_STRINGIZING(x)         #x
#define ENN_PP_STRINGIZING_MACRO(x)   ENN_PP_STRINGIZING(x)

#define ENN_PP_CAT_(x, y)             x##y
#define ENN_PP_CAT(x, y)              ENN_PP_CAT_(x, y)

#define ENN_MEMBER_OFFSET(t, x)		((int) &((t*)0)->x)
#define ENN_ARRAY_SIZE(v)			(sizeof(v) / sizeof((v)[0]))

#define ENN_CLEAR_ZERO(x)			memset( x, 0, sizeof(x) )
#define ENN_CLEAR_BLOCK_ZERO(x)		memset( &(x), 0, sizeof(x))

#define ENN_SAFE_FREE(x)				{ if (x) { ENN_FREE(x);   (x) = 0; } }
#define ENN_SAFE_DELETE(x)				{ if (x) { ENN_DELETE(x);   (x) = 0; } }
#define ENN_SAFE_DELETE_T(x)			{ if (x) { ENN_DELETE_T(x); (x) = 0; } }
#define ENN_SAFE_DELETE_ARRAY_T(x, n)	{ if (x) { ENN_DELETE_ARRAY_T(x, n); (x) = 0; } }
#define ENN_SAFE_RELEASE(x)				{ if (x) { (x)->release();    (x) = 0; } }

#define ENN_FOR_EACH(container, var, it) \
	for ( container::iterator it = (var).begin(), it##_e = (var).end(); it != it##_e; ++it )

#define ENN_FOR_EACH_CONST(container, var, it) \
	for ( container::const_iterator it = (var).begin(), it##_e = (var).end(); it != it##_e; ++it )

#define ENN_ALIGN_4_BYTES(x)		((x + 3) / 4 * 4)

#define ENN_BITS(x)					(1 << x)
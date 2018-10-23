/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/07/22
* File: enn_shared_ptr.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_memory.h"
#include "enn_std_types.h"
#include "enn_object_recycle.h"

namespace enn
{
template<class T, int FREE_METHOD = SPFM_DELETE>
    class SharedPtr
	{
    public:
        typedef T* Pointer;

	private:
		T* pRep;
		unsigned int* pUseCount;

	public:
		SharedPtr() : pRep(0), pUseCount(0)
        {
        }

        template<class Y>
		explicit SharedPtr(Y* rep) 
			: pRep(rep)
			, pUseCount(rep ? ENN_NEW_T(unsigned int)(1) : 0)
		{
		}

		SharedPtr( const SharedPtr& r )
            : pRep(0), pUseCount(0)
		{
			pRep = r.pRep;
			pUseCount = r.pUseCount;
			// Handle zero pointer gracefully to manage STL containers
			if(pUseCount)
			{
				++(*pUseCount); 
			}
		}

		SharedPtr& operator=(const SharedPtr& r) 
        {
			if (pRep == r.pRep)
				return *this;
			// Swap current data into a local copy
			// this ensures we deal with rhs and this being dependent
			SharedPtr<T> tmp(r);
			swap(tmp);
			return *this;
		}
		
		template< class Y>
		SharedPtr(const SharedPtr<Y,FREE_METHOD>& r)
            : pRep(0), pUseCount(0)
		{
			pRep = r.get();
			pUseCount = r.useCountPointer();
			// Handle zero pointer gracefully to manage STL containers
			if(pUseCount)
			{
				++(*pUseCount);
			}
		}

		template< class Y>
		SharedPtr& operator=(const SharedPtr<Y>& r) 
        {
			if (pRep == r.get())
				return *this;
			// Swap current data into a local copy
			// this ensures we deal with rhs and this being dependent
			SharedPtr<T> tmp(r);
			swap(tmp);
			return *this;
		}

		~SharedPtr() 
        {
            _release();
		}

        operator bool() const { return pRep != 0; }
        bool operator!() const { return pRep == 0; }

		T& operator*() const { ENN_ASSERT(pRep); return *pRep; }
		T* operator->() const { ENN_ASSERT(pRep); return pRep; }
		T* get() const { return pRep; }

		/** Binds rep to the SharedPtr.
			@remarks
				Assumes that the SharedPtr is uninitialised!
		*/
		void attach(T* rep)
        {
			ENN_ASSERT(!pRep && !pUseCount);
            _release(); // 只是为了安全，非设计如此
			pUseCount = ENN_NEW_T(unsigned int)(1);
			pRep = rep;
		}

        void _bind( T* rep, unsigned int* useCount )
        {
            ENN_ASSERT(!pRep && !pUseCount);
            _release(); // 只是为了安全，非设计如此
            pRep = rep;
            if ( useCount )
                ++(*useCount);
            pUseCount = useCount;
        }

        T* detach()
        {
            ENN_ASSERT( pUseCount && *pUseCount == 1 );
            T* tmp = pRep;

            pRep = 0;
            ENN_FREE(pUseCount);
            pUseCount = 0;

            return tmp;
        }

		bool unique() const { ENN_ASSERT(pUseCount); return *pUseCount == 1; }
		unsigned int useCount() const { ENN_ASSERT(pUseCount); return *pUseCount; }
		unsigned int* useCountPointer() const { return pUseCount; }

        void swap(SharedPtr<T> &other) 
        {
            std::swap(pRep, other.pRep);
            std::swap(pUseCount, other.pUseCount);
        }

        void release()
        { 
			if (pRep)
			{
				_release();
				pRep = 0;
				pUseCount = 0;
			}
        }

        void reset( T* ptr )
        {
            release();
            attach( ptr );
        }

    private:
        void _release()
        {
			bool destroyThis = false;

			if (pUseCount)
			{
				if (--(*pUseCount) == 0) 
				{
					destroyThis = true;
	            }
			}
   
			if (destroyThis)
				_destroy();
        }

        void _destroy()
        {
            // Use setNull() before shutdown or make sure your pointer goes
            // out of scope before ENN shuts down to avoid this.
			_destroy( IntToType<FREE_METHOD>() );

			// use ENN_FREE instead of ENN_FREE_T since 'unsigned int' isn't a destructor
			// we only used ENN_NEW_T to be able to use constructor
            ENN_FREE(pUseCount);
        }

        void _destroy( IntToType<SPFM_FREE> )
        {
            ENN_FREE(pRep);
        }

        void _destroy( IntToType<SPFM_DELETE_T> )
        {
            ENN_DELETE_T(pRep);
        }

        void _destroy( IntToType<SPFM_DELETE> )
        {
            ENN_DELETE pRep;
        }
	};

	template<class T, class U, int FREE_METHOD> 
    bool operator==(SharedPtr<T,FREE_METHOD> const& a, SharedPtr<U,FREE_METHOD> const& b)
	{
		return a.get() == b.get();
	}

	template<class T, class U, int FREE_METHOD> 
    bool operator!=(SharedPtr<T,FREE_METHOD> const& a, SharedPtr<U,FREE_METHOD> const& b)
	{
		return a.get() != b.get();
	}

	template<class T, class U, int FREE_METHOD>
    bool operator<(SharedPtr<T,FREE_METHOD> const& a, SharedPtr<U,FREE_METHOD> const& b)
	{
		return a.get() < b.get();
	}
    
    template<class T, class U, int FREE_METHOD>
    SharedPtr<T,FREE_METHOD> staticCast( SharedPtr<U,FREE_METHOD>& ptr )
    {
        SharedPtr<T,FREE_METHOD> ptrOth;
        ptrOth._bind( static_cast<SharedPtr<T,FREE_METHOD>::Pointer>(ptr.get()), ptr.useCountPointer() );
        return ptrOth;
    }
}
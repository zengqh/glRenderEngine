/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/28
* File: enn_resource.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_resource.h"
#include "enn_resource_manager.h"

namespace enn
{
Resource::~Resource()
{
	ENN_SAFE_RELEASE(res_creator_);
}

void Resource::setResCreator(IResourceCreator* creator)
{
	ENN_SAFE_RELEASE(res_creator_);
	res_creator_ = creator; 
}

void Resource::setNullCreator()
{
	ResourceNullCreator* s_creator = ENN_NEW ResourceNullCreator;
	setResCreator(s_creator);
}

void Resource::addListener(IResourceListener* listener)
{
	if (!listener)
	{
		ENN_ASSERT(0);
		return;
	}

	if (listeners_.insert(listener).second)
	{
		if (status_ == S_LOADED)
		{
			listener->onResourceLoaded(this);
		}
	}
}

void Resource::removeListener( IResourceListener* listener )
{
	if (!listener)
	{
		ENN_ASSERT(0);
		return;
	}

	if (listeners_.empty())
	{
		return;
	}

	if (listeners_.find(listener) != listeners_.end())
	{
		listeners_.erase( listener );
	}
}

void Resource::setLoadAsync( bool b )
{
	load_async_ = b;
}

void Resource::load(bool async)
{
	setLoadAsync(async);
	manager_->load(this, async);
}

void Resource::unload( bool async )
{
	manager_->unload(this, async);
}

bool Resource::checkLoad()
{
	/** ��������Ѿ��ɹ�ֱ�ӷ��� */
	if ( status_ == S_LOADED ) return true;

	manager_->checkLoad(this);
	return false;
}

void Resource::update()
{
	/** ֪ͨ��Դ�и��� */
	ENN_ASSERT(status_ == S_LOADED);
	for ( ListenerList::iterator it = listeners_.begin(), ite = listeners_.end(); it != ite; ++it )	
	{
		(*it)->onResourceUpdate(this);
	}
}

bool Resource::setLoadingStatus()
{
	/**
	 ���ü�����״̬
	 unloaded => loading
	*/
	if ( status_ == S_UNLOADED )
	{
		status_ = S_LOADING;
		return true;
	}

	return false;
}

void Resource::setPreparedStatus( bool ok )
{
	/**
	 ���õ�ǰ״̬Ϊ׼��
	 loading => prepared ok
	 loading => prepared failed
	*/
	if ( status_ == S_LOADING )
	{
		status_ = ok ? S_PREPARED_OK : S_PREPARED_FAILED;
	}
}

void Resource::setCompletedStatus( bool ok )
{ 
	/**
	 ����������״̬
	 prepared ok => loaded
	 prepared ok => badres
	 prepared failed => badres
	*/

	if ( status_ != S_PREPARED_OK && status_ != S_PREPARED_FAILED )
	{
		return;
	}

	/** loadedΨһ������prepare ok�����Ҳok */
	State finalState = S_BADRES;
	if ( (status_ == S_PREPARED_OK) && ok )
	{
		finalState = S_LOADED;
	}

	/** ��������״̬ */
	status_ = finalState;

	if ( finalState == S_LOADED )
	{
		for ( ListenerList::iterator it = listeners_.begin(), ite = listeners_.end(); it != ite; ++it )
			(*it)->onResourceLoaded( this );
	}
	else
	{
		for ( ListenerList::iterator it = listeners_.begin(), ite = listeners_.end(); it != ite; ++it )
			(*it)->onResourceBad( this );
	}
}


bool Resource::setUnloadingStatus()
{	/**
	 ����ж����״̬
	 loaded => unloading
	 badres => unloading
	*/
	if ( status_ == S_LOADED || status_ == S_BADRES )
	{
		status_ = S_UNLOADING;
		return true;
	}

	return false;
}

void Resource::setUnloadedStatus()
{
	if ( status_ == S_UNLOADING )
	{
		status_ = S_UNLOADED;
	}
}

void Resource::release()
{
	--ref_count_;

	if (ref_count_ == 1 )
	{
		manager_->releaseRes(this);
	}
}

void Resource::setUserData(void* user_data)
{
	user_data_ = user_data;
}

void* Resource::getUserData() const
{
	return user_data_;
}

}


/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/29
* File: enn_resource_manager.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_resource.h"
#include "enn_auto_name_manager.h"

namespace enn
{
class ResourceManager;
class MsgQueueManager;
//////////////////////////////////////////////////////////////////////////
class ResAutoCreatorBase : public AllocatedObject, public IResourceCreator
{
public:
	virtual ~ResAutoCreatorBase() {}

	virtual void release();
	virtual bool prepareResource( Resource* res, void* context );

public:
	void setBasePath( const String& basePath ) { m_basePath = basePath; }
	const String& getBasePath() const { return m_basePath; }
	virtual String getLocateFileName( Resource* res ) const;

protected:
	virtual bool prepareResourceImpl( Resource* res, MemoryBuffer* buff ) = 0;

protected:
	String m_basePath;
};

//////////////////////////////////////////////////////////////////////////
struct IResourceScheduler
{
	virtual void release() = 0;
	virtual void load( Resource* res, bool async ) = 0;
	virtual void unload( Resource* res, bool async ) = 0;
	virtual void checkLoad( Resource* res ) = 0;
	virtual void releaseRes( Resource* res ) = 0;
};

class DefaultResourceScheduler : 
	public AllocatedObject, 
	public IResourceScheduler, 
	public IMsgQueueListener,
	public IFileSystemListener,
	public AutoNameManager
{
public:
	DefaultResourceScheduler();
	virtual ~DefaultResourceScheduler();

public:
	void setQueueMgr(MsgQueueManager* mgr) 
	{
		queue_mgr_ = mgr;
	}

	void registerMsg();

public:
	virtual void release();
	virtual void load( Resource* res, bool async );
	virtual void unload( Resource* res, bool async );
	virtual void checkLoad( Resource* res );
	virtual void releaseRes( Resource* res );

	virtual void onWorkImpl( Message* msg );

	virtual void onGetFileResult( const String& fileName, void* userData, MemoryBuffer* buff, FileSystemGetResult result );

protected:
	MsgQueueManager*		queue_mgr_;
};

//////////////////////////////////////////////////////////////////////////
// 资源管理器
class ResourceManager : public AllocatedObject, public AutoNameManager
{

public:
	typedef enn::map<String, Resource*>::type ResourceMap;

public:
	ResourceManager();
	virtual ~ResourceManager();

public:
	virtual void init();

	void setAutoCreator( ResAutoCreatorBase* creator );
	void setScheduler( IResourceScheduler* scheduler );

	ResAutoCreatorBase* getAutoCreator()  const 
	{ 
		return m_autoCreator; 
	}

	IResourceScheduler* getScheduler() const 
	{ 
		return m_scheduler; 
	}

	Resource* createResource( const String& name );
	Resource* getOrCreateResource( const String& name );
	Resource* getResource( const String& name ) const;

public:
	void load( Resource* res, bool async );
	void unload( Resource* res, bool async );
	void checkLoad( Resource* res );
	void releaseRes( Resource* res );

protected:
	void destroyAllResources();
	virtual Resource* createImpl() = 0;

public:
	virtual void deleteImpl( Resource* res );

protected:
	ResAutoCreatorBase*  m_autoCreator;
	IResourceScheduler*  m_scheduler;
	ResourceMap          m_resMap;
};

}
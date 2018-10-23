/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/28
* File: enn_resource.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_platform_headers.h"
#include "enn_msg_id.h"
#include "enn_file_system.h"
#include "enn_res_ptr.h"

namespace enn
{
class ResourceManager;
class Resource;

struct IResourceCreator
{
	virtual void release() = 0;
	virtual bool prepareResource( Resource* res, void* context ) = 0;
	virtual bool buildResource( Resource* res ) = 0;
};

struct IResourceListener
{
	virtual void onResourceLoaded( Resource* res ) {}
	virtual void onResourceBad( Resource* res ) {}
	virtual void onResourceUpdate( Resource* res ) {}
};

class ResourceNullCreator : public AllocatedObject, public IResourceCreator
{
public:
	virtual void release() { ENN_DELETE this; }
	virtual bool prepareResource( Resource* res, void* context ) { return true; }
	virtual bool buildResource( Resource* res ){ return true; }
};

class Resource : public Noncopyable, public AllocatedObject
{
	ENN_DECLARE_RTTI(Resource)

public:
	enum State
	{
		S_UNLOADED,
		S_LOADING,
		S_PREPARED_OK,
		S_PREPARED_FAILED,
		S_LOADED,
		S_BADRES,
		S_UNLOADING
	};

protected:
	typedef enn::set<IResourceListener*>::type ListenerList;

public:
	Resource() : manager_(0), res_creator_(0), 
		ref_count_(1), load_async_(false), status_(S_UNLOADED)
		, res_load_priority_(0)
	{

	}

	virtual ~Resource();

public:
	void setName(const String& name)
	{
		name_ = name;
	}

	const String& getName() const
	{
		return name_;
	}

	void setLoadPriority(uint32 pri)
	{
		res_load_priority_ = pri;
	}

	uint32 getLoadPriority() const
	{
		return res_load_priority_;
	}

	void setManager(ResourceManager* rm)
	{
		manager_ = rm;
	}

	ResourceManager* getManager() const
	{
		return manager_;
	}

	void setResCreator(IResourceCreator* creator);

	void setNullCreator();

	IResourceCreator* getResCreator() const
	{
		return res_creator_;
	}

	bool getLoadAsync() const
	{
		return load_async_;
	}

	void load(bool async);
	void unload(bool async);
	bool checkLoad();
	void update();

public:
	bool setLoadingStatus();
	void setPreparedStatus( bool ok );
	void setCompletedStatus( bool ok );

	bool setUnloadingStatus();
	void setUnloadedStatus();

	uint32 getStatus() const 
	{ 
		return status_; 
	}

	bool  isLoaded() const 
	{ 
		return status_ == S_LOADED; 
	}

public:
	void addRef() 
	{
		++ref_count_; 
	}

	void release();

	uint getRefCount() const 
	{ 
		return ref_count_; 
	}

public:
	virtual void destructResImpl() {}

public:
	void addListener(IResourceListener* listener);
	void removeListener(IResourceListener* listener);

public:
	void setLoadAsync(bool b);

public:
	void setUserData(void* user_data);
	void* getUserData() const;

protected:
	ResourceManager*            manager_;
	IResourceCreator*           res_creator_;
	String                      name_;
	ListenerList                listeners_;
	uint                        ref_count_;
	bool						load_async_;
	volatile uint32             status_;
	void*						user_data_;

	uint32						res_load_priority_;
};
}
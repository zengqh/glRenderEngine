/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/01
* File: enn_auto_name_manager.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_platform_headers.h"
#include "enn_math.h"

namespace enn
{
class AutoNameManager
{
public:
	AutoNameManager() : m_autoNameIndex(0) {}
	~AutoNameManager() {}

protected:
	int _getNextId()
	{
		int id = m_autoNameIndex;
		++m_autoNameIndex;
		return id;
	}

	String _getNextName( const String& baseName )
	{
		return baseName + to_string( _getNextId() );
	}

	String _getNextName()
	{
		static const String baseName = "_auto_";
		return _getNextName( baseName );
	}

#define _checkNextName(name) ((name).size() ? (name) : _getNextName())

protected:
	int m_autoNameIndex;
};
}


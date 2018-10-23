/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/11
* File: Rtti.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

namespace enn
{
	typedef const char* ClassType;
}


#define ENN_DECLARE_RTTI(cls) \
	public: \
	static ClassType classType() \
{ \
	static const char name_[] = #cls; \
	return name_; \
} \
	virtual ClassType getClassType() const \
{ \
	return classType(); \
}

#define ENN_CLASS_TYPE(T)         T::classType()
#define ENN_OBJECT_TYPE(obj)      (obj)->getClassType()
#define ENN_OBJECT_IS(obj, T)     ((obj)->getClassType() == T::classType())

#define ENN_TYPE_TO_NAME(type)    type
#define ENN_CLASS_TO_NAME(T)      ENN_TYPE_TO_NAME(ENN_CLASS_TYPE(T))
/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/25
* File: enn_std_header.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_os_headers.h"

/** standard c */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <math.h>

/** standard c++ */
#include <limits>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <bitset>

//////////////////////////////////////////////////////////////////////////
namespace enn
{
	typedef signed char 			int8;
	typedef short       			int16;
	typedef int         			int32;
	typedef long long   			int64;

	typedef unsigned char       	uint8;
	typedef unsigned short      	uint16;
	typedef unsigned int        	uint32;
	typedef unsigned long long  	uint64;

	typedef unsigned char   		uchar;
	typedef unsigned short  		ushort;
	typedef unsigned int    		uint;
	typedef unsigned long   		ulong;

	typedef char*       			pstr;
	typedef const char* 			pcstr;
}

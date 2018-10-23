/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/24
* File: enn_vector3.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_vector3.h"

namespace enn
{
	const Vector3 Vector3::ZERO( 0, 0, 0 );
	const Vector3 Vector3::UNIT_X( 1, 0, 0 );
	const Vector3 Vector3::UNIT_Y( 0, 1, 0 );
	const Vector3 Vector3::UNIT_Z( 0, 0, 1 );
	const Vector3 Vector3::NEGATIVE_UNIT_X( -1,  0,  0 );
	const Vector3 Vector3::NEGATIVE_UNIT_Y(  0, -1,  0 );
	const Vector3 Vector3::NEGATIVE_UNIT_Z(  0,  0, -1 );
	const Vector3 Vector3::UNIT_SCALE(1, 1, 1);
}
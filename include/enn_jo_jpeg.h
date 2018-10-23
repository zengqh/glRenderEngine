/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/03/30
* File: jo_jpeg.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include <enn_platform.h>

bool jo_write_jpg(const char *filename, const void *data, int width, int height, int comp, int quality);
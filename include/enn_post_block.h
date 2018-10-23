//
// Copyright (c) 2013-2014 the enn project.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// Time: 2013/10/12
// File: enn_post_block.h
//

#pragma once

#include "enn_platform_headers.h"

namespace enn
{

class RenderTarget;
class Camera;
class PostBlock : public AllocatedObject, public Noncopyable
{
public:
	PostBlock()
		: is_post_enable_(true)
	{

	}

	virtual ~PostBlock()
	{

	}

	virtual void set_post_enable(bool en)
	{
		is_post_enable_ = en;
	}

	virtual bool is_post_enable() const
	{
		return is_post_enable_;
	}

	virtual void prepare() = 0;
	virtual void render(RenderTarget* dest, RenderTarget* src, Camera* cam) = 0;

protected:
	bool is_post_enable_;
};

}
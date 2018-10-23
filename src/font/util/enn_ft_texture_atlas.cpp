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
// Time: 2013/09/18
// File: enn_ft_texture_atlas.cpp
//

#include "enn_ft_texture_atlas.h"

namespace enn
{

FTTextureAtlas::FTTextureAtlas()
: w_(0)
, h_(0)
, data_(0)
, tex_id_(0)
, is_first_upload_(true)
, used(0)
{

}

FTTextureAtlas::~FTTextureAtlas()
{
	destroy();
}

bool FTTextureAtlas::initalize(int w, int h)
{
	w_ = w;
	h_ = h;

	if (w_ <= 0 || h_ <= 0)
	{
		ENN_ASSERT(0);
		return false;
	}

	data_ = (uchar*)ENN_MALLOC(w_ * h_);

	// create first node.
	nv::vec3i* node = ENN_NEW_T(nv::vec3i);
	node->x = 1;
	node->y = 1;
	node->z = w - 2;

	nodes_.push_back(node);

	return true;
}

void FTTextureAtlas::setRegion(int x, int y, int w, int h, const uchar* data, int stride)
{
	ENN_ASSERT(x > 0);
	ENN_ASSERT(y > 0);
	ENN_ASSERT(x < (w_ - 1));
	ENN_ASSERT((x + w) <= (w_ - 1));
	ENN_ASSERT(y < (h_ - 1) );
	ENN_ASSERT(((y + h) <= (h_ - 1)));

	for (int i = 0; i < h; ++i)
	{
		memcpy(data_ + ((y + i) * w_ + x ), data + i * stride, w);
	}

	atlas_flags_.set_flag(ATLAS_DIRTY);
}

void FTTextureAtlas::upload()
{
	if (!atlas_flags_.test_flag(ATLAS_DIRTY))
	{
		return;
	}

	atlas_flags_.reset_flag(ATLAS_DIRTY);

	ENN_ASSERT(data_);

	if (!tex_id_)
	{
		glGenTextures(1, &tex_id_);
	}

	glBindTexture( GL_TEXTURE_2D, tex_id_);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	if (is_first_upload_)
	{
		glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA, w_, h_,
			0, GL_ALPHA, GL_UNSIGNED_BYTE, data_);
	}
	else
	{
		glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, w_, h_,
			GL_ALPHA, GL_UNSIGNED_BYTE, data_ );
	}

	is_first_upload_ = false;
}

bool FTTextureAtlas::getRegion(int w, int h, nv::vec4i& out_reg)
{
	if (atlas_flags_.test_flag(ATLAS_FULL))
	{
		return false;
	}

	int y, best_height, best_width;
	nv::vec3i *node, *prev;
	nv::vec4i region(0,0,w,h);
	NodesIter best_index = nodes_.end();

	best_height = Math::ENN_MAX_SIGNED;
	best_width = Math::ENN_MAX_SIGNED;

	ENN_FOR_EACH(NodesList, nodes_, it)
	{
		y = texture_atlas_fit(it, w, h );

		if( y >= 0 )
		{
			node = *it;
			if( ( (y + h) < best_height ) ||
				( ((y + h) == best_height) && (node->z < best_width)) )
			{
				best_height = y + h;
				best_index = it;
				best_width = node->z;
				region.x = node->x;
				region.y = y;
			}
		}
	}

	// when region is full, the data_ can be free here
	if (best_index == nodes_.end())
	{
		atlas_flags_.set_flag(ATLAS_FULL);

		upload();

		ENN_SAFE_FREE(data_);

		return false;
	}

	node = ENN_MALLOC_T(nv::vec3i);

	node->x = region.x;
	node->y = region.y + h;
	node->z = w;

	nodes_.insert(best_index, node);

	NodesIter best_idx_1 = best_index;

	for (; best_idx_1 != nodes_.end(); ++best_idx_1)
	{
		node = *best_idx_1;

		NodesIter prev_idx = best_idx_1;
		prev_idx--;

		prev = *prev_idx;

		if (node->x < (prev->x + prev->z) )
		{
			int shrink = prev->x + prev->z - node->x;
			node->x += shrink;
			node->z -= shrink;
			if (node->z <= 0)
			{
				// before erase free node.
				ENN_SAFE_FREE(node);
				best_idx_1 = nodes_.erase(best_idx_1);
				best_idx_1--;
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

	texture_atlas_merge();

	out_reg = region;

	used += w * h;

	return true;
}

bool FTTextureAtlas::bindTexture()
{
	upload();

	if (tex_id_ == 0)
	{
		ENN_ASSERT(0);
		return false;
	}

	glBindTexture(GL_TEXTURE_2D, tex_id_);

	return true;
}

void FTTextureAtlas::destroy()
{
	ENN_SAFE_FREE(data_);

	ENN_FOR_EACH(NodesList, nodes_, it)
	{
		nv::vec3i* node = *it;
		ENN_SAFE_FREE(node);
	}

	nodes_.clear();

	if (tex_id_)
	{
		glDeleteTextures( 1, &tex_id_);
		tex_id_ = 0;
		checkGLError();
	}
	
}

int FTTextureAtlas::texture_atlas_fit(NodesIter index, int width, int height)
{
	nv::vec3i *node;
	int x, y, width_left;
	NodesIter i;

	node = *index;

	x = node->x;
	y = node->y;
	width_left = width;
	i = index;

	if ( (x + width) > (w_-1) )
	{
		return -1;
	}

	y = node->y;
	while( width_left > 0 )
	{
		node = *i;
		if( node->y > y )
		{
			y = node->y;
		}
		if( (y + height) > (h_-1) )
		{
			return -1;
		}
		width_left -= node->z;
		++i;
	}
	return y;
}

void FTTextureAtlas::texture_atlas_merge()
{
	nv::vec3i *node, *next;

	ENN_FOR_EACH(NodesList, nodes_, it)
	{
		NodesIter next_it = it;
		next_it++;

		if (next_it == nodes_.end())
		{
			break;
		}

		node = *it;
		next = *next_it;

		if( node->y == next->y )
		{
			node->z += next->z;

			ENN_SAFE_FREE(next);
			nodes_.erase(next_it);
		}
	}
}

}
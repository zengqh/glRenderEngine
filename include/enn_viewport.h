/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/24
* File: enn_viewport.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_math.h"

namespace enn
{
	class RenderTarget;
	class Camera;
	class Viewport : public AllocatedObject
	{
	public:
		Viewport()
			: m_clearFlags(0)
			, z_order_(0)
			, parent_(0)
		{
			
		}

		~Viewport()
		{

		}

	public:
		void setRect(const IntRect& rect)
		{
			m_rect = rect;
		}

		void setRect( int l, int t, int r, int b )
		{
			m_rect.set( l, t, r, b );
		}

		void makeRect( int l, int t, int w, int h )
		{
			m_rect.make( l, t, w, h );
		}

		const IntRect& getRect() const
		{
			return m_rect;
		}

		int getWidth()  const { return m_rect.getWidth(); }
		int getHeight() const { return m_rect.getHeight(); }

		void setClearFlag( uint32 flags ) { m_clearFlags = flags; }
		void setClearColor( const Color& clr ) { m_clearColor = clr; }

		void setZOrder(int z_order)
		{
			z_order_ = z_order;
		}

		int getZOrder() const
		{
			return z_order_;
		}

		void setParent(RenderTarget* rtt);

		RenderTarget* getParent() const;

	public:
		void link_camera(Camera* cam);

		Camera* getCamera() const;

		void apply();

		void applyOnlyVP();

	protected:
		IntRect       		m_rect;

		uint32        		m_clearFlags;
		Color         		m_clearColor;

		int			  		z_order_;
		RenderTarget*		parent_;

		Camera*				cam_;
	};
}
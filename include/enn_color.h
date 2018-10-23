/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/24
* File: enn_color.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_platform_headers.h"
#include "enn_math_def.h"

namespace enn
{
	class Vector3;
	class Vector4;
	class Color
	{
	public:
		static const Color ZERO;
		static const Color BLACK;
		static const Color WHITE;
		static const Color BLUE;

		static const float COLOR_EPSILON;

	public:
		float r, g, b, a;

	public:
		Color() : r(0.0f), g(0.0f), b(0.0f), a(0.0f) {}

		Color(const float* c)
		{
			r = c[0];
			g = c[1];
			b = c[2];
			a = c[3];
		}

		explicit Color( float red, float green, float blue, float alpha = 1.0f ) : r(red), g(green), b(blue), a(alpha) {}

	public:
		bool operator==( const Color& rhs ) const
		{
			return (r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a);
		}

		bool operator!=( const Color& rhs ) const
		{
			return !(*this == rhs);
		}

		Color operator+( const Color& rkVector ) const
		{
			Color kSum;

			kSum.r = r + rkVector.r;
			kSum.g = g + rkVector.g;
			kSum.b = b + rkVector.b;
			kSum.a = a + rkVector.a;

			return kSum;
		}

		Color operator-( const Color& rkVector ) const
		{
			Color kDiff;

			kDiff.r = r - rkVector.r;
			kDiff.g = g - rkVector.g;
			kDiff.b = b - rkVector.b;
			kDiff.a = a - rkVector.a;

			return kDiff;
		}

		Color operator*( const float fScalar ) const
		{
			Color kProd;

			kProd.r = fScalar * r;
			kProd.g = fScalar * g;
			kProd.b = fScalar * b;
			kProd.a = fScalar * a;

			return kProd;
		}

		Color operator*( const Color& rhs ) const
		{
			Color kProd;

			kProd.r = rhs.r * r;
			kProd.g = rhs.g * g;
			kProd.b = rhs.b * b;
			kProd.a = rhs.a * a;

			return kProd;
		}

		Color operator/( const Color& rhs ) const
		{
			Color kProd;

			kProd.r = r / rhs.r;
			kProd.g = g / rhs.g;
			kProd.b = b / rhs.b;
			kProd.a = a / rhs.a;

			return kProd;
		}

		Color operator/( const float fScalar ) const
		{
			ENN_ASSERT( fScalar != 0.0f );

			Color kDiv;

			float fInv = 1.0f / fScalar;
			kDiv.r = r * fInv;
			kDiv.g = g * fInv;
			kDiv.b = b * fInv;
			kDiv.a = a * fInv;

			return kDiv;
		}

		friend Color operator*( const float fScalar, const Color& rkVector )
		{
			Color kProd;

			kProd.r = fScalar * rkVector.r;
			kProd.g = fScalar * rkVector.g;
			kProd.b = fScalar * rkVector.b;
			kProd.a = fScalar * rkVector.a;

			return kProd;
		}

		Color& operator+=( const Color& rkVector )
		{
			r += rkVector.r;
			g += rkVector.g;
			b += rkVector.b;
			a += rkVector.a;

			return *this;
		}

		Color& operator-=( const Color& rkVector )
		{
			r -= rkVector.r;
			g -= rkVector.g;
			b -= rkVector.b;
			a -= rkVector.a;

			return *this;
		}

		Color& operator*=( const float fScalar )
		{
			r *= fScalar;
			g *= fScalar;
			b *= fScalar;
			a *= fScalar;

			return *this;
		}

		Color& operator/=( const float fScalar )
		{
			ENN_ASSERT( fScalar != 0.0f );

			float fInv = 1.0f / fScalar;

			r *= fInv;
			g *= fInv;
			b *= fInv;
			a *= fInv;

			return *this;
		}

	public:
		uint32 getAsRGBA() const;
		uint32 getAsARGB() const;
		uint32 getAsBGRA() const;
		uint32 getAsABGR() const;

		Vector3 getAsVector3() const;
		Vector4 getAsVector4() const;

		void setAsRGBA( uint32 val );
		void setAsARGB( uint32 val );
		void setAsBGRA( uint32 val );
		void setAsABGR( uint32 val );

		void setRGB( float r, float g, float b )
		{
			this->r = r;
			this->g = g;
			this->b = b;
		}

		void setRGB( const Color& rgb )
		{
			r = rgb.r;
			g = rgb.g;
			b = rgb.b;
		}

	public:
		static bool isEqual( float a, float b )
		{
			return Math::realEqual( a, b, COLOR_EPSILON );
		}

		bool isZeroRGB() const
		{
			return Math::realEqual( r, 0.0f, COLOR_EPSILON ) &&
				Math::realEqual( g, 0.0f, COLOR_EPSILON ) &&
				Math::realEqual( b, 0.0f, COLOR_EPSILON );
		}

		bool isZeroAlpha() const
		{
			return Math::realEqual( a, 0.0f, COLOR_EPSILON );
		}

		bool isZeroAll()
		{
			return isZeroRGB() && isZeroAlpha();
		}

		bool isWhiteRGB() const
		{
			return Math::realEqual( r, 1.0f, COLOR_EPSILON ) &&
				Math::realEqual( g, 1.0f, COLOR_EPSILON ) &&
				Math::realEqual( b, 1.0f, COLOR_EPSILON );
		}

		bool isWhiteAlpha() const
		{
			return Math::realEqual( a, 1.0f, COLOR_EPSILON );
		}

		bool isWhite()
		{
			return isWhiteRGB() && isWhiteAlpha();
		}

	public:
		void saturate()
		{
			if ( r < 0 )
				r = 0;
			else if ( r > 1 )
				r = 1;

			if ( g < 0 )
				g = 0;
			else if ( g > 1 )
				g = 1;

			if ( b < 0 )
				b = 0;
			else if ( b > 1 )
				b = 1;

			if ( a < 0 )
				a = 0;
			else if ( a > 1 )
				a = 1;
		}

		Color saturateCopy() const
		{
			Color ret = *this;
			ret.saturate();
			return ret;
		}

	public:
		float operator[]( size_t i ) const
		{
			ENN_ASSERT( i < 4 );
			return *(&r+i);
		}

		float& operator[]( const size_t i )
		{
			ENN_ASSERT( i < 4 );
			return *(&r+i);
		}

		const float* ptr() const
		{
			return &r;
		}

		float* ptr()
		{
			return &r;
		}
	};
}
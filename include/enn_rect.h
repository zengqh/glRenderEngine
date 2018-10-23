/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/24
* File: enn_rect.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_platform_headers.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
struct IntRect
{
public:
	IntRect() : left(0), top(0), right(0), bottom(0) {}
	IntRect( int l, int t, int r, int b ) : left(l), top(t), right(r), bottom(b) {}

public:
	void set( int l, int t, int r, int b )
	{
		left   = l;
		top    = t;
		right  = r;
		bottom = b;
	}

	void make( int l, int t, int w, int h )
	{
		left   = l;
		top    = t;
		right  = l + w;
		bottom = t + h;
	}

	int getWidth()  const { return right - left; }
	int getHeight() const { return bottom - top; }

public:
	int left;
	int top;
	int right;
	int bottom;
};

//////////////////////////////////////////////////////////////////////////
// rectangle
template <typename T> class Rectangle
{
public:

    /**
     * Specifies the x-coordinate of the rectangle.
     */
    T x;

    /**
     * Specifies the y-coordinate of the rectangle.
     */
    T y;

    /**
     * Specifies the width of the rectangle.
     */
    T width;

    /**
     * Specifies the height of the rectangle.
     */
    T height;

    /**
     * Constructs a new rectangle initialized to all zeros.
     */
    Rectangle();

    /**
     * Constructs a new rectangle with the x = 0, y = 0 and the specified width and height.
     *
     * @param width The width of the rectangle.
     * @param height The height of the rectangle.
     */
    Rectangle(T width, T height);

    /**
     * Constructs a new rectangle with the specified x, y, width and height.
     *
     * @param x The x-coordinate of the rectangle.
     * @param y The y-coordinate of the rectangle.
     * @param width The width of the rectangle.
     * @param height The height of the rectangle.
     */
    Rectangle(T x, T y, T width, T height);

    /**
     * Constructs a new rectangle that is a copy of the specified rectangle.
     *
     * @param copy The rectangle to copy.
     */
    Rectangle(const Rectangle& copy);

    /**
     * Destructor.
     */
    ~Rectangle();

    /**
     * Returns a rectangle with all of its values set to zero.
     *
     * @return The empty rectangle with all of its values set to zero.
     */
    static const Rectangle& empty();

    /**
     * Gets a value that indicates whether the rectangle is empty.
     *
     * @return true if the rectangle is empty, false otherwise.
     */
    bool isEmpty() const;

    /**
     * Sets the values of this rectangle to the specified values.
     *
     * @param x The x-coordinate of the rectangle.
     * @param y The y-coordinate of the rectangle.
     * @param width The width of the rectangle.
     * @param height The height of the rectangle.
     */
    void set(T x, T y, T width, T height);

    /**
     * Sets the values of this rectangle to those in the specified rectangle.
     *
     * @param r The rectangle to copy.
     */
    void set(const Rectangle& r);

    /**
     * Sets the x-coordinate and y-coordinate values of this rectangle to the specified values.
     *
     * @param x The x-coordinate of the rectangle.
     * @param y The y-coordinate of the rectangle.
     */
    void setPosition(T x, T y);

    /**
     * Returns the x-coordinate of the left side of the rectangle.
     *
     * @return The x-coordinate of the left side of the rectangle.
     */
    T left() const;

    /**
     * Returns the y-coordinate of the top of the rectangle.
     *
     * @return The y-coordinate of the top of the rectangle.
     */
    T top() const;

    /**
     * Returns the x-coordinate of the right side of the rectangle.
     *
     * @return The x-coordinate of the right side of the rectangle.
     */
    T right() const;

    /**
     * Returns the y-coordinate of the bottom of the rectangle.
     *
     * @return The y-coordinate of the bottom of the rectangle.
     */
    T bottom() const;

    /**
     * Determines whether this rectangle contains a specified point.
     *
     * @param x The x-coordinate of the point.
     * @param y The y-coordinate of the point.
     * 
     * @return true if the rectangle contains the point, false otherwise.
     */
    bool contains(T x, T y) const;

    /**
     * Determines whether this rectangle contains a specified rectangle.
     *
     * @param x The x-coordinate of the rectangle.
     * @param y The y-coordinate of the rectangle.
     * @param width The width of the rectangle.
     * @param height The height of the rectangle.
     * 
     * @return true if the rectangle contains the specified rectangle, false
     * otherwise.
     */
    bool contains(T x, T y, T width, T height) const;

    /**
     * Determines whether this rectangle contains a specified rectangle.
     *
     * @param r The rectangle.
     * 
     * @return true if the rectangle contains the specified rectangle, false
     * otherwise.
     */
    bool contains(const Rectangle& r) const;

    /**
     * Determines whether a specified rectangle intersects with this rectangle.
     * Rectangles intersect if there is a common point that is contained in both rectangles.
     *
     * @param x The x-coordinate of the rectangle.
     * @param y The y-coordinate of the rectangle.
     * @param width The width of the rectangle.
     * @param height The height of the rectangle.
     * 
     * @return true if the specified Rectangle intersects with this one, false otherwise.
     */
    bool intersects(T x, T y, T width, T height) const;

    /**
     * Determines whether a specified rectangle intersects with this rectangle.
     *
     * @param r The rectangle.
     * 
     * @return true if the specified rectangle intersects with this one, false
     * otherwise.
     */
    bool intersects(const Rectangle& r) const;

    /**
     * Returns a new rectangle that exactly contains two other rectangles.
     *
     * @param r1 The first rectangle to contain.
     * @param r2 The second rectangle to contain.
     * @param dst A rectangle to store the union of the two rectangle parameters.
     */
    static void combine(const Rectangle& r1, const Rectangle& r2, Rectangle* dst);

    /**
     * Pushes the edges of the Rectangle out by the horizontal and vertical values specified.
     *
     * Each corner of the Rectangle is pushed away from the center of the rectangle
     * by the specified amounts. This results in the width and height of the Rectangle
     * increasing by twice the values provided.
     *
     * @param horizontalAmount The value to push the sides out by.
     * @param verticalAmount The value to push the top and bottom out by.
     */
    void inflate(T horizontalAmount, T verticalAmount);

    /**
     * operator =
     */
    Rectangle& operator = (const Rectangle& r);

    /**
     * operator ==
     */
    bool operator == (const Rectangle& r) const;

    /**
     * operator !=
     */
    bool operator != (const Rectangle& r) const;
};

// impl

template <typename T>
Rectangle<T>::Rectangle()
: x(0), y(0), width(0), height(0)
{
}

template <typename T>
Rectangle<T>::Rectangle(T width, T height) :
x(0), y(0), width(width), height(height)
{
}

template <typename T>
Rectangle<T>::Rectangle(T x, T y, T width, T height) :
x(x), y(y), width(width), height(height)
{
}

template <typename T>
Rectangle<T>::Rectangle(const Rectangle& copy)
{
	set(copy);
}

template <typename T>
Rectangle<T>::~Rectangle()
{
}

template <typename T>
const Rectangle<T>& Rectangle<T>::empty()
{
	static Rectangle<T> empty;
	return empty;
}

template <typename T>
bool Rectangle<T>::isEmpty() const
{
	return (x == 0 && y == 0 && width == 0 && height == 0);
}

template <typename T>
void Rectangle<T>::set(const Rectangle<T>& r)
{
	set(r.x, r.y, r.width, r.height);
}

template <typename T>
void Rectangle<T>::set(T x, T y, T width, T height)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
}

template <typename T>
void Rectangle<T>::setPosition(T x, T y)
{
	this->x = x;
	this->y = y;
}

template <typename T>
T Rectangle<T>::left() const
{
	return x;
}

template <typename T>
T Rectangle<T>::top() const
{
	return y;
}

template <typename T>
T Rectangle<T>::right() const
{
	return x + width;
}

template <typename T>
T Rectangle<T>::bottom() const
{
	return y + height;
}

template <typename T>
bool Rectangle<T>::contains(T x, T y) const
{
	return (x >= this->x && x <= (this->x + width) && y >= this->y && y <= (this->y + height));
}

template <typename T>
bool Rectangle<T>::contains(T x, T y, T width, T height) const
{
	return (contains(x, y) && contains(x + width, y + height));
}

template <typename T>
bool Rectangle<T>::contains(const Rectangle& r) const
{
	return contains(r.x, r.y, r.width, r.height);
}

template <typename T>
bool Rectangle<T>::intersects(T x, T y, T width, T height) const
{
	T t;
	if ((t = x - this->x) > this->width || -t > width)
		return false;
	if ((t = y - this->y) > this->height || -t > height)
		return false;
	return true;
}

template <typename T>
bool Rectangle<T>::intersects(const Rectangle<T>& r) const
{
	return intersects(r.x, r.y, r.width, r.height);
}

template <typename T>
void Rectangle<T>::combine(const Rectangle<T>& r1, const Rectangle<T>& r2, Rectangle<T>* dst)
{
	ENN_ASSERT(dst);

	dst->x = min(r1.x, r2.x);
	dst->y = min(r1.y, r2.y);
	dst->width = max(r1.x + r1.width, r2.x + r2.width) - dst->x;
	dst->height = max(r1.y + r1.height, r2.y + r2.height) - dst->y;
}

template <typename T>
void Rectangle<T>::inflate(T horizontalAmount, T verticalAmount)
{
	x -= horizontalAmount;
	y -= verticalAmount;
	width += horizontalAmount * 2;
	height += verticalAmount * 2;
}

template <typename T>
Rectangle<T>& Rectangle<T>::operator = (const Rectangle<T>& r)
{
	x = r.x;
	y = r.y;
	width = r.width;
	height = r.height;
	return *this;
}

template <typename T>
bool Rectangle<T>::operator == (const Rectangle<T>& r) const
{
	return (x == r.x && width == r.width && y == r.y && height == r.height);
}

template <typename T>
bool Rectangle<T>::operator != (const Rectangle<T>& r) const
{
	return (x != r.x || width != r.width || y != r.y || height != r.height);
}

typedef Rectangle<int>	IntRectangle;
typedef Rectangle<float> Rectanglef;
typedef Rectangle<double> Rectangled;

inline void initFromIntRect(IntRectangle& dst, const IntRect& src)
{
	dst.set(src.left, src.top, src.getWidth(), src.getHeight());
}

}
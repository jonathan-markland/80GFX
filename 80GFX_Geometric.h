//
// 80GFX - Totally retro 1980s graphics library for C++ with no dependencies.
// Copyright (C) 2017  Jonathan Markland
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//		jynx_emulator {at} yahoo {dot} com
//


#pragma once

#include <stdint.h>
#include <algorithm>

// TODO:  The template nature suggests instantiation with eg: type "double"
//        is possible.  However, not all templates are checked for this, in particular
//        integer division may be relied upon.


#define TEM   template<typename T>
#define POI   Point<T>
#define REC   Rect<T>
#define SIZ   Size<T>

TEM struct Point;
TEM struct Rect;
TEM struct Size;

TEM inline void Initialise( POI &p )
{
	p.x = 0;
	p.y = 0;
}

TEM inline void Initialise( REC &r )
{
	r.left   = 0;
	r.top    = 0;
	r.right  = 0;
	r.bottom = 0;
}

TEM inline void Initialise( SIZ &s )
{
	s.cx = 0;
	s.cy = 0;
}

TEM struct Point
{
	Point()                       { Initialise(*this); }
	Point( T xx, T yy )           : x(xx), y(yy) {}
	T x;
	T y;
};

TEM struct Rect
{
	Rect()                        { Initialise(*this); }
	Rect( T l, T t, T r, T b )    : left(l), top(t), right(r), bottom(b) {}
	Rect( POI p, T w, T h )       : left(p.x), top(p.y), right(p.x+w), bottom(p.y+h) {}
	T left;
	T top;
	T right;
	T bottom;
};

TEM struct Size
{
	Size()                        { Initialise(*this); }
	Size( T cxx, T cyy )          : cx(cxx), cy(cyy) {}
	T cx;
	T cy;
};

typedef Rect<int32_t> Rectangle;

//
// Rectangle query functions
//

TEM inline T Width(  REC r )          { return r.right - r.left; }
TEM inline T Height( REC r )          { return r.bottom - r.top; }
TEM inline POI Centre( REC r )        { return POI( (r.left+r.right)/2, (r.top+r.bottom)/2 ); }
TEM inline bool HasArea( REC r )      { return Width(r) > 0 && Height(r) > 0; }

TEM inline POI TopLeft( REC r )       { return POI(r.left, r.top); }
TEM inline POI TopRight( REC r )      { return POI(r.right, r.top); }
TEM inline POI BottomLeft( REC r )    { return POI(r.left, r.bottom); }
TEM inline POI BottomRight( REC r )   { return POI(r.right, r.bottom); }

TEM inline SIZ SizeOf( REC r )        { return SIZ(Width(r), Height(r)); }

//
// Exact match testing
//

TEM inline bool operator==( POI p1, POI p2 ) { return p1.x == p2.x && p1.y == p2.y; }
TEM inline bool operator!=( POI p1, POI p2 ) { return !( p1 == p2 ); }
TEM inline bool operator==( REC r1, REC r2 ) { return r1.left == r2.left && r1.right == r2.right && r1.top == r2.top && r1.bottom == r2.bottom; }
TEM inline bool operator!=( REC r1, REC r2 ) { return !( r1 == r2 ); }

//
// Rectify
//

TEM inline REC Rectify( REC r )
{
	return REC(
		std::min(r.left, r.right),
		std::min(r.top, r.bottom),
		std::max(r.left, r.right),
		std::max(r.top, r.bottom) );
}

//
// Intersection testing
// IsOff(): Don't forget that right/bottom is NON INCLUSIVE, left/top IS inclusive!
//

TEM inline bool IsInside( POI p, REC r )     { return (p.x >= r.left) && (p.y >= r.top) && (p.x < r.right) && (p.y < r.bottom); }
TEM inline bool IsOff( REC r1, REC r2 )      { return r1.right <= r2.left || r1.left >= r2.right ||r1.top >= r2.bottom ||r1.bottom <= r2.top; }
TEM inline bool Intersects( REC r1, REC r2 ) { return !IsOff( r1,r2 ); }
TEM inline bool Intersects( POI p, REC r )   { return (p.x >= r.left) && (p.x < r.right) && (p.y >= r.top) && (p.y < r.bottom); }
TEM inline bool InsideOut( REC r )           { return r.left > r.right || r.top > r.bottom; }

//
// Size testing
//

TEM inline bool IsTooSmall( REC r, T MinSize )   { return min(Width(r),Height(r)) < MinSize; }

//
// Compute Intersection
//

TEM inline REC Intersection( REC r, REC s )
{
	return REC(
		std::max( r.left,   s.left ),
		std::max( r.top,    s.top ),
		std::min( r.right,  s.right ),
		std::min( r.bottom, s.bottom ) );
}
TEM inline REC operator&( REC r, REC s ) { return Intersection(r,s); }

//
// Compute boundary rectangle from two arbitrary points
//

TEM inline REC GetBoundingRectangle( POI p1, POI p2 )
{
	return REC(
		std::min( p1.x, p2.x ),
		std::min( p1.y, p2.y ),
		std::max( p1.x, p2.x ),
		std::max( p1.y, p2.y ) );
}

//
// Union
//

TEM inline REC Union( REC r, REC s )
{
	return REC(
		std::min(r.left, s.left),
		std::min(r.top, s.top),
		std::max(r.right, s.right),
		std::max(r.bottom, s.bottom) );
}
TEM inline REC Union( REC r, POI p )
{
	return REC(
		std::min(r.left, p.x),
		std::min(r.top, p.y),
		std::max(r.right, p.x),
		std::max(r.bottom, p.y) );
}
TEM inline REC operator|( REC r, REC s ) { return Union(r,s); }
TEM inline REC operator|( REC r, POI p ) { return Union(r,p); }

//
// Inflate a REC by the given number of units.
//

TEM inline REC Inflate( REC r, T dx, T dy )
{
	return REC( r.left-dx, r.top-dy, r.right+dx, r.bottom+dy );
}

//
// Return the REC that is 'r' centred about 'p'.
//

TEM inline REC CentreAbout( REC r, POI p )
{
	T w = Width(r);
	T h = Height(r);
	T l = p.x-(w/2);
	T t = p.y-(h/2);
	return REC( l, t, l+w, t+h );
}

//
// Shunt
//

TEM inline REC ShuntBy( REC r, T dx, T dy )
{
	return REC( r.left+dx, r.top+dy, r.right+dx, r.bottom+dy );
}

#undef TEM
#undef POI
#undef REC
#undef SIZ



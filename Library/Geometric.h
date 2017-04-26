
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

//
// Return the REC that is 'r' inflated to percentage 'Pc'.
// So 80 = return REC 80% of the size.
// The returned REC has its centre aligned with the centre of 'r'.
//

TEM inline REC InflateToPercentage( REC r, T Pc )
{
	REC r2 = REC( 0,0, MulDiv(Width(r),Pc,100), MulDiv(Height(r),Pc,100) );
	return CentreAbout( r2, Centre(r) );
}

//
// Positioning of one rectangle relative to another:
//
//    Vertically:     Above,  Tops,  Centre, Bottoms, Below
//    Horizontally:   LeftOf, Lefts, Centre, Rights,  RightOf
//
//    (NB: Not all possibilities shown on diagram -- inners missed out)
//
//            1   2->  +-3-+  <-4   5
//             \  |    |   |    | /
//          g v---+-------------+---v 6
//                |             |
//            +---|             |---+
//          f |   |             |   | 7
//            +---|             |---+
//                |             |
//          e ^---+-------------+---^ 8
//             /  |    |   |    | \      .
//            d   c->  +-b-+  <-a   9
//

namespace RectPos
{
	namespace Horizontal
	{
		enum Enum { LeftOf, Lefts, Centre, Rights, RightOf };
	}
	namespace Vertical
	{
		enum Enum { Above, Tops, Centre, Bottoms, Below };
	}
}

namespace Internal
{
	TEM inline void CalcPosRelTo( T l, T r, T cx, T &outL, T &outR, uint32_t Flags )
	{
		if( Flags == 0 ) // LeftOf / Above
		{
			outL = l - cx;
			outR = l;
		}
		else if( Flags == 1 ) // Lefts / Tops
		{
			outL = l;
			outR = l + cx;
		}
		else if( Flags == 2 ) // Centre
		{
			outL = ((l + r) - cx) / 2;
			outR = outL + cx;
		}
		else if( Flags == 3 ) // Rights / Bottoms
		{
			outL = r - cx;
			outR = r;
		}
		else if( Flags == 4 ) // RightOf / Below
		{
			outL = r;
			outR = r + cx;
		}
	}
}

TEM inline REC RectPosRelTo( REC Host, SIZ Size, RectPos::Horizontal::Enum hFlags, RectPos::Vertical::Enum vFlags )
{
	REC r;
	Internal::CalcPosRelTo( Host.left, Host.right, Size.cx, r.left, r.right, uint32_t(hFlags) );
	Internal::CalcPosRelTo( Host.top, Host.bottom, Size.cy, r.top, r.bottom, uint32_t(vFlags) );
	return r;
}

//
// Bring a child rectangle into the area of a parent with the least movement.
// If child is larger than the parent, bias towards top left of child remaining visible.
//

namespace Internal
{
	TEM inline void BringOnto( T &cl, T &cr, T pl, T pr )
	{
		if( cr > pr )
		{
			T w = cr-cl;
			cr = pr;
			cl = pr - w;
		}
		if( cl < pl )
		{
			T w = cr-cl;
			cl = pl;
			cr = pl + w;
		}
	}
}

TEM inline REC BringOnto( REC Child, REC Parent )
{
	Internal::BringOnto( Child.left, Child.right, Parent.left, Parent.right );
	Internal::BringOnto( Child.top, Child.bottom, Parent.top, Parent.bottom );
	return Child;
}



TEM inline REC CentreRectangleInArea( T hostWidth, T hostHeight, T guestWidth, T guestHeight )
{
    auto left = (hostWidth - guestWidth) / 2;
    auto top  = (hostHeight - guestHeight) / 2;
    return Rectangle( left, top, left + guestWidth, top + guestHeight );
}



TEM inline REC GetBestFitProjectionArea( T hostWidth, T hostHeight, T guestWidth, T guestHeight )
{
    // Division by zero protection:

    if( guestWidth == 0 || guestHeight == 0 )
    {
        return CentreRectangleInArea( hostWidth, hostHeight, 0, 0 );
    }

    // Try scaling guest to fit host width first.  See if resulting guest height fits host height.

    auto resultGuestHeight = ((int64_t) hostWidth * guestHeight) / guestWidth;
    if( resultGuestHeight <= hostHeight )
    {
        return CentreRectangleInArea( hostWidth, hostHeight, hostWidth, (T) resultGuestHeight );
    }

    // Else, scale guest to fit host height instead.  Calculate what the resulting guest width would be:

    auto resultGuestWidth = ((int64_t) hostHeight * guestWidth) / guestHeight;
    return CentreRectangleInArea( hostWidth, hostHeight, (T) resultGuestWidth, hostHeight );
}



TEM inline REC GetSquarePixelsProjectionArea( T hostWidth, T hostHeight, T guestWidth, T guestHeight )
{
    // Strategy: Get the best fit area, reduce it to square pixels, re-centre it.

    auto bestFitRect = GetBestFitProjectionArea( hostWidth, hostHeight, guestWidth, guestHeight );

    // If the best fit rectangle means that we couldn't even achieve 1:1
    // then return the 1:1 rectangle centred in the host.  This will clip on
    // windowed systems, but retain 1:1 pixels.

    auto bestWidth  = Width(bestFitRect);
    auto bestHeight = Height(bestFitRect);

    if( bestWidth < guestWidth || bestHeight < guestHeight
        || guestWidth == 0 || guestHeight == 0 ) // (division by 0 protection for the case below).
    {
        return CentreRectangleInArea( hostWidth, hostHeight, guestWidth, guestHeight );
    }

    // Calculate cluster sizes with integer division:

    auto clusterSizeX = bestWidth / guestWidth;
    auto clusterSizeY = bestHeight / guestHeight;
    auto minCluster   = std::min( clusterSizeX, clusterSizeY );

    // Calculate a new guest size that will give us integral sized pixels in the projection,
    // and centre it by return:

    return CentreRectangleInArea( hostWidth, hostHeight, minCluster * guestWidth, minCluster * guestHeight );
}



#undef TEM
#undef POI
#undef REC
#undef SIZ



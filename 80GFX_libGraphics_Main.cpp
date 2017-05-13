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


// TODO: Use of exception raising - to be considered.

#include "80GFX_MetaFile.h"
#include "80GFX_libGraphics_Main.h"


#define METAFILE_PIXELS_MAX     32768

#define META_IN                 ">"   /// indent
#define META_OUT                "<"   /// un-indent

#define META_PEN_SOLID          "pixelpen"
#define META_PEN_THICK          "thickpen"
#define META_BRUSH_SOLID        "solidbrush"
#define META_BRUSH_PATTERNED    "patbrush"
#define META_BRUSH_AVERAGE_MIX  "avgbrush"
#define META_BRUSH_ANDXOR       "andxorbrush"
#define META_COLOUR_FOREGROUND  "fg"
#define META_COLOUR_BACKGROUND  "bg"
#define META_VIEWPORT           "viewport"
#define META_FONT               "font"
#define META_BITMAP_BITS_COLOUR "bitmap_colour_def"
#define META_BITMAP_BITS_MONO   "bitmap_mono_def"

#define META_MOVETO             "moveto"
#define META_LINETO             "lineto"
#define META_ARC                "arc"
#define META_SECANT             "secant"
#define META_ELLIPSE            "ellipse"
#define META_PIE                "pie"
#define META_RECTANGLE          "rectangle"
#define META_BEZIER             "bezier"
#define META_TRIANGLE           "triangle"
#define META_LINE               "line"
#define META_LINES              "lines"
#define META_POLY               "poly"
#define META_DRAW_BITMAP        "bitmap"
#define META_TEXT               "text"








inline int32_t MonoWidthPixelsToBytes( int32_t widthPixels )
{
	int32_t byteCount = widthPixels / 8;
	if( widthPixels & 7 ) ++byteCount;
	return byteCount;
}








const int32_t BezierConstants[15*4] =
{
	3375,  675,   45,    1,
	2744, 1176,  168,    8,
	2197, 1521,  351,   27,
	1728, 1728,  576,   64,
	1331, 1815,  825,  125,
	1000, 1800, 1080,  216,
	 729, 1701, 1323,  343,
	 512, 1536, 1536,  512,
	 343, 1323, 1701,  729,
	 216, 1080, 1800, 1000,
	 125,  825, 1815, 1331,
	  64,  576, 1728, 1728,
	  27,  351, 1521, 2197,
	   8,  168, 1176, 2744,
	   1,   45,  675, 3375
};



namespace libBasic
{
	namespace MetaOut
	{
		void Add( SmallStringBuilder &tmpstr, intptr_t x, intptr_t y )
		{
			Add( tmpstr, x );
			Add( tmpstr, y );
		}

		void Add( SmallStringBuilder &tmpstr, const Rect<int32_t> &r )
		{
			Add( tmpstr, r.left, r.top );
			Add( tmpstr, r.right, r.bottom );
		}
	}
}







namespace libGraphics
{
	namespace Pens
	{
		bool Solid::IsSolid() const
		{
			return true;
		}
		
		bool Solid::IsThick() const
		{
			return false;
		}
		
		uint32_t Solid::KludgeGetColour()
		{
			return this->Colour;
		}

		std::shared_ptr<Brushes::AbstractBrush> Solid::KludgeGetBrush()
		{
			return nullptr; // never called
		}
		
		uint32_t Solid::KludgeGetThickness()
		{
			return 0;   // never called
		}

		void Solid::ToMetafileText( libBasic::AbstractTextOutputStream *logStream )
		{
			libBasic::MetaOut::MetaFileWriter metaWriter( _outputTextStream );
			metaWriter.Start( META_PEN_SOLID );
			metaWriter.Add( this->Colour );
			metaWriter.Done();
		}

		// -------------------------------------------------------------------------------
		
		bool ThickPen::IsSolid() const
		{
			return false;
		}
		
		bool ThickPen::IsThick() const
		{
			return true;
		}
		
		uint32_t ThickPen::KludgeGetColour()
		{
			return 0;  // never called
		}
		
		std::shared_ptr<Brushes::AbstractBrush> ThickPen::KludgeGetBrush()
		{
			return this->Brush;
		}
		
		uint32_t ThickPen::KludgeGetThickness()
		{
			return this->Thickness;
		}

		void ThickPen::ToMetafileText( libBasic::AbstractTextOutputStream *logStream )
		{
			libBasic::MetaOut::MetaFileWriter metaWriter( _outputTextStream );
			metaWriter.Start( META_IN META_PEN_THICK );
			metaWriter.Add( this->Thickness );
			metaWriter.Done();
			this->Brush->ToMetafileText( logStream );
			metaWriter.Start( META_OUT META_PEN_THICK );
			metaWriter.Done();
		}
		
	} /// end namespace

} /// end namespace













namespace libGraphics
{
	namespace Brushes
	{
		// Solid brush:

		void Solid::PaintRasterRectangle( uint32_t *destinationImage, intptr_t rowStrafeInBytes, int32_t left, int32_t top, int32_t widthPixels, int32_t heightPixels )
		{
			System::Raster::PaintFilledRectangle(
				destinationImage, rowStrafeInBytes, left, top, widthPixels, heightPixels,
				System::Raster::PaintRasterInSolidBrush<uint32_t,int32_t>,
				Settings );
		}

		void Solid::PaintRasterLR( uint32_t *destinationImage, intptr_t rowStrafeInBytes, int32_t viewportTop, System::Raster::RasterLR<int32_t> *lrArray, int32_t heightPixels )
		{
			System::Raster::PaintLRArray(
				destinationImage, rowStrafeInBytes, viewportTop, lrArray, heightPixels,
				System::Raster::PaintRasterInSolidBrush<uint32_t,int32_t>,
				Settings );
		}

		void Solid::PaintPolygonRasters( uint32_t *destinationImage, intptr_t rowStrafeInBytes, Point<int32_t> *pointsArray, size_t numPoints )
		{
			System::Raster::PaintPolygonPointsArray(
				destinationImage, rowStrafeInBytes, pointsArray, numPoints,
				System::Raster::PaintRasterInSolidBrush<uint32_t,int32_t>,
				Settings );
		}

		void Solid::ToMetafileText( libBasic::AbstractTextOutputStream *logStream )
		{
			libBasic::MetaOut::MetaFileWriter metaWriter( _outputTextStream );
			metaWriter.Start( META_BRUSH_SOLID );
			metaWriter.Add( this->Settings.Colour );
			metaWriter.Done();
		}

		// Patterned brush:

		void Patterned::PaintRasterRectangle( uint32_t *destinationImage, intptr_t rowStrafeInBytes, int32_t left, int32_t top, int32_t widthPixels, int32_t heightPixels )
		{
			System::Raster::PaintFilledRectangle(
				destinationImage, rowStrafeInBytes, left, top, widthPixels, heightPixels,
				System::Raster::PaintRasterInPatternBrush<uint32_t,int32_t>,
				Settings );
		}

		void Patterned::PaintRasterLR( uint32_t *destinationImage, intptr_t rowStrafeInBytes, int32_t viewportTop, System::Raster::RasterLR<int32_t> *lrArray, int32_t heightPixels )
		{
			System::Raster::PaintLRArray(
				destinationImage, rowStrafeInBytes, viewportTop, lrArray, heightPixels,
				System::Raster::PaintRasterInPatternBrush<uint32_t,int32_t>,
				Settings );
		}

		void Patterned::PaintPolygonRasters( uint32_t *destinationImage, intptr_t rowStrafeInBytes, Point<int32_t> *pointsArray, size_t numPoints )
		{
			System::Raster::PaintPolygonPointsArray(
				destinationImage, rowStrafeInBytes, pointsArray, numPoints,
				System::Raster::PaintRasterInPatternBrush<uint32_t,int32_t>,
				Settings );
		}

		void Patterned::ToMetafileText( libBasic::AbstractTextOutputStream *logStream )
		{
			libBasic::MetaOut::MetaFileWriter metaWriter( logStream );
			metaWriter.Start( META_IN META_BRUSH_PATTERNED );
			metaWriter.Add( this->Settings.ForeColour, this->Settings.BackColour );
			metaWriter.Add( this->Settings.ForeTransparent ? 1 : 0, this->Settings.BackTransparent ? 1 : 0 );
			metaWriter.Add( this->Settings.PatternOffsetX, this->Settings.PatternOffsetY );
			metaWriter.Done();
			metaWriter.StartBinary();
			metaWriter.AddBinary( this->Settings.Pattern, this->Settings.Pattern+16 );
			metaWriter.FlushBinary();
			metaWriter.Start( META_OUT META_BRUSH_PATTERNED );
			metaWriter.Done();
		}

		// AverageMixed brush:

		#define AMIX_TYPES_TUPLE     uint32_t(0xFEFEFEFE),uint32_t(1)   // TODO: Defined in more than once place!

		void AverageMixed::PaintRasterRectangle( uint32_t *destinationImage, intptr_t rowStrafeInBytes, int32_t left, int32_t top, int32_t widthPixels, int32_t heightPixels )
		{
			System::Raster::PaintFilledRectangle(
				destinationImage, rowStrafeInBytes, left, top, widthPixels, heightPixels,
				System::Raster::PaintRasterInAverageMixBrush<uint32_t,int32_t,AMIX_TYPES_TUPLE>,
				Settings );
		}

		void AverageMixed::PaintRasterLR( uint32_t *destinationImage, intptr_t rowStrafeInBytes, int32_t viewportTop, System::Raster::RasterLR<int32_t> *lrArray, int32_t heightPixels )
		{
			System::Raster::PaintLRArray(
				destinationImage, rowStrafeInBytes, viewportTop, lrArray, heightPixels,
				System::Raster::PaintRasterInAverageMixBrush<uint32_t,int32_t,AMIX_TYPES_TUPLE>,
				Settings );
		}

		void AverageMixed::PaintPolygonRasters( uint32_t *destinationImage, intptr_t rowStrafeInBytes, Point<int32_t> *pointsArray, size_t numPoints )
		{
			System::Raster::PaintPolygonPointsArray(
				destinationImage, rowStrafeInBytes, pointsArray, numPoints,
				System::Raster::PaintRasterInAverageMixBrush<uint32_t,int32_t,AMIX_TYPES_TUPLE>,
				Settings );
		}

		void AverageMixed::ToMetafileText( libBasic::AbstractTextOutputStream *logStream )
		{
			libBasic::MetaOut::MetaFileWriter metaWriter( logStream );
			metaWriter.Start( META_BRUSH_AVERAGE_MIX );
			metaWriter.Add( this->Settings.Colour );
			metaWriter.Done();
		}

		#undef AMIX_TYPES_TUPLE

		// AndXor brush:

		void AndXor::PaintRasterRectangle( uint32_t *destinationImage, intptr_t rowStrafeInBytes, int32_t left, int32_t top, int32_t widthPixels, int32_t heightPixels )
		{
			System::Raster::PaintFilledRectangle(
				destinationImage, rowStrafeInBytes, left, top, widthPixels, heightPixels,
				System::Raster::PaintRasterInAndXorBrush<uint32_t,int32_t>,
				Settings );
		}

		void AndXor::PaintRasterLR( uint32_t *destinationImage, intptr_t rowStrafeInBytes, int32_t viewportTop, System::Raster::RasterLR<int32_t> *lrArray, int32_t heightPixels )
		{
			System::Raster::PaintLRArray(
				destinationImage, rowStrafeInBytes, viewportTop, lrArray, heightPixels,
				System::Raster::PaintRasterInAndXorBrush<uint32_t,int32_t>,
				Settings );
		}

		void AndXor::PaintPolygonRasters( uint32_t *destinationImage, intptr_t rowStrafeInBytes, Point<int32_t> *pointsArray, size_t numPoints )
		{
			System::Raster::PaintPolygonPointsArray(
				destinationImage, rowStrafeInBytes, pointsArray, numPoints,
				System::Raster::PaintRasterInAndXorBrush<uint32_t,int32_t>,
				Settings );
		}

		void AndXor::ToMetafileText( libBasic::AbstractTextOutputStream *logStream )
		{
			libBasic::MetaOut::MetaFileWriter metaWriter( logStream );
			metaWriter.Start( META_BRUSH_ANDXOR );
			metaWriter.Add( this->Settings.AndMask );
			metaWriter.Add( this->Settings.XorMask );
			metaWriter.Done();
		}

	} /// end namespace

} /// end namespace





























namespace libGraphics
{
	namespace Devices
	{
		// The base activities that all devices do.

		BaseDevice::BaseDevice()
			: _foreColour( 0xFFffFF )
			, _backColour( 0x000000 )
			, _polygonModeCounter( 0 ) /// not in a polygon
		{
		}

		BaseDevice::~BaseDevice()
		{
			ReleaseCurrentFont();
		}

		void BaseDevice::SetForegroundColour( uint32_t colour )
		{
			_foreColour = colour;
		}

		void BaseDevice::SetBackgroundColour( uint32_t colour )
		{
			_backColour = colour;
		}

		void BaseDevice::SetViewport( Rect<int32_t> r )
		{
			_viewport = r;
		}

		void BaseDevice::SelectPen( std::shared_ptr<Pens::AbstractPen> p )
		{
			_pPen = p;
		}

		void BaseDevice::SelectBrush( std::shared_ptr<Brushes::AbstractBrush> b )
		{
			_pBrush = b;
		}

		void BaseDevice::SelectBitmap( std::shared_ptr<Bitmaps::AbstractBitmap> p )
		{
			_pBitmap = p;
		}

		void BaseDevice::SelectFont( const char *fontName, uint32_t pointSizeTenths )
		{
			assert( _pFontServer != 0 ); /// Tell the programmer if there is no font server attached to the DC.
			if( _pFontServer )
			{
				ReleaseCurrentFont();
				_pFont = _pFontServer->AddRefFont( fontName, pointSizeTenths );
			}
		}

		void BaseDevice::ReleaseCurrentFont() /// private
		{
			if( _pFontServer && _pFont )
			{
				_pFontServer->ReleaseFont( _pFont );
				_pFont = nullptr;
			}
		}

		void BaseDevice::Text( int32_t x, int32_t y, Scaling *pScaling, const char *text, size_t charCount )
		{
			if( FontAvailable() )
			{
				Fonts::PaintByBitmaps( *this, *_pFont, x, y, pScaling, text, charCount );
			}
		}

		Rect<int32_t> BaseDevice::GetViewport()
		{
			return _viewport;
		}

		void BaseDevice::StartPoly()
		{
			++_polygonModeCounter;
		}

		void BaseDevice::EndPoly()
		{
			if( _polygonModeCounter > 0 )
			{
				--_polygonModeCounter;
			}
			else
			{
				assert(false); // badly nested calls to StartPoly() / EndPoly()
			}
		}

		std::shared_ptr<Fonts::AbstractFont> BaseDevice::GetFont() const
		{
			return _pFont;
		}

	} /// end namespace

} /// end namespace










namespace libGraphics
{
	namespace Devices
	{
		BitmapDevice::BitmapDevice( Bitmaps::Colour &target )
			: _bitmap(target)
			, _pLRArray(nullptr)
			, _pPointsArray(nullptr)
			, _pointsArrayCapacity(0)
			, _cursorX(0)
			, _cursorY(0)
		{
			SetViewport( GetExtentsRect() );
			_outlinerLineRecv.SetTargetBitmap( target );
			_thickOutlinerLineRecv.SetTargetBitmap( target );
		}

		void BitmapDevice::SetLRArray( System::Raster::RasterLR<int32_t> *pArray, size_t capacity )
		{
			if( pArray )
			{
				assert( capacity >= size_t( _bitmap.HeightPixels ) ); // should match
			}
			_pLRArray = pArray;
		}

		void BitmapDevice::SetPointsArray( Point<int32_t> *pArray, size_t capacity )
		{
			assert( ! DoingPolygon() ); /// changing this half way through is not supported.
			if( pArray )
			{
				_pPointsArray = pArray;
				_pointsArrayCapacity = capacity;
			}
			else
			{
				_pPointsArray = 0;
				_pointsArrayCapacity = 0;
			}
			if( DoingPolygon() )
			{
				/// Do this because we really have to do something.
				/// We certainly can't continue with the previous buffer.
				_scanCvtLineRecv.Reset( _pPointsArray, _pointsArrayCapacity );
			}
		}

		void BitmapDevice::SetViewport( Rect<int32_t> r )
		{
			// The viewport cannot be changed while polygon points are being added.
			if( ! DoingPolygon() )
			{
				// Need a special routine for this because we must never record
				// a viewport outside of the physical target bitmap.
				if( ! InsideOut(r) )
				{
					r = Intersection( r, GetExtentsRect() );
					if( ! InsideOut(r) )
					{
						_viewport = r;
						goto done;
					}
				}
				_viewport = Rect<int32_t>(0,0,0,0); // Set to "no operation" mode.
			done:
				_outlinerLineRecv.SetUncheckedViewport( _viewport );
				_thickOutlinerLineRecv.SetUncheckedViewport( _viewport );
				_scanCvtLineRecv.SetViewport(  _viewport );
			}
		}

		void BitmapDevice::SelectPen( std::shared_ptr<Pens::AbstractPen> pPen )
		{
			BaseDevice::SelectPen( pPen ); /// call the base class
			if( pPen != nullptr )
			{
				// TODO: Sort out these kludges.  Fetches are benign if its the wrong pen type, as it stands.
				_outlinerLineRecv.SetColour( pPen->KludgeGetColour() );
				_thickOutlinerLineRecv.SetBrushAndThickness( pPen->KludgeGetBrush(), pPen->KludgeGetThickness() );
			}
		}

		void BitmapDevice::StartPoly()
		{
			if( ! DoingPolygon() ) /// only reset on the outermost one
			{
				_scanCvtLineRecv.Reset( _pPointsArray, _pointsArrayCapacity );
			}

			BaseDevice::StartPoly(); /// call the base class
		}

		void BitmapDevice::EndPoly()
		{
			BaseDevice::EndPoly(); /// call the base class

			if( ! DoingPolygon() ) /// only paint the fill & reset on the outermost one:
			{
				if( BrushAvailable() && _pPointsArray )
				{
					// Sort the points.
					// This also checks we didn't overflow the points array (and thus truncate points),
					// as ActualSize will be zero in this case.
					size_t actualSize = _scanCvtLineRecv.SortAndGetSize();
					if( actualSize != 0 )
					{
						// Paint the filled polygon:
						PaintPointsArrayToBitmap( _pPointsArray, actualSize );
					}
				}
				_scanCvtLineRecv.Reset( 0, 0 ); /// For tidyness I shall do this.
			}
		}

		void BitmapDevice::MoveTo( int32_t x,int32_t y )
		{
			_cursorX = x;
			_cursorY = y;
		}

		void BitmapDevice::LineTo( int32_t x,int32_t y )
		{
			GetLineReceiverForCurrentMode()->operator()( _cursorX, _cursorY, x, y );
			_cursorX = x;
			_cursorY = y;
		}

		System::LineReceivers::AbstractLineReceiver *BitmapDevice::GetLineReceiverForCurrentMode()
		{
			if( DoingPolygon() ) 
				return static_cast<System::LineReceivers::AbstractLineReceiver *>( &_scanCvtLineRecv );
			
			if( _pPen->IsSolid() )
				return static_cast<System::LineReceivers::AbstractLineReceiver *>( &_outlinerLineRecv );
			
			if( _pPen->IsThick() )
				return static_cast<System::LineReceivers::AbstractLineReceiver *>( &_thickOutlinerLineRecv );
			
			return nullptr;
		}

		void BitmapDevice::Arc( Rect<int32_t> r, int32_t startAngle, int32_t endAngle )
		{
			System::ToLines::Arc<int32_t>( _viewport, r.left, r.top, r.right, r.bottom, startAngle, endAngle, 0, *GetLineReceiverForCurrentMode() );
		}

		void BitmapDevice::Secant( Rect<int32_t> r, int32_t startAngle, int32_t endAngle )
		{
			System::ToLines::Arc<int32_t>( _viewport, r.left, r.top, r.right, r.bottom, startAngle, endAngle, 4, *GetLineReceiverForCurrentMode() );
		}

		void BitmapDevice::Pie( Rect<int32_t> r, int32_t startAngle, int32_t endAngle )
		{
			System::ToLines::Arc<int32_t>( _viewport, r.left, r.top, r.right, r.bottom, startAngle, endAngle, 3, *GetLineReceiverForCurrentMode() );
		}

		void BitmapDevice::Ellipse( Rect<int32_t> r )
		{
			System::ToLines::Arc<int32_t>( _viewport, r.left, r.top, r.right, r.bottom, 0,360,  0, *GetLineReceiverForCurrentMode() );
		}

		void BitmapDevice::Rectangle( Rect<int32_t> r )
		{
			System::ToLines::Rectangle<int32_t>( _viewport, r.left, r.top, r.right, r.bottom, *GetLineReceiverForCurrentMode() );
		}

		void BitmapDevice::Bezier( int32_t x0,int32_t y0,int32_t x1,int32_t y1,int32_t x2,int32_t y2,int32_t x3,int32_t y3 )
		{
			System::ToLines::Bezier( _viewport,  x0,y0, x1,y1, x2,y2, x3,y3, *GetLineReceiverForCurrentMode() );
		}

		void BitmapDevice::Triangle( int32_t x0,int32_t y0,int32_t x1,int32_t y1,int32_t x2,int32_t y2 )
		{
			System::ToLines::Triangle( _viewport, x0,y0, x1,y1, x2,y2, *GetLineReceiverForCurrentMode() );
		}

		void BitmapDevice::PaintLRArrayToBitmap( Rect<int32_t> viewport )
		{
			assert( Union(viewport, _viewport) == _viewport ); // the passed-in Viewport should always be a sub-area of m_Viewport
			_pBrush->PaintRasterLR(
					_bitmap.TopLeft,
					_bitmap.BytesPerScanLine,
					viewport.top,
					_pLRArray + viewport.top,
					Height(viewport) );
		}

		void BitmapDevice::PaintPointsArrayToBitmap( Point<int32_t> *pPts, size_t numPoints )
		{
			_pBrush->PaintPolygonRasters(
					_bitmap.TopLeft,
					_bitmap.BytesPerScanLine,
					pPts,
					numPoints );

			#ifdef SHOW_POLYGON_SPOTS
			// DEBUGGING : Show "spots" at the raster edges
			PointPlotter<uint32_t,int32_t> pp( m_Bitmap.TopLeft, m_Bitmap.BytesPerScanLine, m_Viewport, m_pPen->KludgeGetColour() );
			template_PlotArrayOfPoints( pPts, pPts+NumPoints, pp );
			#endif
		}

		void BitmapDevice::DrawBitmap( Rect<int32_t> areaOnTarget, Rect<int32_t> areaOnSource, uint32_t flags )
		{
			if( BitmapAvailable() )
			{
				if( flags & 1 )
				{
					_pBitmap->DrawTransparentTo32bpp( _bitmap, _viewport, areaOnTarget, areaOnSource, _foreColour );
				}
				else
				{
					_pBitmap->DrawOpaqueTo32bpp( _bitmap, _viewport, areaOnTarget, areaOnSource, _foreColour, _backColour );
				}
			}
		}

		void BitmapDevice::DirectRectangle( int32_t x0,int32_t y0,int32_t x1,int32_t y1 )
		{
			// "Direct" API:  Special case for bitmaps only.
			if( BrushAvailable() )
			{
				auto r = Rectify( Rect<int32_t>(x0,y0,x1,y1) );
				r = Intersection( r, _viewport );
				if( ! InsideOut(r) )
				{
					_pBrush->PaintRasterRectangle( _bitmap.TopLeft, _bitmap.BytesPerScanLine, r.left, r.top, Width(r), Height(r) );
				}
			}
		}

		void BitmapDevice::DirectEllipse( int32_t x0, int32_t y0, int32_t x1, int32_t y1 )
		{
			// "Direct" API:  Special case for bitmaps only.
			if( BrushAvailable()  &&  _pLRArray != nullptr )
			{
				// ** FIX: optimise the range done.  Reminder: params may not be rectified.
				InitArray( _pLRArray, _bitmap.HeightPixels, _bitmap.WidthPixels );
				System::Raster::LRCollector<int32_t>  rasterRecv( _pLRArray, _bitmap.HeightPixels, _viewport );
				System::ToRasters::BresenhamFilledEllipse( x0, y0, x1, y1, rasterRecv );
				PaintLRArrayToBitmap( _viewport );
			}
		}

		void BitmapDevice::DirectTriangle( int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2 )
		{
			// "Direct" API:  Special case for bitmaps only.
			if( BrushAvailable()  &&  _pLRArray != nullptr )
			{
				// ** FIX: optimise the range done.  Reminder: params may not be rectified.
				InitArray( _pLRArray, _bitmap.HeightPixels, _bitmap.WidthPixels );
				System::Raster::LRCollector<int32_t>  pointRecv( _pLRArray, _bitmap.HeightPixels, _viewport );
				System::LineReceivers::LineDrawNoRejectCheck<int32_t, System::Raster::LRCollector<int32_t> >  lineRecv( pointRecv );
				System::ToLines::Triangle( _viewport, x0, y0, x1, y1, x2, y2, lineRecv );
				PaintLRArrayToBitmap( _viewport );
			}
		}

	} /// end namespace

} /// end namespace














namespace libGraphics
{
	namespace Devices
	{
		ExtentsMeasurementDevice::ExtentsMeasurementDevice()
			: _firstTime(true)
		{
			_viewport = Rect<int32_t>( 0x80000000, 0x80000000, 0x7FFFffff, 0x7FFFffff ); // ** FIX: Maximum constants.
		}

		bool ExtentsMeasurementDevice::GetMeasuredExtents( Rect<int32_t> *out_extents ) const
		{
			*out_extents = _measuredExtents;  // always return.
			return ! _firstTime; // because no drawing at all was done since construction.
		}

		void ExtentsMeasurementDevice::ContributeRect( Rect<int32_t> r )
		{
			// Contribute rectangle 'r' to the extents so far.
			auto inter = Intersection( _viewport, r );
			if( _firstTime )
			{
				// This is the first rectangle since construction.
				_measuredExtents = inter;
				_firstTime = false;
			}
			else
			{
				// All subsequent rectangles *may* stretch the current m_MeasuredExtents:
				_measuredExtents = Union( inter, _measuredExtents );
			}
		}

		void ExtentsMeasurementDevice::ContributePoint( int32_t x, int32_t y )
		{
			ContributeRect( Rect<int32_t>(x,y,x,y) );
		}

		void ExtentsMeasurementDevice::MoveTo( int32_t x,int32_t y )
		{
			ContributePoint(x,y);
		}

		void ExtentsMeasurementDevice::LineTo( int32_t x,int32_t y )
		{
			ContributePoint(x,y);
		}

		void ExtentsMeasurementDevice::Arc( Rect<int32_t> r, int32_t startAngle, int32_t endAngle )
		{
			ContributeRect(r);
		}

		void ExtentsMeasurementDevice::Secant( Rect<int32_t> r, int32_t startAngle, int32_t endAngle )
		{
			ContributeRect(r);
		}

		void ExtentsMeasurementDevice::Pie( Rect<int32_t> r, int32_t startAngle, int32_t endAngle )
		{
			ContributeRect(r);
		}

		void ExtentsMeasurementDevice::Ellipse( Rect<int32_t> r )
		{
			ContributeRect(r);
		}

		void ExtentsMeasurementDevice::Rectangle( Rect<int32_t> r )
		{
			ContributeRect(r);
		}

		void ExtentsMeasurementDevice::Bezier( int32_t x0,int32_t y0,int32_t x1,int32_t y1,int32_t x2,int32_t y2,int32_t x3,int32_t y3 )
		{
			ContributePoint(x0,y0);
			ContributePoint(x1,y1);
			ContributePoint(x2,y2);
			ContributePoint(x3,y3);
		}

		void ExtentsMeasurementDevice::Triangle( int32_t x0,int32_t y0,int32_t x1,int32_t y1,int32_t x2,int32_t y2 )
		{
			ContributePoint(x0,y0);
			ContributePoint(x1,y1);
			ContributePoint(x2,y2);
		}

		void ExtentsMeasurementDevice::DrawBitmap( Rect<int32_t> areaOnTarget, Rect<int32_t> areaOnSource, uint32_t flags )
		{
			if( _pBitmap )
			{
				// areaOnSource; flags; // not used
				ContributeRect( areaOnTarget );
			}
		}

	} /// end namespace

} /// end namespace








namespace libGraphics
{
	namespace Bitmaps
	{
		namespace Internal
		{
			void BitmapBitsToMetafile(
				const char *openTagName,
				const char *closeTagName,
				const uint8_t *data,
				int32_t   pixelDelta,
				int32_t   visibleBytesPerPixel,
				int32_t   count,
				intptr_t  bytesPerScanLine,
				int32_t   widthPixels,
				int32_t   heightPixels,
				libBasic::AbstractTextOutputStream *outputTextStream )
			{
				// Start tag
				libBasic::MetaOut::MetaFileWriter metaWriter( outputTextStream );
				metaWriter.Start( openTagName );
				libBasic::MetaOut::Add(   tmpstr, widthPixels, heightPixels );
				libBasic::MetaOut::Done(  tmpstr, outputTextStream );

				// Data
				auto pSrc = data;
				uint32_t h = heightPixels;
				metaWriter.StartBinary( );
				while( h > 0 )
				{
					uint32_t c = count;
					while( c > 0 )
					{
						metaWriter.AddBinary( pSrc, pSrc + visibleBytesPerPixel );
						pSrc += pixelDelta;
						--c;
					}
					pSrc += bytesPerScanLine;
					--h;
				}
				metaWriter.FlushBinary( outputTextStream );

				// End tag
				metaWriter.Start( closeTagName );
				metaWriter.Done();
			}
		}

        Colour::Colour()
        {
			TopLeft = nullptr;
			WidthPixels = 0;
			HeightPixels = 0;
			BytesPerScanLine = 0;
        }

        Colour::Colour( uint32_t *topLeft, int32_t widthPixels, int32_t heightPixels, intptr_t bytesPerScanLine )
        {
			TopLeft = topLeft;
			WidthPixels = widthPixels;
			HeightPixels = heightPixels;
			BytesPerScanLine = bytesPerScanLine;
        }

		void Colour::ToMetafileText( libBasic::AbstractTextOutputStream *logStream )
		{
			// Serialise bitmap definition (including bits) to metafile text
			Internal::BitmapBitsToMetafile(
				META_IN META_BITMAP_BITS_COLOUR,
				META_OUT META_BITMAP_BITS_COLOUR,
				reinterpret_cast<const uint8_t *>( this->TopLeft ),
				4,3,
				this->WidthPixels, /// count
				this->BytesPerScanLine,
				this->WidthPixels,
				this->HeightPixels,
				logStream );
		}

		void Colour::DrawOpaqueTo32bpp( System::template_Bitmap<int32_t,uint32_t> &TargetBM, Rect<int32_t> Viewport, Rect<int32_t> AreaOnTarget, Rect<int32_t> AreaOnSource, uint32_t ForeColour, uint32_t BackColour )
		{
			// ForeColour;  /// not used
			// BackColour;  /// not used
			Bitmaps::ScaleColourOpaque( TargetBM, AreaOnTarget, *this, AreaOnSource, Viewport );
		}

		void Colour::DrawTransparentTo32bpp( System::template_Bitmap<int32_t,uint32_t> &TargetBM, Rect<int32_t> Viewport, Rect<int32_t> AreaOnTarget, Rect<int32_t> AreaOnSource, uint32_t ForeColour )
		{
			Bitmaps::ScaleColourTransparent( TargetBM, AreaOnTarget, *this, AreaOnSource, Viewport, ForeColour );
		}

		Rect<int32_t> Colour::GetExtents()
		{
			return Rect<int32_t>(0,0,WidthPixels,HeightPixels);
		}

        Mono::Mono()
        {
			TopLeft = (nullptr);
			WidthPixels = (0);
			HeightPixels = (0);
			BytesPerScanLine = (0);
        }

        Mono::Mono( uint8_t *topLeft, int32_t widthPixels, int32_t heightPixels, intptr_t bytesPerScanLine )
        {
			TopLeft = ( topLeft );
			WidthPixels = ( widthPixels );
			HeightPixels = ( heightPixels );
			BytesPerScanLine = ( bytesPerScanLine );
        }

		void Mono::ToMetafileText( libBasic::AbstractTextOutputStream *logStream )
		{
			// Serialise bitmap definition (including bits) to metafile text
			int32_t Count = MonoWidthPixelsToBytes(this->WidthPixels);
			Internal::BitmapBitsToMetafile(
				META_IN META_BITMAP_BITS_MONO,
				META_OUT META_BITMAP_BITS_MONO,
				reinterpret_cast<const uint8_t *>( this->TopLeft ),
				1,1,
				Count,
				this->BytesPerScanLine,
				this->WidthPixels,
				this->HeightPixels,
				logStream );
		}

		void Mono::DrawOpaqueTo32bpp( System::template_Bitmap<int32_t,uint32_t> &TargetBM, Rect<int32_t> Viewport, Rect<int32_t> AreaOnTarget, Rect<int32_t> AreaOnSource, uint32_t ForeColour, uint32_t BackColour )
		{
			Bitmaps::ScaleMonoOpaque( TargetBM, AreaOnTarget, *this, AreaOnSource, Viewport, ForeColour, BackColour );
		}

		void Mono::DrawTransparentTo32bpp( System::template_Bitmap<int32_t,uint32_t> &TargetBM, Rect<int32_t> Viewport, Rect<int32_t> AreaOnTarget, Rect<int32_t> AreaOnSource, uint32_t ForeColour )
		{
			Bitmaps::ScaleMonoTransparent( TargetBM, AreaOnTarget, *this, AreaOnSource, Viewport, ForeColour );
		}

		Rect<int32_t> Mono::GetExtents()
		{
			return Rect<int32_t>(0,0,WidthPixels,HeightPixels);
		}

	} /// end namespace

} /// end namespace












namespace libGraphics
{
	namespace Devices
	{
		MetafileRecorderDevice::MetafileRecorderDevice( libBasic::AbstractTextOutputStream *logStream )
			: _outputTextStream( logStream )
		{
			assert( logStream != nullptr );
		}

		void MetafileRecorderDevice::SetForegroundColour( uint32_t foreColour )
		{
			BaseDevice::SetForegroundColour( foreColour ); /// call the base
			libBasic::MetaOut::MetaFileWriter metaWriter( _outputTextStream );
			metaWriter.Start( META_COLOUR_FOREGROUND );
			metaWriter.Add( foreColour );
			metaWriter.Done();
		}

		void MetafileRecorderDevice::SetBackgroundColour( uint32_t backColour )
		{
			BaseDevice::SetForegroundColour( backColour ); /// call the base
			libBasic::MetaOut::MetaFileWriter metaWriter( _outputTextStream );
			metaWriter.Start( META_COLOUR_BACKGROUND );
			metaWriter.Add( backColour );
			metaWriter.Done();
		}

		void MetafileRecorderDevice::SetViewport( Rect<int32_t> r )
		{
			BaseDevice::SetViewport(r); /// call the base
			libBasic::MetaOut::MetaFileWriter metaWriter( _outputTextStream );
			metaWriter.Start( META_VIEWPORT );
			metaWriter.Add( r.left, r.top );
			metaWriter.Add( r.right, r.bottom );
			metaWriter.Done();
		}

		void MetafileRecorderDevice::SelectPen( std::shared_ptr<Pens::AbstractPen> p )
		{
			BaseDevice::SelectPen(p); /// call the base
			if( p != nullptr )
			{
				p->ToMetafileText( _outputTextStream );
			}
		}

		void MetafileRecorderDevice::SelectBrush( std::shared_ptr<Brushes::AbstractBrush> b )
		{
			BaseDevice::SelectBrush(b); /// call the base
			if( b != nullptr )
			{
				b->ToMetafileText( _outputTextStream );
			}
		}

		void MetafileRecorderDevice::SelectBitmap( std::shared_ptr<Bitmaps::AbstractBitmap> p )
		{
			BaseDevice::SelectBitmap(p); /// call the base
			if( p != nullptr )
			{
				p->ToMetafileText( _outputTextStream );
			}
		}

		void MetafileRecorderDevice::SelectFont( const char *fontName, uint32_t pointSizeTenths )
		{
			/// We don't need to call the base here:  BaseDevice::SelectFont( FontName, PointSizeTenths ); /// call the base
			/// Reminder: The metafile recorder doesn't need a "font server" to be able to record the font selection.
			libBasic::MetaOut::MetaFileWriter metaWriter( _outputTextStream );
			metaWriter.Start( META_IN META_FONT );
			metaWriter.Add( pointSizeTenths );
			metaWriter.Done();

			auto t = fontName;
			auto len = strlen(t);
			metaWriter.StartBinary( );
			metaWriter.AddBinary( t, t+len );
			metaWriter.FlushBinary();

			metaWriter.Start( META_OUT META_FONT );
			metaWriter.Done();
		}

		void MetafileRecorderDevice::Text( int32_t x, int32_t y, Scaling *pScaling, const char *text, size_t charCount )
		{
			/// We do NOT call the base here (don't want bitmaps!)
			/// Reminder: The metafile recorder doesn't need a "font server" to be able to record the text.
			if( text != 0 )
			{
				SmallStringBuilder  tmpstr;

				metaWriter.Start( META_IN META_TEXT );
				metaWriter.Add( x, y );
				if( pScaling )
				{
					metaWriter.Add( pScaling->MultiplierX, pScaling->DivisorX );
					metaWriter.Add( pScaling->MultiplierY, pScaling->DivisorY );
				}
				else // no scaling
				{
					metaWriter.Add( 1, 1 );
					metaWriter.Add( 1, 1 );
				}
				metaWriter.Add( charCount );
				metaWriter.Done();

				metaWriter.StartBinary();
				metaWriter.AddBinary( text, text + charCount );
				metaWriter.FlushBinary();

				metaWriter.Start( META_OUT META_TEXT );
				metaWriter.Done();
			}
		}

		void MetafileRecorderDevice::StartPoly()
		{
			if( ! DoingPolygon() ) /// space save: only output the outermost one
			{
				// TODO: Quite a lot of effort goes into just appending a string literal.  Review all places with this problem.
				libBasic::MetaOut::MetaFileWriter metaWriter( _outputTextStream );
				metaWriter.Start( META_IN META_POLY );
				metaWriter.Done();
			}

			BaseDevice::StartPoly(); /// call base class
		}

		void MetafileRecorderDevice::EndPoly()
		{
			BaseDevice::EndPoly(); /// call base class

			if( ! DoingPolygon() ) /// space save: only output the outermost one
			{
				libBasic::MetaOut::MetaFileWriter metaWriter( _outputTextStream );
				metaWriter.Start( META_OUT META_POLY );
				metaWriter.Done();
			}
		}

		void MetafileRecorderDevice::MoveTo( int32_t x,int32_t y )
		{
			libBasic::MetaOut::MetaFileWriter metaWriter( _outputTextStream );
			metaWriter.Start( META_MOVETO );
			metaWriter.Add( x, y );
			metaWriter.Done();
		}

		void MetafileRecorderDevice::LineTo( int32_t x,int32_t y )
		{
			libBasic::MetaOut::MetaFileWriter metaWriter( _outputTextStream );
			metaWriter.Start( META_LINETO );
			metaWriter.Add( x, y );
			metaWriter.Done();
		}

		void MetafileRecorderDevice::Arc( Rect<int32_t> r, int32_t startAngle, int32_t endAngle )
		{
			libBasic::MetaOut::MetaFileWriter metaWriter( _outputTextStream );
			metaWriter.Start( META_ARC );
			metaWriter.Add( r );
			metaWriter.Add( startAngle, endAngle );
			metaWriter.Done();
		}

		void MetafileRecorderDevice::Secant( Rect<int32_t> r, int32_t startAngle, int32_t endAngle )
		{
			libBasic::MetaOut::MetaFileWriter metaWriter( _outputTextStream );
			metaWriter.Start( META_SECANT );
			metaWriter.Add( r );
			metaWriter.Add( startAngle, endAngle );
			metaWriter.Done();
		}

		void MetafileRecorderDevice::Pie( Rect<int32_t> r, int32_t startAngle, int32_t endAngle )
		{
			libBasic::MetaOut::MetaFileWriter metaWriter( _outputTextStream );
			metaWriter.Start( META_PIE );
			metaWriter.Add( r );
			metaWriter.Add( startAngle, endAngle );
			metaWriter.Done();
		}

		void MetafileRecorderDevice::Ellipse( Rect<int32_t> r )
		{
			libBasic::MetaOut::MetaFileWriter metaWriter( _outputTextStream );
			metaWriter.Start( META_ELLIPSE );
			metaWriter.Add( r );
			metaWriter.Done();
		}

		void MetafileRecorderDevice::Rectangle( Rect<int32_t> r )
		{
			libBasic::MetaOut::MetaFileWriter metaWriter( _outputTextStream );
			metaWriter.Start( META_RECTANGLE );
			metaWriter.Add( r );
			metaWriter.Done();
		}

		void MetafileRecorderDevice::Bezier( int32_t x0,int32_t y0,int32_t x1,int32_t y1,int32_t x2,int32_t y2,int32_t x3,int32_t y3 )
		{
			libBasic::MetaOut::MetaFileWriter metaWriter( _outputTextStream );
			metaWriter.Start( META_BEZIER );
			metaWriter.Add( x0, y0 );
			metaWriter.Add( x1, y1 );
			metaWriter.Add( x2, y2 );
			metaWriter.Add( x3, y3 );
			metaWriter.Done();
		}

		void MetafileRecorderDevice::Triangle( int32_t x0,int32_t y0,int32_t x1,int32_t y1,int32_t x2,int32_t y2 )
		{
			libBasic::MetaOut::MetaFileWriter metaWriter( _outputTextStream );
			metaWriter.Start( META_TRIANGLE );
			metaWriter.Add( x0, y0 );
			metaWriter.Add( x1, y1 );
			metaWriter.Add( x2, y2 );
			metaWriter.Done();
		}



		namespace Internal
		{
			template<typename PIXEL>
			void template_RecordDrawBitmap(
				const char *openTagName,
				const char *closeTagName,
				const System::template_Bitmap<int32_t, PIXEL> &source,
				int32_t x, int32_t y,
				intptr_t visibleDataBytesPerRow,
				libBasic::AbstractTextOutputStream *outputStream )
			{
				libBasic::MetaOut::MetaFileWriter metaWriter( outputStream );

				// Start tag
				metaWriter.Start( openTagName );
				metaWriter.Add( x, y );
				metaWriter.Add( source.WidthPixels, source.HeightPixels );
				metaWriter.Add( source.BytesPerScanLine, visibleDataBytesPerRow );
				metaWriter.Done();

				// Data
				auto pSrc = reinterpret_cast<const uint8_t *>( source.TopLeft );
				uint32_t h = source.HeightPixels;
				metaWriter.StartBinary( );
				while( h > 0 )
				{
					metaWriter.AddBinary( pSrc, pSrc + visibleDataBytesPerRow );
					pSrc += source.BytesPerScanLine;
					--h;
				}
				metaWriter.FlushBinary( outputStream );

				// End tag
				metaWriter.Start( closeTagName );
				metaWriter.Done();
			}

		}  /// end namespace


		void MetafileRecorderDevice::DrawBitmap( Rect<int32_t> areaOnTarget, Rect<int32_t> areaOnSource, uint32_t flags )
		{
			if( BitmapAvailable() )
			{
				libBasic::MetaOut::MetaFileWriter metaWriter( _outputTextStream );
				metaWriter.Start( META_DRAW_BITMAP );
				metaWriter.Add( areaOnTarget );
				metaWriter.Add( areaOnSource );
				metaWriter.Add( flags );
				metaWriter.Done();
			}
		}

		void MetafileRecorderDevice::Done( SmallStringBuilder &tmpstr )
		{
			metaWriter.Done();
		}

	} /// end namespace

} /// end namespace







namespace libGraphics
{
	namespace Meta
	{
		namespace Internal
		{
			const char *g_EmptyMetaFile = ""; /// A default metafile (empty string)
		}

		Player::Player()
			: _pos( Internal::g_EmptyMetaFile )
			, _pTarget( nullptr )
			, _pBitmapBuffer( nullptr )
			, _bitmapBufferSizeBytes( 0 )
		{
		}

		void Player::SetSourceText( const char *sourceText )
		{
			_pos = sourceText ? sourceText : Internal::g_EmptyMetaFile;
		}

		void Player::SetTarget( Devices::AbstractDevice *target )
		{
			assert( target != nullptr );
			_pTarget = target;
		}

		void Player::SetMasterClippingRectangle( Rect<int32_t> r )
		{
			_masterViewport = r;
		}

		void Player::SetBitmapBuffer( void *bitmapBuffer, size_t bitmapSize )
		{
			_pBitmapBuffer         = bitmapBuffer;
			_bitmapBufferSizeBytes = bitmapSize;
		}

		bool Player::PlayAll()
		{
			for(;;)
			{
				/// Do all of the records we can:
				if( PlaySingleStep() ) continue;

				/// Expect end of string (null terminator):
				if( *_pos != 0 ) return false; /// unrecognised content.
				return true; /// played all OK
			}
		}

		bool Player::PlaySingleStep()
		{
			// Process *precisely one* command from the source text:

			assert( _pTarget != nullptr );

			// TODO: Objects to have a ParseFrom() function.
			// TODO: META_PEN_THICK
			
			if( libBasic::MetaIn::ParseMetaCmd( _pos, META_PEN_SOLID ) )
			{
				// Solid pen
				METAREAD_PARSE_FIELD( _solidPen->Colour );
				_pTarget->SelectPen( _solidPen );
			}
			else if( libBasic::MetaIn::ParseMetaCmd( _pos, META_BRUSH_SOLID ) )
			{
				// Solid brush
				METAREAD_PARSE_FIELD( _solidBrush->Settings.Colour );
				_pTarget->SelectBrush( _solidBrush );
			}
			else if( libBasic::MetaIn::ParseMetaCmd(_pos,META_IN META_BRUSH_PATTERNED) )
			{
				// Patterned brush
				METAREAD_PARSE_FIELD( _pattBrush->Settings.ForeColour );
				METAREAD_PARSE_FIELD( _pattBrush->Settings.BackColour );
				METAREAD_PARSE_FIELD( _pattBrush->Settings.ForeTransparent );
				METAREAD_PARSE_FIELD( _pattBrush->Settings.BackTransparent );
				METAREAD_PARSE_FIELD( _pattBrush->Settings.PatternOffsetX );
				METAREAD_PARSE_FIELD( _pattBrush->Settings.PatternOffsetY );
				METAREAD_PARSE_BINARY( &_pattBrushPattern[0], 32 );
				METAREAD_CMD( META_OUT META_BRUSH_PATTERNED );
				_pattBrush->Settings.Pattern = &_pattBrushPattern[0];
				_pTarget->SelectBrush( _pattBrush );
			}
			else if( libBasic::MetaIn::ParseMetaCmd( _pos, META_BRUSH_AVERAGE_MIX ) )
			{
				// Average-mix brush
				METAREAD_PARSE_FIELD( _avgMixBrush->Settings.Colour );
				_pTarget->SelectBrush( _avgMixBrush );
			}
			else if( libBasic::MetaIn::ParseMetaCmd( _pos, META_COLOUR_FOREGROUND ) )
			{
				// Set foreground colour
				uint32_t foreColour = 0;
				METAREAD_PARSE_FIELD( foreColour );
				_pTarget->SetForegroundColour( foreColour );
			}
			else if( libBasic::MetaIn::ParseMetaCmd( _pos, META_COLOUR_BACKGROUND ) )
			{
				// Set background colour
				uint32_t backColour = 0;
				METAREAD_PARSE_FIELD( backColour );
				_pTarget->SetBackgroundColour( backColour );
			}
			else if( libBasic::MetaIn::ParseMetaCmd( _pos, META_VIEWPORT ) )
			{
				// Viewport
				Rect<int32_t> r;
				METAREAD_PARSE_FIELD( r );
				// Set target's viewport to intersection with our master viewport:
				r = Intersection( r, _masterViewport );
				if( HasArea(r) )
				{
					_pTarget->SetViewport(r);
				}
				else
				{
					_pTarget->SetViewport( Rect<int32_t>(0,0,0,0) ); // No intersection, so disable.
				}
			}
			else if( libBasic::MetaIn::ParseMetaCmd( _pos, META_IN META_BITMAP_BITS_COLOUR ) )
			{
				// Select colour bitmap
				Bitmaps::Colour  tempBMP( nullptr, 0, 0, 0 );
				METAREAD_PARSE_FIELD( tempBMP.WidthPixels );
				METAREAD_PARSE_FIELD( tempBMP.HeightPixels );

				int32_t  widthPixels  = tempBMP.WidthPixels;
				int32_t  heightPixels = tempBMP.HeightPixels;

				if( widthPixels >= 0 && heightPixels >= 0 &&
					widthPixels <= METAFILE_PIXELS_MAX && heightPixels <= METAFILE_PIXELS_MAX )
                {
                    // Is OK.  Do nothing.
                }
				else
				{
					assert(false); /// invalid dimensions
					return false;
				}

				intptr_t  totalSourcePixels = widthPixels * heightPixels;
				intptr_t  bytesPerScanLine  = widthPixels * 4;
				size_t    allocationSize    = bytesPerScanLine * heightPixels;

				// ** FIX: We are only supporting the internal buffer for now (no heap)

				if( _pBitmapBuffer == 0 || allocationSize > _bitmapBufferSizeBytes )
				{
					assert(false);
					return false; /// out of memory ** FIX  Do we want to parse and skip the bytes?
				}

				tempBMP.TopLeft          = reinterpret_cast<uint32_t *>( _pBitmapBuffer );
				tempBMP.BytesPerScanLine = bytesPerScanLine;

				/// Parse bitmap image pixels (B,G,R triples):
				auto dest = reinterpret_cast<uint8_t *>( tempBMP.TopLeft );
				while( totalSourcePixels > 0 )
				{
					if( ! libBasic::MetaIn::ParseByte( _pos, *dest ) )  return false;  ++dest;
					if( ! libBasic::MetaIn::ParseByte( _pos, *dest ) )  return false;  ++dest;
					if( ! libBasic::MetaIn::ParseByte( _pos, *dest ) )  return false;  ++dest;
					*dest = 0; /// blank 4th byte (is not written to the metafile)
					++dest;
					--totalSourcePixels;
				}
				if( ! libBasic::MetaIn::ParseBinaryEndMarker( _pos ) ) return false; /// missing end marker

				/// Select bitmap:
				*_colourBitmap = tempBMP; /// only ever update member variable with a VALID bitmap
				_pTarget->SelectBitmap( _colourBitmap );

				/// End:
				METAREAD_CMD( META_OUT META_BITMAP_BITS_COLOUR );
			}
			else if( libBasic::MetaIn::ParseMetaCmd( _pos, META_IN META_BITMAP_BITS_MONO ) )
			{
				// Select mono bitmap
				Bitmaps::Mono   tempBMP;
				METAREAD_PARSE_FIELD( tempBMP.WidthPixels );
				METAREAD_PARSE_FIELD( tempBMP.HeightPixels );

				int32_t  widthPixels  = tempBMP.WidthPixels;
				int32_t  heightPixels = tempBMP.HeightPixels;

				if( widthPixels >= 0 && heightPixels >= 0 &&
					widthPixels <= METAFILE_PIXELS_MAX && heightPixels <= METAFILE_PIXELS_MAX )
                {
                    // Is OK.  Do nothing.
                }
				else
				{
					assert(false); /// invalid dimensions
					return false;
				}

				intptr_t  bytesPerScanLine  = MonoWidthPixelsToBytes( widthPixels );
				size_t    allocationSize    = bytesPerScanLine * heightPixels;

				// ** FIX: We are only supporting the internal buffer for now (no heap)

				if( _pBitmapBuffer == 0 || allocationSize > _bitmapBufferSizeBytes )
				{
					assert(false);
					return false; /// out of memory ** FIX  Do we want to parse and skip the bytes?
				}

				tempBMP.TopLeft          = reinterpret_cast<uint8_t *>( _pBitmapBuffer );
				tempBMP.BytesPerScanLine = bytesPerScanLine;

				/// Parse bitmap image:
				auto dest = reinterpret_cast<uint8_t *>( tempBMP.TopLeft );
				METAREAD_PARSE_BINARY( dest, allocationSize );

				/// Select:
				*_monoBitmap = tempBMP; /// only ever update member variable with a VALID bitmap
				_pTarget->SelectBitmap( _monoBitmap );

				/// End:
				METAREAD_CMD( META_OUT META_BITMAP_BITS_MONO );
			}
			else if( libBasic::MetaIn::ParseMetaCmd( _pos, META_DRAW_BITMAP ) )
			{
				// Draw bitmap
				Rect<int32_t>  areaOnTarget;
				Rect<int32_t>  areaOnSource;
				uint32_t  flags;
				METAREAD_PARSE_FIELD( areaOnTarget );
				METAREAD_PARSE_FIELD( areaOnSource );
				METAREAD_PARSE_FIELD( flags );
				_pTarget->DrawBitmap( areaOnTarget, areaOnSource, flags );
			}
			else if( libBasic::MetaIn::ParseMetaCmd( _pos, META_IN META_FONT ) )
			{
				int32_t  pointSizeTenths = 0;
				METAREAD_PARSE_FIELD( pointSizeTenths );
				SmallStringBuilder fontName;
				if( ! libBasic::MetaIn::ParseKernelString( _pos, &fontName ) ) return false;
				METAREAD_CMD( META_OUT META_FONT );
				_pTarget->SelectFont( fontName.c_str(), pointSizeTenths );
			}
			else if( libBasic::MetaIn::ParseMetaCmd( _pos, META_IN META_TEXT ) )
			{
				int32_t x;
				int32_t y;
				size_t count;
				Scaling sc;
				METAREAD_PARSE_FIELD( x );
				METAREAD_PARSE_FIELD( y );
				METAREAD_PARSE_FIELD( sc.MultiplierX );
				METAREAD_PARSE_FIELD( sc.DivisorX );
				METAREAD_PARSE_FIELD( sc.MultiplierY );
				METAREAD_PARSE_FIELD( sc.DivisorY );
				METAREAD_PARSE_FIELD( count );
				assert( count < 256 ); // ** FIX: Won't fit into the kernel string (change this implementation)
				SmallStringBuilder  text; // ** FIX: We do not want this ultimately
				if( ! libBasic::MetaIn::ParseKernelString( _pos, &text ) ) return false;
				METAREAD_CMD( META_OUT META_TEXT );
				if( sc.MultiplierX == 1 &&
					sc.MultiplierY == 1 &&
					sc.DivisorX == 1 &&
					sc.DivisorY == 1 )
				{
					// Speed opt for 1:1 scaling
					_pTarget->Text( x, y, nullptr, text.c_str(), count );
				}
				else
				{
					// Using scaling
					_pTarget->Text( x, y, &sc, text.c_str(), count );
				}
			}
			else if( libBasic::MetaIn::ParseMetaCmd( _pos, META_IN META_POLY ) )
			{
				_pTarget->StartPoly();
			}
			else if( libBasic::MetaIn::ParseMetaCmd( _pos, META_OUT META_POLY ) )
			{
				_pTarget->EndPoly();
			}
			else if( libBasic::MetaIn::ParseMetaCmd( _pos, META_MOVETO ) )
			{
				int32_t x,y;
				METAREAD_PARSE_FIELD(x);
				METAREAD_PARSE_FIELD(y);
				_pTarget->MoveTo(x,y);
			}
			else if( libBasic::MetaIn::ParseMetaCmd( _pos, META_LINETO ) )
			{
				int32_t x,y;
				METAREAD_PARSE_FIELD(x);
				METAREAD_PARSE_FIELD(y);
				_pTarget->LineTo(x,y);
			}
			else if( libBasic::MetaIn::ParseMetaCmd( _pos, META_ARC ) )
			{
				Rect<int32_t> r;
				int32_t startAngle, endAngle;
				METAREAD_PARSE_FIELD( r );
				METAREAD_PARSE_FIELD( startAngle );
				METAREAD_PARSE_FIELD( endAngle );
				_pTarget->Arc( r, startAngle, endAngle );
			}
			else if( libBasic::MetaIn::ParseMetaCmd( _pos, META_SECANT ) )
			{
				Rect<int32_t> r;
				int32_t startAngle, endAngle;
				METAREAD_PARSE_FIELD( r );
				METAREAD_PARSE_FIELD( startAngle );
				METAREAD_PARSE_FIELD( endAngle );
				_pTarget->Secant( r, startAngle, endAngle );
			}
			else if( libBasic::MetaIn::ParseMetaCmd( _pos, META_PIE ) )
			{
				Rect<int32_t> r;
				int32_t startAngle, endAngle;
				METAREAD_PARSE_FIELD( r );
				METAREAD_PARSE_FIELD( startAngle );
				METAREAD_PARSE_FIELD( endAngle );
				_pTarget->Pie( r, startAngle, endAngle );
			}
			else if( libBasic::MetaIn::ParseMetaCmd( _pos, META_ELLIPSE ) )
			{
				Rect<int32_t> r;
				METAREAD_PARSE_FIELD( r );
				_pTarget->Ellipse( r );
			}
			else if( libBasic::MetaIn::ParseMetaCmd( _pos, META_RECTANGLE ) )
			{
				Rect<int32_t> r;
				METAREAD_PARSE_FIELD( r );
				_pTarget->Rectangle( r );
			}
			else if( libBasic::MetaIn::ParseMetaCmd( _pos, META_BEZIER ) )
			{
				int32_t x0, y0, x1, y1, x2, y2, x3, y3;
				METAREAD_PARSE_FIELD( x0 );
				METAREAD_PARSE_FIELD( y0 );
				METAREAD_PARSE_FIELD( x1 );
				METAREAD_PARSE_FIELD( y1 );
				METAREAD_PARSE_FIELD( x2 );
				METAREAD_PARSE_FIELD( y2 );
				METAREAD_PARSE_FIELD( x3 );
				METAREAD_PARSE_FIELD( y3 );
				_pTarget->Bezier( x0,y0, x1,y1, x2,y2, x3,y3 );
			}
			else if( libBasic::MetaIn::ParseMetaCmd( _pos, META_TRIANGLE ) )
			{
				int32_t x0, y0, x1, y1, x2, y2;
				METAREAD_PARSE_FIELD( x0 );
				METAREAD_PARSE_FIELD( y0 );
				METAREAD_PARSE_FIELD( x1 );
				METAREAD_PARSE_FIELD( y1 );
				METAREAD_PARSE_FIELD( x2 );
				METAREAD_PARSE_FIELD( y2 );
				_pTarget->Triangle( x0,y0, x1,y1, x2,y2 );
			}
			else
			{
				return false; /// not recognised
			}

			return true;
		}

	} /// end namespace

} /// end namespace






namespace libGraphics
{
	namespace Devices
	{
		Rescaler::Rescaler()
			: _multiplierX(1)
			, _divisorX(1)
			, _multiplierY(1)
			, _divisorY(1)
			, _dx(0)
			, _dy(0)
			, _pTarget(nullptr)
		{
		}

		void Rescaler::SetTarget( AbstractDevice *pTarget )
		{
			assert( pTarget != this );
			if( pTarget != this ) // self reference not allowed
			{
				_pTarget = pTarget;
			}
		}

		void Rescaler::SetRescale( int32_t theMultiplier, int32_t theDivisor )
		{
			// Set both horizontal and vertical scaling to the same:
			assert( theDivisor != 0 );
			_multiplierX = theMultiplier;
			_multiplierY = theMultiplier;
			_divisorX    = theDivisor;
			_divisorY    = theDivisor;
		}

		void Rescaler::SetRescale( const Scaling &sc )
		{
			// Independent scaling of X and Y directions.
			assert( sc.DivisorX != 0 );
			assert( sc.DivisorY != 0 );
			_multiplierX = sc.MultiplierX;
			_multiplierY = sc.MultiplierY;
			_divisorX    = sc.DivisorX;
			_divisorY    = sc.DivisorY;
		}

		void Rescaler::SetTranslation( int32_t dx, int32_t dy )
		{
			_dx = dx;
			_dy = dy;
		}

		void Rescaler::SetViewport( Rect<int32_t> r )
		{
			_preScaledViewport = r;
			if(_pTarget)
			{
				_pTarget->SetViewport( ScaleRect(r) );
			}
		}

		void Rescaler::SelectPen( std::shared_ptr<Pens::AbstractPen> p )
		{
			if(_pTarget) _pTarget->SelectPen(p);
		}

		void Rescaler::SelectBrush( std::shared_ptr<Brushes::AbstractBrush> b )
		{
			if(_pTarget) _pTarget->SelectBrush(b);
		}

		void Rescaler::SelectBitmap( std::shared_ptr<Bitmaps::AbstractBitmap> b )
		{
			if(_pTarget) _pTarget->SelectBitmap(b);
		}

		void Rescaler::SelectFont( const char *FontName, uint32_t PointSizeTenths )
		{
			if(_pTarget) _pTarget->SelectFont( FontName, PointSizeTenths );
		}

		void Rescaler::SetForegroundColour( uint32_t ForeColour )
		{
			if(_pTarget) _pTarget->SetForegroundColour( ForeColour );
		}

		void Rescaler::SetBackgroundColour( uint32_t BackColour )
		{
			if(_pTarget) _pTarget->SetBackgroundColour( BackColour );
		}

		Rect<int32_t> Rescaler::GetViewport()
		{
			return _preScaledViewport;
		}

		void Rescaler::StartPoly()
		{
			if( _pTarget ) _pTarget->StartPoly();
		}

		void Rescaler::EndPoly()
		{
			if( _pTarget ) _pTarget->EndPoly();
		}

		void Rescaler::MoveTo( int32_t x,int32_t y )
		{
			if( _pTarget ) _pTarget->MoveTo( ScaleX(x), ScaleY(y) );
		}

		void Rescaler::LineTo( int32_t x,int32_t y )
		{
			if( _pTarget ) _pTarget->LineTo( ScaleX(x), ScaleY(y) );
		}

		void Rescaler::Arc( Rect<int32_t> r, int32_t StartAngle, int32_t EndAngle )
		{
			if( _pTarget ) _pTarget->Arc( ScaleRect(r), StartAngle, EndAngle );
		}

		void Rescaler::Secant( Rect<int32_t> r, int32_t StartAngle, int32_t EndAngle )
		{
			if( _pTarget ) _pTarget->Secant( ScaleRect(r), StartAngle, EndAngle );
		}

		void Rescaler::Pie( Rect<int32_t> r, int32_t StartAngle, int32_t EndAngle )
		{
			if( _pTarget ) _pTarget->Pie( ScaleRect(r), StartAngle, EndAngle );
		}

		void Rescaler::Ellipse( Rect<int32_t> r )
		{
			if( _pTarget ) _pTarget->Ellipse( ScaleRect(r) );
		}

		void Rescaler::Rectangle( Rect<int32_t> r )
		{
			if( _pTarget ) _pTarget->Rectangle( ScaleRect(r) );
		}

		void Rescaler::Bezier( int32_t x0,int32_t y0,int32_t x1,int32_t y1,int32_t x2,int32_t y2,int32_t x3,int32_t y3 )
		{
			if( _pTarget ) _pTarget->Bezier(
				ScaleX(x0), ScaleY(y0),
				ScaleX(x1), ScaleY(y1),
				ScaleX(x2), ScaleY(y2),
				ScaleX(x3), ScaleY(y3) );
		}

		void Rescaler::Triangle( int32_t x0,int32_t y0,int32_t x1,int32_t y1,int32_t x2,int32_t y2 )
		{
			if( _pTarget ) _pTarget->Triangle(
				ScaleX(x0), ScaleY(y0),
				ScaleX(x1), ScaleY(y1),
				ScaleX(x2), ScaleY(y2) );
		}

		void Rescaler::DrawBitmap( Rect<int32_t> AreaOnTarget, Rect<int32_t> AreaOnSource, uint32_t Flags )
		{
			if(_pTarget)
			{
				_pTarget->DrawBitmap( ScaleRect(AreaOnTarget), AreaOnSource, Flags );
			}
		}

		void Rescaler::Text( int32_t x, int32_t y, Scaling *pScaling, const char *Text, size_t Count )
		{
			if(_pTarget)
			{
				if( pScaling )
				{
					Scaling sc;
					sc.MultiplierX = pScaling->MultiplierX * int32_t(_multiplierX);
					sc.MultiplierY = pScaling->MultiplierY * int32_t(_multiplierY);
					sc.DivisorX    = pScaling->DivisorX    * int32_t(_divisorX);
					sc.DivisorY    = pScaling->DivisorY    * int32_t(_divisorY);
					_pTarget->Text( ScaleX(x), ScaleY(y), &sc, Text, Count );
				}
				else
				{
					Scaling sc;
					sc.MultiplierX = int32_t(_multiplierX);
					sc.MultiplierY = int32_t(_multiplierY);
					sc.DivisorX    = int32_t(_divisorX);
					sc.DivisorY    = int32_t(_divisorY);
					_pTarget->Text( ScaleX(x), ScaleY(y), &sc, Text, Count );
				}
			}
		}

		std::shared_ptr<Fonts::AbstractFont> Rescaler::GetFont() const
		{
			return _pTarget ? _pTarget->GetFont() : 0;
		}

	} /// end namespace

} /// end namespace

















namespace libGraphics
{
	namespace System
	{
		namespace StretchBlt
		{
			namespace Stepper
			{
				// This is the Bresenham-based "stepping" algorithm used as a core to
				// insert or miss out pixels/rows as part of the stretch.
				//
				// - The Stage1() routine is used to run the algorithm from the start
				//   point until the first visible point.  The output result can be used
				//   to initialise for all subsequent rows in a rectangle.
				//
				// - The Stage2() routine runs for the points in the visible portion
				//   and pulls data from the source to fill out the target.
				//
				// These routines are used to do both the horizontal and vertical
				// directions, since the logic is the same, only the actions are different.

				struct StartState
				{
					StartState() : Decision(0), NumSourceIncrements(0) {}
					int32_t  Decision;
					int32_t  NumSourceIncrements;
				};


				void Stage1( int32_t sourceSize, int32_t destSize, int32_t pixelsUntilVisible, StartState *out_Result )
				{
					// Discover state as soon as first point visible.
					// ** The conditions in this routine must be mirrored by those in Stage2 **

					StartState  result;

					if( sourceSize == destSize )
					{
						// Same size:  Optimise:  We don't do the Bresenham algorithm, Result.Decision not used.
						result.NumSourceIncrements = pixelsUntilVisible;
					}
					else if( sourceSize <= destSize )
					{
						// Expand:
						auto minorDelta  = sourceSize;    // Source pixels wide
						auto majorDelta  = destSize;      // Destination pixels wide

						while( pixelsUntilVisible > 0 )
						{
							--pixelsUntilVisible;
							result.Decision += minorDelta;
							if( result.Decision >= majorDelta )
							{
								result.Decision -= majorDelta;
								++result.NumSourceIncrements;
							}
						}
					}
					else
					{
						// Shrink:
						auto minorDelta  = destSize;      // Destination pixels wide
						auto majorDelta  = sourceSize;    // Source pixels wide

						while( pixelsUntilVisible > 0 )
						{
							++result.NumSourceIncrements;
							result.Decision += minorDelta;
							if( result.Decision >= majorDelta )
							{
								result.Decision -= majorDelta;
								--pixelsUntilVisible;
							}
						}
					}

					*out_Result = result;
				}


				template<typename HANDLER>
				void Stage2( int32_t sourceSize, int32_t destSize, int32_t decision, int32_t paintSizePixels, HANDLER &h )
				{
					// 'SourceSize' is the FULL size of the source.
					// 'DestSize' is the FULL size of the destination.
					// 'Decision' must be the value returned by stage 1 (preparation).
					// 'PaintSizePixels' is the size of the paint region to actually do.
					// NB: h's destination must be on the first visible pixel in the viewport.
					// NB: h's source must be the edge address + the 'NumSourceIncrements' value returned by stage 1 (preparation).

					// ** The conditions in this routine must be mirrored by those in Stage1() **

					if( sourceSize == destSize )
					{
						// Same size:  Optimise:  We don't do the Bresenham algorithm, Result.Decision not used.

						while( paintSizePixels > 0 )
						{
							--paintSizePixels;
							h.ActionAndIncDestAddress(); // ie: Read source, store at destination
							h.IncSourceAddress();
						}
					}
					else if( sourceSize <= destSize )
					{
						// Expand

						auto  minorDelta = sourceSize;    // Source pixels wide
						auto  majorDelta = destSize;      // Destination pixels wide

						while( paintSizePixels > 0 )
						{
							--paintSizePixels;
							h.ActionAndIncDestAddress(); // ie: Read source, store at destination
							decision += minorDelta;
							if( decision >= majorDelta )
							{
								decision -= majorDelta;
								h.IncSourceAddress();
							}
						}
					}
					else
					{
						// Shrink

						auto  minorDelta = destSize;      // Destination pixels wide
						auto  majorDelta = sourceSize;    // Source pixels wide

						while( paintSizePixels > 0 )
						{
							decision += minorDelta;
							if( decision >= majorDelta )
							{
								decision -= majorDelta;
								h.ActionAndIncDestAddress(); // ie: Read source, store at destination
								--paintSizePixels;
							}
							h.IncSourceAddress();
						}
					}
				}

			}  /// end namespace

		}  /// end namespace

	}  /// end namespace

}  /// end namespace

// - - EXPAND abstract sources -> 32bpp - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace libGraphics
{
	namespace System
	{
		namespace StretchBlt
		{
			namespace Internal
			{
				struct WrappedParams // ** FIX: name
				{
					uint32_t *_destAddress;          // initialised to first row leftmost visible pixel
					int32_t   _sourceWidthPixels;
					int32_t   _destWidthPixels;
					int32_t   _rowPrepDecision;
					int32_t   _paintWidthPixels;
				};

				class AbstractHorizontalHandler
				{
				public:
					virtual void SetStartPositionInSource( int32_t dx, int32_t dy ) = 0; // called ONCE to prime the object BEFORE it gets used.
					virtual void IncSourceAddress() = 0;
					virtual void Action( const WrappedParams & ) = 0;
				};

				class GeneralVerticalHandler
				{
				public:

					// This object is a functor for the vertical-direction "Stage2()".
					// This is called back by Stage2() for each scan line.
					// We call the AbstractHorizontalHandler to do each line rendering.

					GeneralVerticalHandler(
						AbstractHorizontalHandler *pAbsHH,
						uint32_t *destAddress,           // topmost pixel in viewport at target
						intptr_t bytesPerScanLineAtDestination,
						int32_t sourceWidthPixels,
						int32_t destWidthPixels,
						int32_t paintWidthPixels,
						int32_t rowPrepDecision )
							: _pAbsHH(pAbsHH)
							, _bytesPerScanLineAtDestination( bytesPerScanLineAtDestination )
					{
						_wp._destAddress       = destAddress;
						_wp._sourceWidthPixels = sourceWidthPixels;
						_wp._destWidthPixels   = destWidthPixels;
						_wp._paintWidthPixels  = paintWidthPixels;
						_wp._rowPrepDecision   = rowPrepDecision;
					}

					inline void IncSourceAddress()
					{
						_pAbsHH->IncSourceAddress();
					}

					inline void ActionAndIncDestAddress()
					{
						_pAbsHH->Action( _wp );
						reinterpret_cast<intptr_t &>( _wp._destAddress ) += _bytesPerScanLineAtDestination;
					}

				private:

					AbstractHorizontalHandler *_pAbsHH;
					intptr_t       _bytesPerScanLineAtDestination;
					WrappedParams  _wp;  // For convenience to pass by reference to m_pAbsHH->Action()

				};

				void BitmapScalerCore(
					System::template_Bitmap<int32_t,uint32_t> &bmpTarget,
					const Rect<int32_t> &areaOnTarget,
					const Rect<int32_t> &callersAreaOnSource, // Can be sub-area of the source bitmap.
					int32_t sourceBitmapWidthPixels,          // Dimensions of the source bitmap (full width)
					int32_t sourceBitmapHeightPixels,         // Dimensions of the source bitmap (full height)
					AbstractHorizontalHandler *pAbsHH,
					const Rect<int32_t> &viewport )           // Must be non-zero area wholly *within* the target.
				{
					// This is the core bitmap scaler that abstracts the source using an AbstractHorizontalHandler.
					// This allows many sources to target 32-bpp bitmaps.

					// ** We do NOT check the Viewport rectangle has area **
					assert( HasArea(viewport) );

					// ** We do NOT intersect the Viewport with the extents of the target **
					assert( HasArea(Intersection( viewport, Bitmaps::BitmapExtents(bmpTarget) )) );

					// Intersect target with viewport:
					if( ! HasArea(areaOnTarget) ) return; // Sanity check
					auto inter = Intersection( viewport, areaOnTarget );
					if( ! HasArea(inter) ) return;  // Result check

					// Clip AreaOnSource within the source bitmap area:
					if( ! HasArea(callersAreaOnSource) ) return;  // Sanity check
					auto areaOnSource = Intersection(callersAreaOnSource, Rect<int32_t>(0,0,sourceBitmapWidthPixels,sourceBitmapHeightPixels));
					if( ! HasArea(areaOnSource) ) return;  // Result check

					// Dimensions of desired target area that source will be scaled to fit:
					// This is logical coordinates (can be larger than the viewport):
					int32_t destWidthPixels      = Width(areaOnTarget);
					int32_t destHeightPixels     = Height(areaOnTarget);

					// If the target starts off the top/left of the viewport, this is the deltas:
					// Use 0 if we start within the viewport.
					int32_t pixelsUntilVisibleX  = (areaOnTarget.left < viewport.left) ? (viewport.left - areaOnTarget.left) : 0;
					int32_t pixelsUntilVisibleY  = (areaOnTarget.top  < viewport.top)  ? (viewport.top  - areaOnTarget.top)  : 0;

					// Dimensions to paint at the target (ie: Width and Height of (bitmap intersect viewport))
					int32_t paintWidthPixels      = Width(inter);
					int32_t paintHeightPixels     = Height(inter);

					// Dimensions of source area:
					int32_t sourceWidthPixels     = Width(areaOnSource);
					int32_t sourceHeightPixels    = Height(areaOnSource);

					// Do the preparation that will apply for all horizontal rows:
					Stepper::StartState  rowPrep;
					Stepper::Stage1( sourceWidthPixels, destWidthPixels, pixelsUntilVisibleX, &rowPrep );

					// Do the preparation that applies for the vertical direction:
					Stepper::StartState  colPrep;
					Stepper::Stage1( sourceHeightPixels, destHeightPixels, pixelsUntilVisibleY, &colPrep );

					// Inform the abstract source object of the (dx,dy) delta into the source surface
					// at which we're going to begin fetching:
					pAbsHH->SetStartPositionInSource(
						rowPrep.NumSourceIncrements + areaOnSource.left,
						colPrep.NumSourceIncrements + areaOnSource.top );

					// Determine the destination address, and bytes per scan line:
					intptr_t  bytesPerScanLineAtDestination = bmpTarget.BytesPerScanLine;
					auto pDestPixel = System::Raster::template_CalcPixelAddress(
						bmpTarget.TopLeft, inter.left, inter.top, bytesPerScanLineAtDestination );

					// Prepare the functor object that Stage2() will use:
					GeneralVerticalHandler  vh(
						pAbsHH,
						pDestPixel,
						bytesPerScanLineAtDestination,
						sourceWidthPixels,
						destWidthPixels,
						paintWidthPixels,
						rowPrep.Decision );

					// Do the expansion in the Y direction (does the X direction inside vh.Action()).
					Stepper::Stage2( sourceHeightPixels, destHeightPixels, colPrep.Decision, paintHeightPixels, vh );
				}

			}  /// end namespace

		}  /// end namespace

	}  /// end namespace

}  /// end namespace

// - - EXPAND 32bpp -> 32bpp - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace libGraphics
{
	namespace System
	{
		namespace StretchBlt
		{
			namespace Internal
			{
				template<bool TRANSPARENCY>
				class HorizontalHandler32to32
				{
				public:

					// A callback functor for the call to Stage2() within ExpandColourBitmapHorizontally::Action()
					// This does the pixel copying for colour 32-bpp bitmap -> colour 32-bpp bitmap blitting.
					// For performance, this wants to be inlined into Stage2().
					// This template is instantiated with a boolean to generate the "transparency" vs "opaque" versions.

					HorizontalHandler32to32( const uint32_t *sourceAddress, uint32_t *destAddress, uint32_t transparencyColour )
						: _sourceAddress(sourceAddress)
						, _destAddress(destAddress)
					{
						if( TRANSPARENCY )
						{
							_transparencyColour = transparencyColour;
						}
					}

					inline void IncSourceAddress()
					{
						++_sourceAddress;
					}

					inline void ActionAndIncDestAddress()
					{
						if(TRANSPARENCY)
						{
							if( *_sourceAddress != _transparencyColour )
							{
								*_destAddress = *_sourceAddress;
							}
							++_destAddress;
						}
						else
						{
							*_destAddress = *_sourceAddress; ++_destAddress;
						}
					}

				private:

					const uint32_t *_sourceAddress;
					uint32_t       *_destAddress;
					uint32_t        _transparencyColour;

				};

				template<bool TRANSPARENCY>
				class ExpandColourBitmapHorizontally: public AbstractHorizontalHandler
				{
				public:

					// This template is instantiated with a boolean to generate the "transparency" vs "opaque" versions.

					ExpandColourBitmapHorizontally( const uint32_t *sourceTopLeft, intptr_t bytesPerScanLineAtSource, uint32_t transparencyColour )
						: _sourceTopLeft( sourceTopLeft )
						, _sourceAddress(nullptr)
						, _bytesPerScanLineAtSource( bytesPerScanLineAtSource )
					{
						if( TRANSPARENCY )
						{
							_transparencyColour = transparencyColour;
						}
					}

					virtual void SetStartPositionInSource( int32_t dx, int32_t dy )
					{
						_sourceAddress = System::Raster::template_CalcPixelAddress( _sourceTopLeft, dx, dy, _bytesPerScanLineAtSource );
					}

					virtual void IncSourceAddress()
					{
						reinterpret_cast<intptr_t &>( _sourceAddress ) += _bytesPerScanLineAtSource;
					}

					virtual void Action( const WrappedParams &wp )
					{
						HorizontalHandler32to32<TRANSPARENCY>  hh( _sourceAddress, wp._destAddress, _transparencyColour );
						Stepper::Stage2( wp._sourceWidthPixels, wp._destWidthPixels, wp._rowPrepDecision, wp._paintWidthPixels, hh );
					}

				private:

					const uint32_t *_sourceTopLeft;
					const uint32_t *_sourceAddress;  // initialised to first row to fetch from (includes off-top offset)
					intptr_t        _bytesPerScanLineAtSource;
					uint32_t        _transparencyColour;

				};

			} /// end namespace

		} /// end namespace

	} /// end namespace

	namespace Bitmaps
	{
		void ScaleColourOpaque(
			System::template_Bitmap<int32_t,uint32_t> &bmpTarget,	const Rect<int32_t> &areaOnTarget,
			System::template_Bitmap<int32_t,uint32_t> &bmpSource,	const Rect<int32_t> &areaOnSource,
			const Rect<int32_t> &theViewport )
		{
			// Bitmap stretching of source 32-bpp to target 32-bpp.
			System::StretchBlt::Internal::ExpandColourBitmapHorizontally<false> ebh( bmpSource.TopLeft, bmpSource.BytesPerScanLine, 0 /*TransparencyColour not used*/ );
			System::StretchBlt::Internal::BitmapScalerCore( bmpTarget, areaOnTarget, areaOnSource, bmpSource.WidthPixels, bmpSource.HeightPixels, &ebh, theViewport );
		}

		void ScaleColourTransparent(
			System::template_Bitmap<int32_t,uint32_t> &bmpTarget,	const Rect<int32_t> &areaOnTarget,
			System::template_Bitmap<int32_t,uint32_t> &bmpSource,	const Rect<int32_t> &areaOnSource,
			const Rect<int32_t> &theViewport,
			uint32_t transparencyColour )
		{
			// Bitmap stretching of source 32-bpp to target 32-bpp with transparency colour.
			System::StretchBlt::Internal::ExpandColourBitmapHorizontally<true> ebh( bmpSource.TopLeft, bmpSource.BytesPerScanLine, transparencyColour );
			System::StretchBlt::Internal::BitmapScalerCore( bmpTarget, areaOnTarget, areaOnSource, bmpSource.WidthPixels, bmpSource.HeightPixels, &ebh, theViewport );
		}

	}  /// end namespace

}  /// end namespace

// - - EXPAND 1bpp -> 32bpp - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace libGraphics
{
	namespace System
	{
		namespace StretchBlt
		{
			namespace Internal
			{
				template<bool TRANSPARENCY>
				class HorizontalHandler1to32
				{
				public:

					// A callback functor for the call to Stage2() within ExpandColourBitmapHorizontally::Action()
					// This does the pixel copying for colour 1-bpp bitmap -> colour 32-bpp bitmap blitting.
					// For performance, this wants to be inlined into Stage2().
					// This template is instantiated with a boolean to generate the "transparency" vs "opaque" versions.

					HorizontalHandler1to32( const uint8_t *sourceAddress, uint32_t *destAddress, uint8_t initialMask, uint32_t foreColour, uint32_t backColour )
						: _sourceAddress(sourceAddress)
						, _destAddress(destAddress)
						, _foreColour(foreColour)
						, _backColour(backColour)
						, _mask(initialMask)
					{
					}

					inline void IncSourceAddress()
					{
						_mask >>= 1;
						if( _mask == 0 )
						{
							_mask = 0x80;
							++_sourceAddress;
						}
					}

					inline void ActionAndIncDestAddress()
					{
						if( *_sourceAddress & _mask )
						{
							*_destAddress = _foreColour;
						}
						else
						{
							if( ! TRANSPARENCY )
							{
								*_destAddress = _backColour;
							}
						}

						++_destAddress;
					}

				private:

					const uint8_t  *_sourceAddress;
					uint32_t       *_destAddress;
					uint32_t        _foreColour;
					uint32_t        _backColour;
					uint8_t         _mask;

				};

				template<bool TRANSPARENCY>
				class ExpandMonoBitmapHorizontally: public AbstractHorizontalHandler
				{
				public:

					// This template is instantiated with a boolean to generate the "transparency" vs "opaque" versions.

					ExpandMonoBitmapHorizontally( const uint8_t *sourceTopLeft, intptr_t bytesPerScanLineAtSource, uint32_t foreColour, uint32_t backColour )
						: _sourceTopLeft(sourceTopLeft)
						, _sourceAddress(nullptr)
						, _bytesPerScanLineAtSource(bytesPerScanLineAtSource)
						, _foreColour(foreColour)
						, _backColour(backColour)
						, _rowInitialMask(0)
					{
					}

					virtual void SetStartPositionInSource( int32_t dx, int32_t dy )
					{
						_sourceAddress = System::Raster::template_CalcPixelAddress( _sourceTopLeft, dx/8, dy, _bytesPerScanLineAtSource );
						_rowInitialMask = 0x80 >> (dx & 7);
					}

					virtual void IncSourceAddress()
					{
						reinterpret_cast<intptr_t &>(_sourceAddress) += _bytesPerScanLineAtSource;
					}

					virtual void Action( const WrappedParams &wp )
					{
						assert( _rowInitialMask != 0 ); /// should have been set by SetStartPositionInSource()
						HorizontalHandler1to32<TRANSPARENCY> hh( _sourceAddress, wp._destAddress, _rowInitialMask, _foreColour, _backColour );
						Stepper::Stage2( wp._sourceWidthPixels, wp._destWidthPixels, wp._rowPrepDecision, wp._paintWidthPixels, hh );
					}

				private:

					const uint8_t  *_sourceTopLeft;
					const uint8_t  *_sourceAddress;  // initialised to first row to fetch from (includes off-top offset)
					intptr_t        _bytesPerScanLineAtSource;
					uint32_t        _foreColour;
					uint32_t        _backColour;
					uint8_t         _rowInitialMask;

				};

			} /// end namespace

		} /// end namespace

	} /// end namespace

	namespace Bitmaps
	{
		void ScaleMonoOpaque(
			System::template_Bitmap<int32_t,uint32_t> &bmpTarget,   const Rect<int32_t> &areaOnTarget,
			System::template_Bitmap<int32_t,uint8_t> &bmpSource,   const Rect<int32_t> &areaOnSource,
			const Rect<int32_t> &viewport,
			uint32_t  foreColour,
			uint32_t  backColour )
		{
			// Bitmap stretching of source 1-bpp to target 32-bpp.
			System::StretchBlt::Internal::ExpandMonoBitmapHorizontally<false> ebh( bmpSource.TopLeft, bmpSource.BytesPerScanLine, foreColour, backColour );
			System::StretchBlt::Internal::BitmapScalerCore( bmpTarget, areaOnTarget, areaOnSource, bmpSource.WidthPixels, bmpSource.HeightPixels, &ebh, viewport );
		}

		void ScaleMonoTransparent(
			System::template_Bitmap<int32_t,uint32_t> &bmpTarget,  const Rect<int32_t> &areaOnTarget,
			System::template_Bitmap<int32_t,uint8_t> &bmpSource,   const Rect<int32_t> &areaOnSource,
			const Rect<int32_t> &viewport,
			uint32_t foreColour )
		{
			// Bitmap stretching of source 32-bpp to target 32-bpp with transparency colour.
			System::StretchBlt::Internal::ExpandMonoBitmapHorizontally<true> ebh( bmpSource.TopLeft, bmpSource.BytesPerScanLine, foreColour, 0 );
			System::StretchBlt::Internal::BitmapScalerCore( bmpTarget, areaOnTarget, areaOnSource, bmpSource.WidthPixels, bmpSource.HeightPixels, &ebh, viewport );
		}

	}  /// end namespace

}  /// end namespace


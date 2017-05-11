80GFX
=====

Retro 1980's computer graphics rendering library with classic mid-1980s features.
Written by Jonathan Markland, using mainly template-based C++, with separated
architecture.

May be of interest to eg: hobby OS writers who would like some routines
that do software-rendering without dependencies on loads of other stuff.

Could be a good source for enthusiasts who wish to "pick and choose" while
making their own graphics library.

Integer-arithmetic implementation.




Shapes
------

The library offers a classic list of shapes:

- Point
- Line
- Rectangle
- Triangle
- Bezier curve
- Polygon
- Arc
- Pie slice
- Secant
- Ellipse
- Polygon 

All shapes available as outline or filled.



Polygons and Brushes
--------------------

Any shape can "contribute" to a polygon data buffer, which can be 
scan-converted using a variety of brushes.

- Solid Brush
- Pattern Brush
- Average Mix Brush

Pen facility is present, including a fast solid colour single-pixel pen,
and a thickened circular pen of any size, that draws using a brush.



Fonts
-----

1980s style super-retro blocky fonts rendered from various sources.
No anti-aliasing because it's not retro enough.
Re-scaling supported.

- 8*N pixel fonts, rendered from statically-allocated font definition.
- Coloured fonts from TGA files (a format Jonathan invented).



Device Architecture
-------------------

"Device" architecture for drawing to:-

- Bitmaps (ie: 2D memory array, or linear frame buffer)
- Measuring extents of any shapes  eg: polygons!
- Metafiles  (another format Jonathan invented)
- Scaling and translation "device" that adjusts coordinates before piping 
  through to any other "device".

Metafile parsing and replay is supported, targetting any "device".



Implementation Notes
====================

- Template based implementation, parameterisable PIXEL type.

- The "Device" architecture is optional, you can "get in underneath"
  on a lot of things.

- Separated point-plot and raster-painting routines.

Shapes can have their outline lines sent to any "Line Receiver"
which means:

- Plot on the screen as pixel lines, 
- Store in a polygon buffer for scan conversion.
- Store in a fixed-sized array of left/right edge extents, useful
  for rendering solid-filled triangles without needing a full
  polygon scan converting "sort" to be done!

Bresenham implementation for lines, circles, and Jonathan's 
adaptation to ellipses.


- Colour and Mono bitmaps supporting transparent sections by keyed colour


Redesign notes
==============
The code contains some TODOs to smooth internal design.
There are also some optimisation opportunities currently missed.


Demonstration Images (as jpegs)
===============================

Bar chart with patterned brush
![Bar chart](/Examples/BarChart.jpg)

Pie chart with patterned brush fills, and thick pen outline (circular nib).
![Pie chart](/Examples/PieChart.jpg)

Polygon scan converted capital letter 'A' (no beziers)
![Capital A](/Examples/CapitalA.jpg)

Polygon scan converted Omega symbol with lines and bezier curves, and thickened outline.
![Omega demo - filled and outline shape with beziers](/Examples/Omega.jpg)

Multiple basic shapes contributing to a polygon that is scan-converted in a single pass.
![Polygon with holes](/Examples/PolyWithHoles.jpg)

Multiple basic shapes contributing to a polygon that is scan-converted in a single pass.
![Polygon with holes](/Examples/PolyWithHoles2.jpg)

Demonstration of brush types: Solid, Patterned, Average Mix, and painting the R,G,B channels independently.
![Brushes](/Examples/Brushes.jpg)

Font demonstration.  Bitmap fonts supported, of user-definable formats.
![Font demo](/Examples/FontDemo.jpg)

"Direct" drawing:  Drawing filled shapes without a polygon scan conversion buffer (or it's associated sort() ! ).
![Direct drawing functions](/Examples/DirectFunctionsDemo.jpg)

Stretched bitmap drawing used to show a palette.
![Palette bitmap demo](/Examples/PaletteDemo.jpg)



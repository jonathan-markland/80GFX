//
// The BAR chart example
//


#include <fstream>
#include <string>
#include <iostream>
#include "libGraphics_Main.h"
#include "libGraphics_Fonts.h"
#include "ColoursEnum.h"
#include "Resources_FixedFont.h"
#include "AssistanceForExamples.h"
#include "AssistanceChartDrawing.h"



const int DemoBitmapWidth = 640;
const int DemoBitmapHeight = 480;


 
int main()
{
	VectorOfInt32  testData = { 100,200,300,400,500 };
	
	return WithNewBitmapDo( DemoBitmapWidth, DemoBitmapHeight, "output.data", 
		[&testData]( libGraphics::Devices::AbstractDevice &theDevice )
		{
            DrawBarChart( theDevice, &testData, DemoBitmapWidth, DemoBitmapHeight );
		}) ? 0 : 1;
}

//
// AssistanceChartDrawing.h
//

#pragma once

typedef std::vector<int32_t> VectorOfInt32;

void DrawBarChart( libGraphics::Devices::AbstractDevice &theDevice, const VectorOfInt32 *pVec, int32_t projectionWidth, int32_t projectionHeight );
void DrawPieChart( libGraphics::Devices::AbstractDevice &theDevice, const VectorOfInt32 *pVec, int32_t projectionWidth, int32_t projectionHeight );

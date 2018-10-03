emcc \
 lib80GFXDemo.cpp \
 ../DemoSharedLibrary/AssistanceChartDrawing.cpp \
 ../lib80GFX/lib80GFX_Abstractions.cpp \
 ../lib80GFX/lib80GFX_Fonts.cpp \
 ../lib80GFX/lib80GFX_Main.cpp \
 ../lib80GFX/lib80GFX_Maths.cpp \
 ../lib80GFX/lib80GFX_MetaFile.cpp \
 ../lib80GFX/lib80GFX_NumStr.cpp \
 ../lib80GFX/lib80GFX_Resources_FixedFont_CamputersLynxII.cpp \
 ../lib80GFX/lib80GFX_Resources_FixedFont_PcBios8x16.cpp \
 ../lib80GFX/lib80GFX_Resources_Pattern1616.cpp ../lib80GFX/lib80GFX_SineTable.cpp \
 ../lib80GFX/lib80GFX_SmallStringBuilder.cpp \
 -s WASM=1 -s USE_SDL=2 -O3 -std=c++11 -o index.js -I ../lib80GFX 


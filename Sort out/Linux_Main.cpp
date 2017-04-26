
#include "gtk/gtk.h"
#include "GraphicsDemo.h"
#include "FileLoader.h"
#include "Linux_FileOpener.h"



const char *g_courierNewPath = "/home/jonathan/Documents/Work/LinuxWork/Gauntlet/Fonts/Courier New 10.tga";
const char *g_bigFontPath    = "/home/jonathan/Documents/Work/LinuxWork/Gauntlet/Fonts/BigFont.tga";



int main()
{
    Linux_FileOpener  courierNewTgaFontFileOpener( g_courierNewPath );
    Linux_FileOpener  bigFontFileOpener( g_bigFontPath );
    gtk_init( nullptr, nullptr );
    GraphicsDemoMain( &courierNewTgaFontFileOpener, &bigFontFileOpener );
    return 0;
}

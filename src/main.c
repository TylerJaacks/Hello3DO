#include "stdio.h"
#include "displayutils.h"
#include "celutils.h"

uint32 color;

ScreenContext *sc;
GrafCon gcon;
Item sport;
Item vbl;

// Initialize everything.
void init()
{
    OpenGraphicsFolio();

    sc = (ScreenContext*) AllocMem(sizeof(ScreenContext), MEMTYPE_ANY);

    CreateBasicDisplay(sc, DI_TYPE_DEFAULT, 2);

    sc->sc_CurrentScreen = 0;

    sport = GetVRAMIOReq();
    vbl = GetVBLIOReq();

    // Make the background red.
    color = MakeRGB15Pair(31, 0, 0);
}

int main(int argc, char *argv[])
{
    // Initialize all the things.
    init();

    while (TRUE)
    {
        // Clear the screen.
        SetVRAMPages(sport, 
            sc->sc_Bitmaps[sc->sc_CurrentScreen]->bm_Buffer,
            color,
            sc->sc_NumBitmapPages,
            -1);

        // Set the position of the text.
        gcon.gc_PenX = 10;
        gcon.gc_PenY = 10;

        // Draw the text to the screen.
        DrawText8(&gcon, sc->sc_BitmapItems[sc->sc_CurrentScreen], (uint8*) "Hello, world!");

        // Display to the screen.
        DisplayScreen(sc->sc_ScreenItems[sc->sc_CurrentScreen], 0);

        // Flip the screen
        sc->sc_CurrentScreen = 1 - sc->sc_CurrentScreen;

        // Wait for VBlank.
        WaitVBL(vbl, 1);
    }

    return 0;
}
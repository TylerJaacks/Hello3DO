#include "stdio.h"
#include "displayutils.h"
#include "celutils.h"

#define CURRENT_SCREEN sc->sc_CurrentScreen

#define CURRENT_SCREEN_ITEMS sc->sc_ScreenItems[CURRENT_SCREEN]
#define CURRENT_SCREEN_BITMAPS sc->sc_Bitmaps[CURRENT_SCREEN]

#define FRACBITS_16 16
#define FRACBITS_20 20

ScreenContext *sc;
Item sport;
Item vbl;
CCB *ccb;

ubyte *bg;


// Initialize everything.
void init()
{
    OpenGraphicsFolio();

    sc = (ScreenContext*) AllocMem(sizeof(ScreenContext), MEMTYPE_ANY);

    CreateBasicDisplay(sc, DI_TYPE_DEFAULT, 2);

    CURRENT_SCREEN = 0;

    sport = GetVRAMIOReq();
    vbl = GetVBLIOReq();

    bg = (ubyte*) LoadImage("Graphics/bg", NULL, NULL, sc);
    
    ccb = LoadCel("Graphics/sun", MEMTYPE_CEL);

    // Move the cel to the center of the screen;
    ccb->ccb_XPos = 144 << FRACBITS_16;
    ccb->ccb_YPos = 104 << FRACBITS_16;

    // Scale the cel by 2 times.
    ccb->ccb_HDX = 2 << FRACBITS_20;
    ccb->ccb_VDY = 2 << FRACBITS_16;

    // Makes it so there is only one cel in the list.
    ccb->ccb_Flags |= CCB_LAST;
}

int main(int argc, char *argv[])
{
    // Initialize all the things.
    init();

    while (TRUE)
    {
        // Set the background to our bitmap.
        CopyVRAMPages(sport, CURRENT_SCREEN_BITMAPS->bm_Buffer, bg, sc->sc_NumBitmapPages, -1);

        DrawCels(sc->sc_BitmapItems[CURRENT_SCREEN], ccb);

        // Display to the screen.
        DisplayScreen(CURRENT_SCREEN_ITEMS, 0);

        // Flip the screen
        CURRENT_SCREEN = 1 - CURRENT_SCREEN;

        // Wait for VBlank.
        WaitVBL(vbl, 1);
    }

    return 0;
}
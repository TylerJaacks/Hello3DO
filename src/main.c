#include "stdio.h"
#include "displayutils.h"
#include "celutils.h"
#include "controlpad.h"
#include "event.h"

#define CURRENT_SCREEN sc->sc_CurrentScreen

#define CURRENT_SCREEN_ITEMS sc->sc_ScreenItems[CURRENT_SCREEN]
#define CURRENT_SCREEN_BITMAPS sc->sc_Bitmaps[CURRENT_SCREEN]

#define FRACBITS_16 16
#define FRACBITS_20 20

#define CEL_VEL_16 (4 << FRACBITS_16)

#define BULLET_VEL_16 (8 << FRACBITS_16)

ScreenContext *sc;
CCB *ccb;

CCB *bullets[100];

ControlPadEventData eventData;

Item sport;
Item vbl;

ubyte *bg;

uint32 previousButtons;
uint32 buttons;

int i;
int j;

// Initialize everything.
void init()
{
    OpenGraphicsFolio();

    sc = (ScreenContext *)AllocMem(sizeof(ScreenContext), MEMTYPE_ANY);

    CreateBasicDisplay(sc, DI_TYPE_DEFAULT, 2);

    CURRENT_SCREEN = 0;

    sport = GetVRAMIOReq();
    vbl = GetVBLIOReq();

    bg = (ubyte *)LoadImage("Graphics/bg", NULL, NULL, sc);

    ccb = LoadCel("Graphics/sun", MEMTYPE_CEL);

    // Move the cel to the center of the screen;
    ccb->ccb_XPos = 144 << FRACBITS_16;
    ccb->ccb_YPos = 104 << FRACBITS_16;
    // Makes it so there is only one cel in the list.
    // ccb->ccb_Flags |= CCB_LAST;

    InitEventUtility(1, 0, LC_Observer);
}

int main(int argc, char *argv[])
{
    // Initialize all the things.
    init();

    while (TRUE)
    {
        /* Update */

        /* Input */

        GetControlPad(1, FALSE, &eventData);

        buttons = eventData.cped_ButtonBits;

        if (buttons & ControlLeft)
        {
            ccb->ccb_XPos -= CEL_VEL_16;

            if (buttons & ControlUp)
            {
                ccb->ccb_YPos -= CEL_VEL_16;
            }
            else if (buttons & ControlDown)
            {
                ccb->ccb_YPos += CEL_VEL_16;
            }

            if (buttons & ControlA)
            {
                if (i <= 100)
                {
                    bullets[i] = LoadCel("Graphics/bullet", MEMTYPE_CEL);

                    bullets[i]->ccb_XPos = ccb->ccb_XPos;
                    bullets[i]->ccb_YPos = ccb->ccb_YPos;

                    i++;
                }
                else
                {
                    i = 0;
                }
            }
        }
        else if (buttons & ControlRight)
        {
            ccb->ccb_XPos += CEL_VEL_16;

            if (buttons & ControlUp)
            {
                ccb->ccb_YPos -= CEL_VEL_16;
            }
            else if (buttons & ControlDown)
            {
                ccb->ccb_YPos += CEL_VEL_16;
            }

            if (buttons & ControlA)
            {
                if (i <= 100)
                {
                    bullets[i] = LoadCel("Graphics/bullet", MEMTYPE_CEL);

                    bullets[i]->ccb_XPos = ccb->ccb_XPos;
                    bullets[i]->ccb_YPos = ccb->ccb_YPos;

                    i++;
                }
                else
                {
                    i = 0;
                }
            }
        }

        else if (buttons & ControlUp)
        {
            ccb->ccb_YPos -= CEL_VEL_16;

            if (buttons & ControlLeft)
            {
                ccb->ccb_XPos -= CEL_VEL_16;
            }
            else if (buttons & ControlRight)
            {
                ccb->ccb_XPos += CEL_VEL_16;
            }

            if (buttons & ControlA)
            {
                if (i <= 100)
                {
                    bullets[i] = LoadCel("Graphics/bullet", MEMTYPE_CEL);

                    bullets[i]->ccb_XPos = ccb->ccb_XPos;
                    bullets[i]->ccb_YPos = ccb->ccb_YPos;

                    i++;
                }
                else
                {
                    i = 0;
                }
            }
        }
        else if (buttons & ControlDown)
        {
            ccb->ccb_YPos += CEL_VEL_16;

            if (buttons & ControlLeft)
            {
                ccb->ccb_XPos -= CEL_VEL_16;
            }
            else if (buttons & ControlRight)
            {
                ccb->ccb_XPos += CEL_VEL_16;
            }

            if (buttons & ControlA)
            {
                if (i <= 100)
                {
                    bullets[i] = LoadCel("Graphics/bullet", MEMTYPE_CEL);

                    bullets[i]->ccb_XPos = ccb->ccb_XPos;
                    bullets[i]->ccb_YPos = ccb->ccb_YPos;

                    i++;
                }
                else
                {
                    i = 0;
                }
            }
        }
        else if (buttons & ControlA)
        {
            if (i <= 100)
            {
                bullets[i] = LoadCel("Graphics/bullet", MEMTYPE_CEL);

                bullets[i]->ccb_XPos = ccb->ccb_XPos;
                bullets[i]->ccb_YPos = ccb->ccb_YPos;

                i++;
            }
            else
            {
                i = 0;
            }

            if (buttons & ControlA)
            {
                if (i <= 100)
                {
                    bullets[i] = LoadCel("Graphics/bullet", MEMTYPE_CEL);

                    bullets[i]->ccb_XPos = ccb->ccb_XPos;
                    bullets[i]->ccb_YPos = ccb->ccb_YPos;

                    i++;
                }
                else
                {
                    i = 0;
                }
            }
        }

        /* Draw */

        // Set the background to our bitmap.
        CopyVRAMPages(sport, CURRENT_SCREEN_BITMAPS->bm_Buffer, bg, sc->sc_NumBitmapPages, -1);

        // Draws the Sun.
        DrawCels(sc->sc_BitmapItems[CURRENT_SCREEN], ccb);

        j = 0;

        for (; j < 100; j++)
        {
            DrawCels(sc->sc_BitmapItems[CURRENT_SCREEN], bullets[j]);

            bullets[j]->ccb_XPos += BULLET_VEL_16;
        }

        // Display to the screen.
        DisplayScreen(CURRENT_SCREEN_ITEMS, 0);

        // Flip the screen
        CURRENT_SCREEN = 1 - CURRENT_SCREEN;

        // Wait for VBlank.
        WaitVBL(vbl, 1);

        previousButtons = buttons;
    }

    return 0;
}
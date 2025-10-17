/**
 * @brief   News Reader
 * @author  Thomas Cherryhomes
 * @email   thom dot cherryhomes at gmail dot com
 * @license gpl v. 3, see LICENSE for details.
 * @verbose Bar routines
 */

#include <cmoc.h>
#include <coco.h>
#include "globals.h"
#include "cocotext.h"
#include "bar.h"


/**
 * @brief toggle bar for Y position (0-15) 
 * @param y vertical position (0-15) 
 */
void bar(int y)
{
    byte *sp;
    int o = 0;

    if (textMode == 32)
    {
        sp = (byte *)SCREEN_RAM_TOP_32;
        o = y << 5; // Multiply by 32
        sp += o;
        // In 32 column mode, the bar is a full line of inverse text
        for (int i=0;i<32;i++)
        {
            // Only change non-semigraphics characters
            if (*sp < 0x80)
                *sp ^= 0x40;
            sp++;
        }
    }
}


/**
 * @brief draw shadow for color c at vert pos y
 * @param y Vertical position (0-15)
 * @param c bkg color (0-7) 
 */
void shadow(int y, int c)
{
    byte *sp = (byte *)SCREEN_RAM_TOP_32;
    int o = y << 5;

    sp += o;

    *sp = (byte)c | 0x0b;
    memset(sp+1,c | 0x03, 31);
}

void hd_bar(byte y, const char *text, bool rev)
{
    gotoxy(0, y);
    int width;

    if (hirestxt_mode)
    {
        width = textMode;
    }
    else
    {
        width = textMode - 1;
    }

    reverse(rev);

    printf("%-*s", width, text);

    reverse(false);
}

void multiline_hd_bar(byte y, int lines, const char *text, bool rev)
{
    gotoxy(0, y);

    reverse(rev);

    printf("%-*s", (textMode * lines), text);

    reverse(false);
}




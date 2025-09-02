/**
 * @brief   News Reader
 * @author  Thomas Cherryhomes
 * @email   thom dot cherryhomes at gmail dot com
 * @license gpl v. 3, see LICENSE for details.
 * @verbose Bar routines
 */

#include <cmoc.h>
#include <coco.h>
#include "bar.h"
#include "globals.h"

#define SCREEN_RAM_TOP_32 0x0400
#define SCREEN_RAM_TOP_40_80 0x6C000
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
 * @brief Draw text with underline on/off at line y
 * @param y Vertical position (0-23)
 * @param text The text to print
 * @param on Underline on/off 
 */
void underline(int y, const char *text, BOOL on)
{
    locate(0, (byte) y);
    attr(0,0, FALSE, on);
    printf("%s", text);
}

void hd_bar(byte y, int fgcolor, int bgcolor, const char *text)
{
    locate(0, y);
    attr((byte) fgcolor,(byte) bgcolor, FALSE, FALSE);
    if (textMode == 40)
    {
        printf("%-39s", text);
    }
    else
    {
        printf("%-79s", text);
    }
    attr(FG_BLACK, BG_GREEN,FALSE,FALSE);
}

void multiline_hd_bar(byte y, int fgcolor, int bgcolor, int lines, const char *text)
{
    char fmtstr[8];

    locate(0, y);
    attr((byte) fgcolor,(byte) bgcolor, FALSE, FALSE);
    sprintf(fmtstr, "%%-%ds", (textMode * lines) - 1);
    printf(fmtstr, text);
    attr(FG_BLACK, BG_GREEN,FALSE,FALSE);
}

void print_lowercase_as_reverse(const char *text)
{
    attr(FG_BLACK, BG_GREEN, FALSE,FALSE);

    int len = strlen(text);
    for (int i = 0; i < len; i++)
    {
        if (text[i] >= 'a' && text[i] <= 'z')
        {
            attr(FG_WHITE, BG_BLUE, FALSE, FALSE);
            putchar(text[i] - 32);
        }
        else
        {
            attr(FG_BLACK, BG_GREEN, FALSE,FALSE);
            putchar(text[i]);
        }
    }
    attr(FG_BLACK, BG_GREEN, FALSE,FALSE);
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




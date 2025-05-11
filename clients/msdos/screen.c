/**
 * @brief Screen Routines
 */

#include <string.h>
#include <i86.h>
#include "screen.h"
#include "globals.h"

// explicit prototype for outb to avoid collision
extern unsigned int outp(int portid, int value);

/**
 * @brief macro to generate attribute byte
 */
#define ATTR(b,f) (b << 4) | f;

/**
 * @brief Current drawing colors
 */
color _bgColor = BLUE, _fgColor = WHITE;

/**
 * @brief current max lines per screen
 */
static unsigned char max_lines = 25;

/**
 * @brief Register pack
 */
static union REGS r;

/**
 * @brief pointer to video ram
 */
static unsigned char far *_vram;

/**
 * @brief get/set BIOS output mode
 * @param newmode -1 = Get, other = set mode
 * @return The currently set mode
 */
signed char mode(signed char newmode)
{
    // Set active page to 0
    r.h.ah = 0x05;
    r.h.al = 0;
    int86(0x10,&r,&r);

    if (newmode > -1)
    {
        r.h.ah = 0x00;
        r.h.al = newmode;
        int86(0x10,&r,&r);
    }

    r.h.ah = 0x0f;
    int86(0x10,&r,&r);

    // set VRAM pointer based on returned graphics mode
    switch (r.h.al)
    {
    case 0x00:
    case 0x01:
    case 0x02:
    case 0x03:
    case 0x04:
    case 0x05:
    case 0x06:
    case 0x08:
    case 0x09:
    case 0x0A:
        _vram = MK_FP(0xB800,0);
        break;
    case 0x07:
        _vram = MK_FP(0xB000,0);
        break;
    case 0x0D:
    case 0x0E:
    case 0x0F:
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    default:
        _vram = MK_FP(0xA000,0);
    }

    max_cols = r.h.ah;

    return r.h.al;
}

/**
 * @brief draw box with current foreground/background
 * @param x1 top-left column
 * @param y1 top-left row
 * @param x2 bottom-right column
 * @param y2 bottom-right row
 */
void box(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2)
{
    /* Then scroll by max lines */
    r.h.ah = 0x07;
    r.h.al = y2-y1;
    r.h.bh = ATTR(_bgColor,_fgColor);
    r.h.ch = y1;
    r.h.cl = x1+column_offset;
    r.h.dh = y2;
    r.h.dl = x2+column_offset;
    int86(0x10,&r,&r);
}

/**
 * @brief clear whole screen with current foreground/background using box()
 */
void clrscr(void)
{
    /* First, get # of columns */
    r.h.ah = 0x0F;
    int86(0x10,&r,&r);
    max_cols = r.h.ah;

    box(0,0,max_cols,max_lines);
}

/**
 * @brief draw shadow at edges of rectangle.
 * @param x1 top-left column
 * @param y1 top-left row
 * @param x2 bottom-right column
 * @param y2 bottom-right row
 */
void shadow(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2)
{
    unsigned char i=0; // Row index

    _bgColor=BLACK;

    /* Adjust shadow coordinates */
    x1++;
    y1++;
    x2++;

    box(x2,y1,x2,y2); // right
    box(x1,y2,x2,y2); // bottom
}

/**
 * @brief place character at cursor with current fg/bg
 * @param x column to place character
 * @param y row to place character
 * @param c Character to place
 */
void cputc(unsigned char x, unsigned char y, const unsigned char c)
{
    /* Place cursor */
    r.h.ah = 0x02;
    r.h.bh = 0;
    r.h.dh = y;
    r.h.dl = x+column_offset;
    int86(0x10,&r,&r);

    r.h.ah = 0x0e;
    r.h.al = c;
    r.h.bh = 0;

    int86(0x10,&r,&r);
}

/**
 * @brief place string at cursor, with current fg/bg.
 * @param x column to place cursor
 * @param y row to place cursor
 * @param s NULL terminated String to place.
 */
void cputs(unsigned char x, unsigned char y, const char *s)
{
    /* Place cursor */
    r.h.ah = 0x02;
    r.h.bh = 0x00;
    r.h.dh = y;
    r.h.dl = x+column_offset;
    int86(0x10,&r,&r);
    
    do
    {
        if (*s == 0x0a)
        {
            r.h.ah = 0x0e;
            r.h.al = 0x0d;
            r.h.bh = 0;

            int86(0x10,&r,&r);
        }

        r.h.ah = 0x0e;
        r.h.al = *s;
        r.h.bh = 0;
        int86(0x10,&r,&r);
    }
    
    while(*s++);
}

/**
 * @brief bar
 */
void bar(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char h)
{
    int y=y1;
    int x=x1+column_offset;

    for (y=y1;y<y1+h;y++)
    {
        for (x=x1+column_offset;x<x2+column_offset;x++)
        {
            _vram[(((y*max_cols)+x) << 1)+1] ^= 0x7F;
        }
    }
}

/**
 * @brief read keyboard
 * @return key pressed.
 */
int getk(void)
{
    r.h.ah = 0x00;
    int86(0x16,&r,&r);

    return r.x.ax;
}

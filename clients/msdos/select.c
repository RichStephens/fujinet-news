/**
 * @brief   FujiNet News Reader for MS-DOS
 * @author  Thomas Cherryhomes
 * @email   thom dot cherryhomes at gmail dot com
 * @license gpl v. 3, see LICENSE for details.
 * @verbose selection logic
 */

#include "screen.h"
#include "globals.h"

/**
 * @brief Set up and process menu
 * @param x1 top-left column of first option
 * @param y1 top-left row of first option
 * @param x2 rightmost column of option
 * @param y2 Size of option in rows
 * @param m  number of options
 * @param s  Selected first option
 * @param p  positive state
 * @param n  negative state
 */
State select(unsigned char x1,
             unsigned char y1,
             unsigned char x2,
             unsigned char h,
             unsigned char m,
             unsigned char s,
             State p,
             State n)
{
    bar(x1,y1+(s*h),x2,h);

    while(1)
    {
        switch(getk())
        {
        case 0x4800: // UP ARROW
            bar(x1,y1+(s*h),x2,h);
            if (s)
            {
                s--;
            }
            bar(x1,y1+(s*h),x2,h);
            break;
        case 0x5000: // DOWN ARROW
            bar(x1,y1+(s*h),x2,h);
            if (s<m-1)
            {
                s++;
            }
            bar(x1,y1+(s*h),x2,h);
            break;
        case 0x1C0D: // ENTER
            return p;
        case 0x011B: // ESC
            return n;
        default:
            break;
        }
    }
}

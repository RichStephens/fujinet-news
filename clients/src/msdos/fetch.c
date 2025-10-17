/**
 * @brief   FujiNet News Reader for MS-DOS
 * @author  Thomas Cherryhomes
 * @email   thom dot cherryhomes at gmail dot com
 * @license gpl v. 3, see LICENSE for details.
 * @verbose fetch data into buffer
 */

#include <dos.h>
#include "screen.h"

union REGS r;
struct SREGS sr;

struct _s
{
    unsigned short bw;
    unsigned char connected;
    unsigned char error;
} s;

void fetch(char *url, char *buf)
{
    s.bw = 0;
    s.connected = 0;
    s.error = 0;

    while (s.error != 1)
    {
        /* /\* open *\/ */
        r.h.dl = 0x80;
        r.h.ah = 'O';
        r.h.al = 0x71;
        r.h.cl = 0x0c; // HTTP GET
        r.h.ch = 0x00;
        r.x.si = 0x0000;
        sr.es = FP_SEG(url);
        r.x.bx = FP_OFF(url);
        r.x.di = 256;
        int86x(0xF5,&r,&r,&sr);
        
        /* GET # of bytes waiting for result */
        r.h.dl = 0x40;
        r.h.ah = 'S';
        r.h.al = 0x71;
        r.x.cx = 0x0000;
        r.x.si = 0x0000;
        sr.es = FP_SEG(&s);
        r.x.bx = FP_OFF(&s);
        r.x.di = sizeof(s);
        int86x(0xF5,&r,&r,&sr);
        
        /* Read Result into buffer */
        r.h.dl = 0x40;
        r.h.ah = 'R';
        r.h.al = 0x71;
        r.x.cx = s.bw;
        r.x.si = 0x0000;
        sr.es = FP_SEG(buf);
        r.x.bx = FP_OFF(buf);
        r.x.di = s.bw;
        int86x(0xF5,&r,&r,&sr);
    }
}

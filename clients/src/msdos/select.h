/**
 * @brief   FujiNet News Reader for MS-DOS
 * @author  Thomas Cherryhomes
 * @email   thom dot cherryhomes at gmail dot com
 * @license gpl v. 3, see LICENSE for details.
 * @verbose selection logic
 */

#ifndef SELECT_H
#define SELECT_H

#include "globals.h"

State select(unsigned char x1,
             unsigned char y1,
             unsigned char x2,
             unsigned char y2,
             unsigned char m,
             unsigned char s,
             State p,
             State n,
             State pu,
             State pd,
             unsigned char *si);

#endif /* SELECT_H */

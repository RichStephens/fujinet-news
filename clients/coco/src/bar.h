/**
 * @brief   News Reader
 * @author  Thomas Cherryhomes
 * @email   thom dot cherryhomes at gmail dot com
 * @license gpl v. 3, see LICENSE for details.
 * @verbose Bar routines
 */

#ifndef BAR_H
#define BAR_H

#include <cmoc.h>
#include <coco.h>

/**
 * @brief toggle bar for Y position (0-15) 
 * @param y vertical position (0-15) 
 */
void bar(int y);


/**
 * @brief Draw text with underline on/off at line y
 * @param y Vertical position (0-23)
 * @param text The text to print
 * @param on Underline on/off 
 */
void underline(int y, const char *text, BOOL on);

/**
 * @brief Draw a line of text across the screen with fg/bg colors
 * @param y Vertical position (0-23)
 * @param fgcolor The foreground color
 * @param bgcolor The background color
 * @param text The text to print 
 */
void hd_bar(byte y, int fgcolor, int bgcolor, const char *text);

/**
 * @brief Draw a line of text across the screen with fg/bg colors
 * @param y Vertical position (0-23)
 * @param fgcolor The foreground color
 * @param bgcolor The background color
 * @param lines The number of consecutive lines to highlight
 * @param text The text to print 
 */
void multiline_hd_bar(byte y, int fgcolor, int bgcolor, int lines, const char *text);

/**
 * @brief Print a line of text with lowercase characters reversed in 40/80 column mode
 *        Assume default colors to be black on green, reverse white on blue
 */
void print_lowercase_as_reverse(const char *text);

/**
 * @brief Print text as reverse in any mode (32, 42, 51, 40, 80)
 *        Assume default colors in 40/80 mode
 *        to be black on green, reverse white on blue
 */ 
void print_reverse(const char *text);

/**
 * @brief draw shadow for color c at vert pos y
 * @param y Vertical position (0-15)
 * @param c bkg color (0-7) 
 */
void shadow(int y, int c);

#endif /* BAR_H */

/**
 * @brief   News Reader
 * @author  Thomas Cherryhomes
 * @email   thom dot cherryhomes at gmail dot com
 * @license gpl v. 3, see LICENSE for details.
 * @verbose Bar routines
 */

#ifndef BAR_H
#define BAR_H

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
void hd_bar(int y, int fgcolor, int bgcolor, const char *text);

/**
 * @brief draw shadow for color c at vert pos y
 * @param y Vertical position (0-15)
 * @param c bkg color (0-7) 
 */
void shadow(int y, int c);

#endif /* BAR_H */

/**
 * @brief Header for screen routines
 */

#ifndef SCREEN_H
#define SCREEN_H

/**
 * @brief RGBI colors
 */
typedef enum _color
{
    BLACK = 0,
    BLUE = 1,
    GREEN = 2,
    CYAN = 3,
    RED = 4,
    MAGENTA = 5,
    BROWN = 6,
    GRAY = 7,
    DARK_GRAY = 8,
    LIGHT_BLUE = 9,
    LIGHT_GREEN = 10,
    LIGHT_CYAN = 11,
    LIGHT_RED = 12,
    LIGHT_MAGENTA = 13,
    LIGHT_YELLOW = 14,
    WHITE = 15
} color;

extern color _fgColor;
extern color _bgColor;

signed char mode(signed char newmode);
void box(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2);
void shadow(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2);
void bar(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2);
void clrscr(void);
void cputc(unsigned char x, unsigned char y, const unsigned char c);
void cputs(unsigned char x, unsigned char y, const char *s);
int getk(void);

#endif /* SCREEN_H */

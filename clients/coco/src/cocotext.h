#ifndef COCOTEXT_H
#define COCOTEXT_H

#include "fujinet-fuji.h"

#define WIDTH_CANCEL 999

byte text_width_menu(void);
void set_text_width(byte screen_width);
char *screen_upper(char *s);
void reverse(bool onoff);
void print_lowercase_as_reverse(const char *text);
void print_reverse(byte x, byte y, const char *text);

#endif
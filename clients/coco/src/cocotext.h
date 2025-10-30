#ifndef COCOTEXT_H
#define COCOTEXT_H

#include "fujinet-fuji.h"

#define WIDTH_CANCEL 999

void hirestxt_init(byte screen_width);
void hirestxt_exit(void);
byte text_width_menu(void);
void set_text_width(byte screen_width);
bool in_hires_mode();
#endif
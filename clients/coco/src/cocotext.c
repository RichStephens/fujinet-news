#include <cmoc.h>
#include <coco.h>
#include "fujinet-fuji.h"
#include "hirestxt.h"
#include "globals.h"
#include "bar.h"
#include "cocotext.h"

void hirestxt_init(byte screen_width)
{
    byte colorset = 0x00;
    struct HiResTextScreenInit init;

    // Define a `HiResTextScreenInit` object:
    // if (screen_width == 42)
    // {
    //     init =
    //         {
    //             42,                 /* characters per row */
    //             writeCharAt_42cols, /* must be consistent with previous field */
    //             (byte *)*(byte *)0x00BC << 8,
    //             TRUE,          /* redirects printf() to the 42x24 text screen */
    //             (word *)0x112, /* pointer to a 60 Hz async counter (Color Basic's TIMER) */
    //             0,             /* default cursor blinking rate */
    //             NULL,          /* use inkey(), i.e., Color Basic's INKEY$ */
    //             NULL,          /* no sound on '\a' */
    //         };
    // }
    // else
    // {
    //     init =
    //         {
    //             51,                 /* characters per row */
    //             writeCharAt_51cols, /* must be consistent with previous field */
    //             (byte *)*(byte *)0x00BC << 8,
    //             TRUE,          /* redirects printf() to the 51x24 text screen */
    //             (word *)0x112, /* pointer to a 60 Hz async counter (Color Basic's TIMER) */
    //             0,             /* default cursor blinking rate */
    //             NULL,          /* use inkey(), i.e., Color Basic's INKEY$ */
    //             NULL,          /* no sound on '\a' */
    //         };
    // }

    // width(32);                               /* PMODE graphics will only appear from 32x16 (does nothing on CoCo 1&2) */
    // pmode(4, (byte *)init.textScreenBuffer); /* hires text mode */
    // pcls(255);
    // screen(1, colorset);
    // initHiResTextScreen(&init);
}

void hirestxt_exit(void)
{
    closeHiResTextScreen();
    pmode(0, 0);
    screen(0, 0);
    cls(255);
}

bool in_hires_mode()
{
    return (textMode == 42 || textMode == 51);
}

void set_text_width(byte screen_width)
{
    if (screen_width == 32)
    {
        // Unset hires mode
        if (in_hires_mode)
        {
            hirestxt_exit();
        }
        width(screen_width);
        textMode = screen_width;
    }
    else if (screen_width == 42 || screen_width == 51)
    {
        hirestxt_init(screen_width);
        textMode = screen_width;
    }
    else // 40 or 80
    {
        if (isCoCo3)
        {
            width(screen_width);
            textMode = screen_width;
        }
    }
}

byte text_width_menu(void)
{
    unsigned char widths[5] = {32, 42, 51, 40, 80};
    unsigned char menu_line;
    char tmp_str[3];
    byte max_choice;
    byte choice = 0;

    if (isCoCo3)
    {
        max_choice = 4;
    }
    else
    {
        max_choice = 2;
    }

    if (textMode == 32)
    {
        menu_line = 15;
        locate(0, menu_line - 1);
        printf("l/r TO SELECT, break TO CANCEL");
    }
    else
    {
        menu_line = 23;
        locate(0, menu_line -1);
        print_lowercase_as_reverse("l/r TO SELECT, break TO CANCEL");
    }

    locate(0, menu_line);
    printf("                               ");
    for (int i = 0; i <= max_choice; i++)
    {
        if (i > 2 && !isCoCo3)
        {
            break;
        }

        if (textMode == widths[i])
        {
            itoa((int) widths[i], tmp_str, 10);
            print_reverse(tmp_str);
            putchar(' ');
            choice = i;
        }
        else
        {
            printf("%d ", widths[i]);
        }

    }

    while (true)
    {

        switch (waitkey(false))
        {
        case ARROW_LEFT:
            locate(choice * 3, menu_line);
            printf("%d ", widths[choice]);
            choice--;
            if (choice < 0)
            {
                choice = 0;
            }
            locate(choice * 3, menu_line);
            itoa((int) widths[choice], tmp_str, 10);
            print_reverse(tmp_str);
            putchar(' ');
            break;
        case ARROW_RIGHT:
            choice++;
            locate(choice * 3, menu_line);
            printf("%d ", widths[choice]);
            if (choice > max_choice)
            {
                choice = max_choice;
            }
            locate(choice * 3, menu_line);
            itoa((int) widths[choice], tmp_str, 10);
            print_reverse(tmp_str);
            putchar(' ');
            break;
        case ENTER:
            return widths[choice];
            break;
        case BREAK:
            return (unsigned char) WIDTH_CANCEL;
            break;
        }
    }
}
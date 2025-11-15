#include <cmoc.h>
#include <coco.h>
#include "fujinet-fuji.h"
#include "globals.h"
#include "bar.h"
#include "cocotext.h"

/**
 * @brief Temp space for strupr(s) output, so original strings doesn't get changed.
 */
char uppercase_tmp[1536];

void reverse(bool onoff)
{
    if (textMode != 32) // Nothing to do for 32
    {
        if (onoff)
        {
            attr(FG_WHITE, BG_BLUE, FALSE, FALSE);
        }
        else
        {
            attr(FG_BLACK, BG_GREEN, FALSE, FALSE);
        }
    }
}

void print_lowercase_as_reverse(const char *text)
{
    int len = strlen(text);

    if (textMode == 32)
    {
        putstr(text, len);
    }
    else // CoCO3 40/80 mode
    {

        for (int i = 0; i < len; i++)
        {
            if (text[i] >= 'a' && text[i] <= 'z')
            {
                reverse(1);
                putchar(text[i] - 32);
            }
            else
            {
                reverse(0);
                putchar(text[i]);
            }
        }
        reverse(0);
    }
}

void print_reverse(byte x, byte y, const char *text)
{
    byte *sp;
    int o = 0;
    int len = strlen(text);

    locate(x,y);

    if (textMode == 32)
    {
        putstr(text, len);
        sp = (byte *)SCREEN_RAM_TOP_32 + (int)((int)y * 32) + x;

        for (int i = 0; i < len; i++)
        {
            // Only change non-semigraphics characters
            if (*sp < 0x80)
                *sp ^= 0x40;
            sp++;
        }
    }
    else
    {
        reverse(1);
        putstr(text, len);
        reverse(0);
    }
}

/**
 * @brief Return uppercase string without modifying original
 */
char *screen_upper(char *s)
{
    memset(uppercase_tmp,0,sizeof(uppercase_tmp));
    strcpy(uppercase_tmp,s);

    return strupr(uppercase_tmp);
}

void set_text_width(byte screen_width)
{
    width(screen_width);

    textMode = screen_width;
}

void width_highlight(int i, byte y, byte width, bool highlight)
{
    char buf[10];
    sprintf(buf, "%d", (int) width);

    if (highlight)
    {
        print_reverse((byte) i * 3, y, buf);
    }
    else
    {
        locate((byte) i * 3, y);
        printf(buf);
    }
}

byte text_width_menu(void)
{
    unsigned char widths[3] = {32, 40, 80};
    unsigned char menu_line;
    byte max_choice;
    int choice = 0;

    max_choice = 2;
    menu_line = (textMode == 32) ? 15 : 23;

    locate(0, menu_line -1);
    print_lowercase_as_reverse("l/r TO SELECT, break TO CANCEL");
    locate(0, menu_line);
    printf("                               ");
    locate(0, menu_line);
    for (byte i = 0; i < max_choice +1; i++)
    {
        if (widths[i] == textMode)
        {
            choice = i;
            width_highlight(i, menu_line, widths[i], true);
        }
        else
        {
            width_highlight(i, menu_line, widths[i], false);
        }
    }

    while (true)
    {
        // Move the cursor to the end of the last line
        locate(textMode - 1, menu_line);
        switch (waitkey(false))
        {
        case ARROW_LEFT:
            width_highlight(choice, menu_line, widths[choice], false);
            choice--;
            if (choice < 0)
            {
                choice = 0;
            }
            width_highlight(choice, menu_line, widths[choice], true);
            break;
        case ARROW_RIGHT:
            width_highlight(choice, menu_line, widths[choice], false);
            choice++;
            if (choice > max_choice)
            {
                choice = max_choice;
            }
            width_highlight(choice, menu_line, widths[choice], true);
            break;
        case ENTER:
            return widths[choice];
            break;
        case BREAK:
            return (unsigned char)WIDTH_CANCEL;
            break;
        }
    }
}
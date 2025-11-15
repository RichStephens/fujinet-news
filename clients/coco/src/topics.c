/**
 * @brief   News Reader
 * @author  Thomas Cherryhomes
 * @email   thom dot cherryhomes at gmail dot com
 * @license gpl v. 3, see LICENSE for details.
 * @verbose Topics menu
 */

#include <cmoc.h>
#include <coco.h>
#include "globals.h"
#include "topics.h"
#include "bar.h"
#include "cocotext.h"

#define MENU_Y_32 3
#define MENU_Y_40_80 8

TopicState topicState = TOPICS_RESET;

enum _selected_topic
{
    TOP_STORIES = 0,
    WORLD_NEWS,
    BUSINESS,
    SCIENCE,
    TECHNOLOGY,
    HEALTH,
    ENTERTAINMENT,
    POLITICS,
    SPORTS
} selectedTopic = TOP_STORIES;

int topicChanged = 0; // flag for updating selection bar

/**
 * @brief Reset topics menu 
 */
TopicState topics_reset(void)
{
    selectedTopic = TOP_STORIES;
    topicChanged = 1;
    return TOPICS_DISPLAY;
}

TopicState topics_display(void)
{
    if (textMode==32)
    {
        cls(3);
        locate(0, 0);
        printf("      FUJINET  NEWS TOPICS      ");
        shadow(1, 0xA0);
        bar(0);

        locate(0, MENU_Y_32);

        printf(" \x99 TOP STORIES\n");
        printf(" \x9F WORLD NEWS\n");
        printf(" \x8F BUSINESS\n");
        printf(" \xAF SCIENCE\n");
        printf(" \xBF TECHNOLOGY\n");
        printf(" \xCF HEALTH\n");
        printf(" \xDF ENTERTAINMENT\n");
        printf(" \xEF POLITICS\n");
        printf(" \xFF SPORTS\n");

        shadow(0x0C, 0xA0);

        if (isCoCo3)
        {
            locate(0, 14);
            print_lowercase_as_reverse("w TO CHANGE SCREEN WIDTH");
        }
        locate(0, 15);
        printf("SELECT WITH ARROWS, THEN ENTER.");
    }
    else
    {
        cls(1);
        locate(0, 0);
        switch (textMode)
        {
        case 40:
            hd_bar(0, FG_WHITE, BG_BLUE, "          FUJINET  NEWS TOPICS");
            break;
        case 80:
            hd_bar(0, FG_WHITE, BG_BLUE, "                              FUJINET  NEWS TOPICS");
            break;
        }

        locate(0, MENU_Y_40_80 - 1);

        for (byte i =0; i < 9; i++)
        { 
            hd_bar(i + MENU_Y_40_80, FG_BLACK, BG_GREEN, (char *)topicStrings[i]);
        }

        if (isCoCo3)
        {
            locate(0, 22);
            print_lowercase_as_reverse("w TO CHANGE SCREEN WIDTH");
        }
        hd_bar(23, FG_BLACK, BG_GREEN, "SELECT WITH ARROWS, THEN ENTER.");
    }

    return TOPICS_MENU;
}

void select_screen_width(void)
{
    byte width_return = text_width_menu();

    if (width_return != WIDTH_CANCEL && width_return != textMode)
    {
        set_text_width(width_return);
    }
}

TopicState topics_menu()
{
    if (topicChanged)
    {
        topicChanged = 0;
        if (textMode == 32)
        {
            bar(selectedTopic + MENU_Y_32);
            locate(15, textMode-1);
        }
        else
        {
            hd_bar((byte) selectedTopic + MENU_Y_40_80, FG_WHITE, BG_BLUE, topicStrings[selectedTopic]);
            locate(23, textMode-1);
        }
    }

    while (true)
    {
        switch (waitkey(false))
        {
        case ARROW_UP:
            return TOPICS_UP;
        case ARROW_DOWN:
            return TOPICS_DOWN;
        case ENTER:
            return TOPICS_SELECT;
        case BREAK:
            return TOPICS_EXIT;
        case 'W':
        case 'w':
            if (isCoCo3)
            {
                select_screen_width();
                return TOPICS_RESET;
            }
        }
    }
}

TopicState topics_up()
{
    if (selectedTopic > TOP_STORIES)
    {
        if (textMode == 32)
        {
            bar(selectedTopic + MENU_Y_32);
            locate(15, textMode-1);
        }
        else
        {
            hd_bar((byte) selectedTopic + MENU_Y_40_80, FG_BLACK, BG_GREEN, topicStrings[selectedTopic]);
            locate(23, textMode-1);
        }

        selectedTopic--;
        topicChanged = 1;
    }

    return TOPICS_MENU;
}

TopicState topics_down()
{
    if (selectedTopic < SPORTS)
    {
        if (textMode == 32)
        {
            bar(selectedTopic + MENU_Y_32);
            locate(15, textMode-1);
        }
        else
        {
            hd_bar((byte) selectedTopic + MENU_Y_40_80, FG_BLACK, BG_GREEN, topicStrings[selectedTopic]);
            locate(23, textMode-1);
        }

        selectedTopic++;
        topicChanged=1;
    }

    return TOPICS_MENU;
}

State topics(void)
{
    topicState=TOPICS_RESET;
    
    while (state == TOPICS)
    {
        switch(topicState)
        {
        case TOPICS_RESET:
            topicState = topics_reset(); 
            break;
        case TOPICS_DISPLAY:
            topicState = topics_display();
            break;
        case TOPICS_MENU:
            topicState = topics_menu();
            break;
        case TOPICS_UP:
            topicState = topics_up();
            break;
        case TOPICS_DOWN:
            topicState = topics_down();
            break;
        case TOPICS_SELECT:
            return ARTICLES;
        case TOPICS_EXIT:
            return BYE;
        }
    }

    return BYE;
}

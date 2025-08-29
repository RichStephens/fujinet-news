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

        locate(0, 15);
        printf("SELECT WITH ARROWS, THEN ENTER.");
    }
    else
    {
        cls(1);
        locate(0, 0);
        if (textMode == 40)
        {
            hd_bar(0, FG_BLACK, BG_GREEN, "          FUJINET  NEWS TOPICS");
            //printf("********* FUJINET  NEWS TOPICS *********");
        }
        else
        {
            hd_bar(0, FG_BLACK, BG_GREEN, "                              FUJINET  NEWS TOPICS");
            //printf("***************************** FUJINET  NEWS TOPICS *****************************");
        } 

        locate(0, MENU_Y_40_80 - 1);

        //printf("*******************\n");
        for (int i =0; i < 9; i++)
        { 
            //printf("%s\n", topicStrings[i]); 
            hd_bar(i + MENU_Y_40_80, FG_BLACK, BG_GREEN, (char *)topicStrings[i]);
        }
        //printf("*******************\n");

        //locate(0, 23);
        //printf("SELECT WITH ARROWS, THEN ENTER.");
        hd_bar(23, FG_BLACK, BG_GREEN, "SELECT WITH ARROWS, THEN ENTER.");
    }

    return TOPICS_MENU;
}

TopicState topics_menu()
{
    if (topicChanged)
    {
        topicChanged = 0;
        if (textMode == 32)
        {
            bar(selectedTopic + MENU_Y_32);
        }
        else
        {
            hd_bar(selectedTopic + MENU_Y_40_80, FG_GREEN, BG_BLUE, topicStrings[selectedTopic]);
            //underline(selectedTopic + MENU_Y_40_80, topicStrings[selectedTopic], true);
        }
    }

    switch(waitkey(false))
    {
    case ARROW_UP:
        return TOPICS_UP;
    case ARROW_DOWN:
        return TOPICS_DOWN;
    case ENTER:
        return TOPICS_SELECT;
    case BREAK:
        return TOPICS_EXIT;
    }       
}

TopicState topics_up()
{
    if (selectedTopic > TOP_STORIES)
    {
        if (textMode == 32)
        {
            bar(selectedTopic + MENU_Y_32);
        }
        else
        {
            hd_bar(selectedTopic + MENU_Y_40_80, FG_BLACK, BG_GREEN, topicStrings[selectedTopic]);
            //underline(selectedTopic + MENU_Y_40_80, topicStrings[selectedTopic], false);
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
        }
        else
        {
            hd_bar(selectedTopic + MENU_Y_40_80, FG_BLACK, BG_GREEN, topicStrings[selectedTopic]);
            //underline(selectedTopic + MENU_Y_40_80, topicStrings[selectedTopic], false);
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

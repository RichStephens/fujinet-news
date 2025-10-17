/**
 * @brief   FujiNet News Reader for MS-DOS
 * @author  Thomas Cherryhomes
 * @email   thom dot cherryhomes at gmail dot com
 * @license gpl v. 3, see LICENSE for details.
 * @verbose topics module
 */

#include "topics.h"
#include "screen.h"
#include "select.h"

extern unsigned char selected_topic;

const char *topic_titles[] =
    {
        "\xF0 TOP STORIES",
        "\xF0 WORLD NEWS",
        "\xF0 BUSINESS",
        "\xF0 SCIENCE",
        "\xF0 TECHNOLOGY",
        "\xF0 HEALTH",
        "\xF0 ENTERTAINMENT",
        "\xF0 POLITICS",
        "\xF0 SPORTS"
    };

void topics(void)
{
    int i=0;
    
    _fgColor=WHITE;
    _bgColor=BLUE;
    clrscr();

    if (max_cols > 40)
        column_offset=20;

    cputs(13,7,"SELECT A TOPIC:");
    cputs(5,23,"\x18\x19 MOVE SELECTION  \x11\xD9 SELECT");
    _fgColor=BLACK;
    _bgColor=CYAN;
    
    box(2,2,36,5);
    shadow(2,2,36,5);
    cputs(10,3,"#FUJINET NEWS READER");

    _fgColor=WHITE;
    _bgColor=MAGENTA;
    box(2,9,36,20);
    shadow(2,9,36,20);

    for (i=0;i<9;i++)
        cputs(4,10+i,topic_titles[i]);

    selected_articles_page = 1;
    
    state=select(2,10,37,1,9,selected_topic,ARTICLES,EXIT,IGNORE,IGNORE,&selected_topic);
}

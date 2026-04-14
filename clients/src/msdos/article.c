/**
 * @brief   FujiNet News Reader for MS-DOS
 * @author  Thomas Cherryhomes
 * @email   thom dot cherryhomes at gmail dot com
 * @license gpl v. 3, see LICENSE for details.
 * @verbose article module
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "screen.h"
#include "globals.h"
#include "select.h"
#include "fetch.h"

const char *fetching_page="FETCHING PAGE, PLEASE WAIT...";

char article_data[2048];

static char *article_title    = NULL;
static char *article_date     = NULL;
static char *article_source   = NULL;
static char *article_page     = NULL;
static char *article_pageData = NULL;

static void article_parse(void)
{
    article_title    = strtok(article_data,"\n");
    article_date     = strtok(NULL,"\n");
    article_source   = strtok(NULL,"\n");
    article_page     = strtok(NULL,"\n");
    article_pageData = strtok(NULL,"\0");

    {
        char *slash = strchr(article_page, '/');
        if (slash)
            selected_article_page_max = atoi(slash + 1);
    }
}

void article_display(void)
{
    column_offset = 0;

    _fgColor=WHITE;
    _bgColor=BLUE;
    clrscr();

    _fgColor=BLACK;
    _bgColor=GREEN;
    box(0,0,max_cols-1,3);
    cputs(0,0,article_title);
    cputs(max_cols-strlen(article_page)-1,2,article_page);

    _fgColor=BLACK;
    _bgColor=CYAN;
    box(0,21,max_cols-1,24);
    if (max_cols > 40)
        cputs(0,21,"       [PgUp]/[PgDn] CHANGE PAGE       [I/F1] INFO             [ESC] QUIT");
    else
        cputs(0,21,"[PgUp/Dn] PAGE [I/F1] INFO [ESC] QUIT");

    _fgColor=WHITE;
    _bgColor=BLUE;
    cputs(0,3,article_pageData);
}

void article_info(void)
{
    unsigned char label_col = (max_cols - 27) / 2;
    unsigned char value_col = label_col + 8;

    _fgColor=WHITE;
    _bgColor=BLUE;
    clrscr();

    _fgColor=BLACK;
    _bgColor=GREEN;
    box(0,0,max_cols-1,3);
    cputs(0,0,article_title);
    cputs(max_cols-strlen(article_page)-1,2,article_page);

    _fgColor=WHITE;
    _bgColor=BLUE;
    cputs(label_col, 5, "Date:   ");
    cputs(value_col, 5, article_date);
    cputs(label_col, 7, "Source: ");
    cputs(value_col, 7, article_source);

    _fgColor=BLACK;
    _bgColor=CYAN;
    box(0,21,max_cols-1,24);
    {
        const char *msg = "< Press any key to continue >";
        cputs((max_cols - strlen(msg)) / 2, 21, msg);
    }

    getk();
}

void article(void)
{
    memset(url,0,sizeof(url));
    memset(article_data,0,sizeof(article_data));

    _fgColor=WHITE;
    _bgColor=BLUE;
    clrscr();

    cputs((max_cols/2)-strlen(fetching_page)/2,11,fetching_page);

    sprintf(url,"%s?t=lf&ps=%ux16&l=5&p=%u&a=%lu",urlBase,max_cols-2,selected_article_page,selected_article);

    fetch(url,article_data);

    article_parse();
    article_display();

    while (1)
    {
        int k = getk();
        switch (k)
        {
        case 0x4900:
            if (selected_article_page > 1)
                selected_article_page--;
            return;
        case 0x5100:
            if (selected_article_page < selected_article_page_max)
                selected_article_page++;
            return;
        case 0x3B00: // F1
            article_info();
            article_display();
            break;
        default:
            if ((k & 0xFF) == 'i' || (k & 0xFF) == 'I') {
                article_info();
                article_display();
            }
            break;
        case 0x11b:
            state=ARTICLES;
            return;
        }
    }
}

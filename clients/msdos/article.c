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

void article_display(void)
{
    char *p = strtok(article_data,"\n");
    char *date = strtok(NULL,"\n");
    char *source = strtok(NULL,"\n");
    char *page = strtok(NULL,"\n");
    char *pageData = strtok(NULL,"\0");

    column_offset = 0;
    
    _fgColor=WHITE;
    _bgColor=BLUE;
    clrscr();
    
    _fgColor=BLACK;
    _bgColor=GREEN;
    box(0,0,max_cols-1,3);
    cputs(0,0,p);


    _fgColor=BLACK;
    _bgColor=CYAN;
    box(0,21,max_cols-1,24);
    cputs(0,21,"       [PgUp]/[PgDn] CHANGE PAGE         [F1] INFO                [ESC] QUIT");

    _fgColor=WHITE;
    _bgColor=BLUE;
    cputs(max_cols-strlen(page)-1,3,page);
    cputs(0,3,pageData);
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

    article_display();

    switch (getk())
    {
    case 0x4900:
        selected_article_page--;
        return;
    case 0x5100:
        selected_article_page++;
        return;
    case 0x11b:
        state=ARTICLES;
        return;
    }
}

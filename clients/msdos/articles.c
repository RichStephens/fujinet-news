/**
 * @brief   FujiNet News Reader for MS-DOS
 * @author  Thomas Cherryhomes
 * @email   thom dot cherryhomes at gmail dot com
 * @license gpl v. 3, see LICENSE for details.
 * @verbose articles module
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include "screen.h"
#include "globals.h"
#include "select.h"
#include "fetch.h"

#define ARTICLES_PER_PAGE 5

static const char *category_names[] =
{
    "top",
    "world",
    "business",
    "science",
    "technology",
    "health",
    "entertainment",
    "politics",
    "sports"
};

extern const char *topic_titles[];

const char *fetching_articles="FETCHING ARTICLES, PLEASE WAIT...";

unsigned char articles_data[2048];

void articles_pu(void)
{
    if (selected_articles_page>1)
        selected_articles_page--;

    state=ARTICLES;
}

void articles_pd(void)
{
    selected_articles_page++;

    state=ARTICLES;
}

void articles(void)
{
    int articles_on_this_page=0;
    char *page;
    char *p;
    unsigned long article_ids[ARTICLES_PER_PAGE]={0,0,0,0,0};
    unsigned char item=0;
    union REGS r;
    struct SREGS sr;
    
    _fgColor = WHITE;
    _bgColor = BLUE;
        
    column_offset = 0;
    selected_article_page=1;
    
    clrscr();
    cputs((max_cols / 2) - (strlen(fetching_articles)/2),11,fetching_articles);

    memset(url,0x00,sizeof(url));
    memset(articles_data,0x00,sizeof(articles_data));
    sprintf(url,"%s?t=lf&ps=%ux20&l=5&p=%u&c=%s",urlBase,max_cols-2,selected_articles_page,category_names[selected_topic]);

    fetch(url,articles_data);

    page = strtok(articles_data,"\n");
    p = strtok(NULL,"|");
    
    clrscr();

    // Title box
    _fgColor = WHITE;
    _bgColor = BLACK;
    box(0,0,max_cols-1,0);
    cputs((max_cols / 2) - (strlen(topic_titles[selected_topic]) / 2),0,topic_titles[selected_topic]);
    cputs(max_cols-strlen(page)-1,0,page);

    // Keys
    cputs(0,22,"[\x18\x19] CHOOSE   [PgUp/PgDn] CHANGE PAGE   [\x11\xD9] SELECT  [ESC] TOPICS");
    
    while (p != NULL)
    {
        char title[240];
        
        // Article box
        _fgColor = WHITE;
        _bgColor = BLUE;
        box(0,articles_on_this_page*4+1,max_cols-1,articles_on_this_page*4+1);
        _fgColor = BLACK;
        _bgColor = CYAN;
        box(0,articles_on_this_page*4+1+1,max_cols-1,articles_on_this_page*4+4+1);
        
        // Timestamp
        article_ids[articles_on_this_page] = atol(p);
        p=strtok(NULL,"|");
        cputs(max_cols-19,articles_on_this_page*4+1,p);

        /* // Title */
        memset(title,0,sizeof(title));
        p=strtok(NULL,"\n");
        strncpy(title,p,max_cols*3);
        cputs(0,articles_on_this_page*4+1+1,title);
        
        p = strtok(NULL, "|");

        articles_on_this_page++;
    }

    state=select(0,1,max_cols,4,articles_on_this_page,0,ARTICLE,TOPICS,ARTICLES_PU,ARTICLES_PD,&item);
    selected_article=article_ids[item];
}

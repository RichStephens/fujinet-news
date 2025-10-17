/**
 * @brief   FujiNet News Reader for MS-DOS
 * @author  Thomas Cherryhomes
 * @email   thom dot cherryhomes at gmail dot com
 * @license gpl v. 3, see LICENSE for details.
 */

#include <stdlib.h>
#include "screen.h"
#include "topics.h"
#include "articles.h"
#include "article.h"
#include "globals.h"

void main(void)
{
    mode(-1);

    while(1)
    {
        switch(state)
        {
        case TOPICS:
            topics();
            break;
        case ARTICLES:
            articles();
            break;
        case ARTICLES_PU:
            articles_pu();
            break;
        case ARTICLES_PD:
            articles_pd();
            break;
        case ARTICLE:
            article();
            break;
        case EXIT:
            clrscr();
            exit(0);
        }
    }    
}

/**
 * @brief   News Reader
 * @author  Thomas Cherryhomes
 * @email   thom dot cherryhomes at gmail dot com
 * @license gpl v. 3, see LICENSE for details.
 * @verbose Main Program
 */

#include <cmoc.h>
#include <coco.h>
#include "globals.h"
#include "topics.h"
#include "articles.h"
#include "article.h"

void halt(void)
{
    while(1);
}

void init(void)
{
    initCoCoSupport();
    textMode = getTextMode();
    rgb();
}

int main(void)
{
    screen(0,1);
    cls(1);
    printf("Calling init...");
    waitkey(0);
    init();
    printf("Setting screen and clearing..");
    waitkey(0);
    screen(0,1);
    cls(1);
    while(1)
    {
        switch(state)
        {
        case TOPICS:
            printf("Calling topics()");
            waitkey(0);
            state=topics();
            break;
        case ARTICLES:
            state=articles();
            break;
        case ARTICLE:
            state=article();
            break;
        case HALT:
            halt();
            break;
        case BYE:
            break;
        }

        if (state==BYE)
            break;
    }

    cls(1);
    return 0;
}

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
#include "cocotext.h"

void init(void)
{
    initCoCoSupport();
    textMode = getTextMode();
    if (textMode == 32)
    {
        // Use hires text mode by default 
        set_text_width(41);
    }
}

int main(void)
{
    clear_screen(1);
    init();

    while(1)
    {
        switch(state)
        {
        case TOPICS:
            state=topics();
            break;
        case ARTICLES:
            state=articles();
            break;
        case ARTICLE:
            state=article();
            break;
        case BYE:
            break;
        }

        if (state==BYE)
            break;
    }

    hirestxt_close();
    clear_screen(1);
    return 0;
}

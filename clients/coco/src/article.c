/**
 * @brief   News Reader
 * @author  Thomas Cherryhomes
 * @email   thom dot cherryhomes at gmail dot com
 * @license gpl v. 3, see LICENSE for details.
 * @verbose Article display
 */

#include <cmoc.h>
#include <coco.h>
#include "globals.h"
#include "article.h"
#include "net.h"
#include "bar.h"

/**
 * @brief Temp space for strupr(s) output, so original strings doesn't get changed.
 */
char uppercase_tmp[1536]; 

/**
 * @brief the currently selected article ID 
 */
long article_id;

/**
 * @brief current article page
 */
int article_page=1;

/**
 * @brief Article Page buffer 
 */
char article_page_buffer[2048];

/**
 * @brief Article metadata
 */
static char *title = NULL;
static char *date = NULL;
static char *source = NULL;
static char *page = NULL;
static char *pageData = NULL;
static int page_max = 0;
static byte menu_line = 15; // line for menu display

/**
 * @brief current state for article view
 */
static ArticleState articleState;

/**
 * @brief the Article URL
 */
static char article_url[256];

static char menu_line_buffer[81];

extern char *articles_display_headline(char *s);

/**
 * @brief Initial entrypoint, reset local state
 */
ArticleState article_reset(void)
{
    article_page = 1;
    if (textMode == 32)
    {
        menu_line = 15;
    }
    else
    {
        menu_line = 22;
    }

    return ARTICLE_FETCH;
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

/**
 * @brief Fetch current article page
 */
ArticleState article_fetch(void)
{
    NetworkStatus ns;
    char *p = NULL;
    char tmp[8];
    
    memset(article_url,0,sizeof(article_url));
    memset(article_page_buffer,0,sizeof(article_page_buffer));

    char fetching_buf[81] = "FETCHING PAGE, PLEASE WAIT.";

    if (textMode == 32)
    {
        cls(3);
        printf("%s", fetching_buf);
    }
    else
    {
        cls(1);
        hd_bar(0, FG_BLACK, BG_GREEN, fetching_buf);
    }

    int rows = (textMode == 32) ? 10 : 17;

    sprintf(article_url,"%s?t=lf&ps=%dx%d&p=%u&a=%lu",
            urlBase,
            textMode - 1,
            rows,
            article_page,
            article_id);

    net_open(0,12,0,article_url);
    net_status(0,&ns);
    unsigned int buf_offset = 0;
    while(ns.error == 1 && ns.bytesWaiting > 0)
    {   net_read(0,(byte *)&article_page_buffer[0+buf_offset],ns.bytesWaiting);
        buf_offset += ns.bytesWaiting;
        net_status(0, &ns);
        
        strcat(fetching_buf, ".");

        if (textMode == 32)
        {
            locate(0,0);
            printf("%s", fetching_buf);
        }
        else
        {
            cls(1);
            hd_bar(0, FG_BLACK, BG_GREEN, fetching_buf);
        }
    }
    net_close(0);

    title = strtok(article_page_buffer,"\n");
    date = strtok(NULL,"\n");
    source = strtok(NULL,"\n");
    page = strtok(NULL,"\n");
    pageData = strtok(NULL,"\0");

    // copy, parse page counter so we can get max # of pages.
    strcpy(tmp,page);
    p = strtok(tmp,"/"); // skip over current page
    p = strtok(NULL,"\0"); // get last # as max pages.
    page_max = atoi(p); // set it.
    
    return ARTICLE_DISPLAY;
}

/**
 * @brief Display current article page
 */
ArticleState article_display(void)
{
    if (textMode == 32)
    {
        cls(3);
        printf("%-96s\n", articles_display_headline(title));
        bar(0);
        bar(1);
        bar(2);
        shadow(3, 0xa0);
        printf("%s", screen_upper(pageData));
        shadow(14, 0xa0);
    }
    else
    {
        cls(1);

        if (textMode == 40)
        {
            multiline_hd_bar(0, FG_WHITE, BG_BLUE, 3, articles_display_headline(title));
            printf("\n\n");
        }
        else
        {
            multiline_hd_bar(0, FG_WHITE, BG_BLUE, 2, articles_display_headline(title));
            printf("\n\n");
        }
        printf("%s", pageData);
    }

    return ARTICLE_MENU;
}

/**
 * @brief Format a menu line to fit in given width
 * @param page Current page string
 * @param text Menu text
 * @param width Width of the line
 * @return pointer to static buffer containing formatted line
 */
char *format_menu_line(const char *page, const char *menu_text, int width) 
{
    memset(menu_line_buffer, 0, sizeof(menu_line_buffer));

    // Start with spaces
    memset(menu_line_buffer, ' ', width);
    menu_line_buffer[width] = '\0';

    // Place page string on the left
    int page_len = strlen(page);
    if (page_len > width) page_len = width;
    memcpy(menu_line_buffer, page, page_len);

    // Center the menu text
    int menu_len = strlen(menu_text);
    if (menu_len > width) menu_len = width;

    int menu_start = (width - menu_len) / 2;
    memcpy(menu_line_buffer + menu_start, menu_text, menu_len);

    return menu_line_buffer;
}

/**
 * @brief Handle article menu
 */
ArticleState article_menu(void)
{
    locate(0, menu_line);

    if (textMode == 32)
    {
        printf("      up/dn iNFO break ARTICLES");
        locate(0, menu_line);
        printf("%s", page);
    }
    else
    {
        if (textMode == 40)
        {
            print_lowercase_as_reverse(format_menu_line(page, "up/dn iNFO  break ARTICLES", textMode));
        }
        else
        {
            print_lowercase_as_reverse(format_menu_line(page, "up/down iNFO break ARTICLES", textMode));
        }
    }

    locate(textMode -1,menu_line);

    switch(waitkey(1))
    {
    case BREAK:
        return ARTICLE_EXIT;
    case ARROW_UP:
        return ARTICLE_PREV_PAGE;
    case ARROW_DOWN:
        return ARTICLE_NEXT_PAGE;
    case 'I':
    case 'i':
        return ARTICLE_INFO;
    }
    
    return ARTICLE_MENU;
}

/**
 * @brief Go to next page 
 */
ArticleState article_next_page(void)
{
    article_page++;

    if (article_page > page_max)
        return ARTICLE_MENU;
    
    return ARTICLE_FETCH;
}

/**
 * @brief Go to previous page
 */
ArticleState article_prev_page(void)
{
    if (article_page == 1)
        return ARTICLE_MENU;

    article_page--;
    
    return ARTICLE_FETCH;
}

/**
 * @brief Show Article Metadata
 */
ArticleState article_info(void)
{
    if (textMode == 32)
    {
        cls(6);
        printf("%-96s", articles_display_headline(title));
        bar(0);
        bar(1);
        bar(2);
        shadow(3, 0xD0);

        locate(0, 4);
        printf("%10s%-22s", "date:", screen_upper(date));
        printf("%10s%-22s", "source:", screen_upper(source));
        
        shadow(6,0xD0);
        locate(0, menu_line);
        printf("    PRESS ANY KEY TO CONTINUE");
    }
    else
    {
        cls(1);

        if (textMode == 40)
        {
            multiline_hd_bar(0, FG_WHITE, BG_BLUE, 3, articles_display_headline(title));
            locate (0, 5);
            printf("%13s%s\n", "Date:", date);
            printf("%13s%s", "Source:", source);
        }
        else
        {
            multiline_hd_bar(0, FG_WHITE, BG_BLUE, 2, articles_display_headline(title));
            locate(0,4);
            printf("%33s%s\n", "Date:", date);
            printf("%33s%s", "Source:", source);
        }


        locate(0, menu_line);

        if (textMode == 40)
        {
            printf("      <Press any key to continue>");
        }
        else
        {
            printf("                            <Press any key to continue>");
        }
    }


    waitkey(1);
    
    return ARTICLE_DISPLAY;
}

/**
 * @brief Handle article state
 */
State article(void)
{
    articleState=ARTICLE_RESET;

    while (state == ARTICLE)
    {
        switch(articleState)
        {
        case ARTICLE_RESET:
            articleState = article_reset();
            break;
        case ARTICLE_FETCH:
            articleState = article_fetch();
            break;
        case ARTICLE_DISPLAY:
            articleState = article_display();
            break;
        case ARTICLE_MENU:
            articleState = article_menu();
            break;
        case ARTICLE_NEXT_PAGE:
            articleState = article_next_page();
            break;
        case ARTICLE_PREV_PAGE:
            articleState = article_prev_page();
            break;
        case ARTICLE_INFO:
            articleState = article_info();
            break;
        case ARTICLE_EXIT:
            return ARTICLES;            
        }
    }

    return ARTICLES;
}

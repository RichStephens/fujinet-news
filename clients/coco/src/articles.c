/**
 * @brief   FujiNet News Reader
 * @author  Thomas Cherryhomes
 * @email   thom dot cherryhomes at gmail dot com
 * @license gpl v. 3, see LICENSE for details.
 * @verbose Articles Display/Select
 */

#include <cmoc.h>
#include <coco.h>
#include "globals.h"
#include "articles.h"
#include "net.h"
#include "bar.h"

#define ARTICLES_PER_PAGE 3
#define MAX_ARTICLES_PER_PAGE 6

static ArticlesState articlesState = ARTICLES_RESET;
static int articles_page = 1;
static char articles_buffer[1024];
static char outs[161];
static char topic_line[81];
static char indicator_buffer[62];
int articles_per_page = ARTICLES_PER_PAGE;
byte menu_line = 14;
static int headline_length = 96;
static int articles_on_this_page = 0;
static char page[32];   // enough for "12345/12345" etc.
static char article_cursor_pos = 0;
static char article_cursor_pos_prev = 0;


extern long article_id;

struct Article {
    long id;             // use long for article ID (at least 32 bits)
    char timestamp[20];  // "YYYY-MM-DD HH:MM:SS"
    char headline[256];   // fixed size headline buffer
};

struct Article _articles[MAX_ARTICLES_PER_PAGE];

static const char *topic_titles[9] =
    {
        "TOP STORIES",
        "WORLD NEWS",
        "BUSINESS",
        "SCIENCE",
        "TECHNOLOGY",
        "HEALTH",
        "ENTERTAINMENT",
        "POLITICS",
        "SPORTS"
    };

const char *category_name_to_num(int c)
{
    switch(c)
    {
    case 0:
        return "top";
    case 1:
        return "world";
    case 2:
        return "business";
    case 3:
        return "science";
    case 4:
        return "technology";
    case 5:
        return "health";
    case 6:
        return "entertainment";
    case 7:
        return "politics";
    case 8:
        return "sports";
    }
};

const byte headline_locations_32[3] = {1, 5, 9};
const byte headline_locations_40 [5]= {1, 5, 9, 13, 17};
const byte headline_locations_80[6] = {1, 4, 7, 10, 13, 16};


/**
 * @brief Reset articles state
 */
ArticlesState articles_reset(void)
{
    articles_page = 1;
    article_cursor_pos = 0;
    article_cursor_pos_prev = 0;

    if (textMode == 32)
    {
        menu_line = 14;
        articles_per_page = 3;
        headline_length = 96;
    }
    else
    {
        menu_line = 22;
        if (textMode==40)
        {
            articles_per_page = 5;
            headline_length = 120;
        }
        else
        {
            articles_per_page = 6;
            headline_length = 160;
        }   
    }
    
    memset(_articles, 0, MAX_ARTICLES_PER_PAGE * sizeof(struct Article));

    return ARTICLES_FETCH;
}

/**
 * @brief Fetch articles for current page
 */
ArticlesState articles_fetch(void)
{
    char url[256];
    NetworkStatus ns;

    memset(articles_buffer,0,sizeof(articles_buffer));

    char fetching_buf[81] = "FETCHING ARTICLES, PLEASE WAIT.";

    if (textMode == 32)
    {
        cls(2);
        printf("%s", fetching_buf);
    }
    else
    {
        cls(1);
        hd_bar(0, FG_BLACK, BG_GREEN, fetching_buf);
    }

    int rows = (textMode==32) ? 13 : 21;
    
    // Set up URL
    sprintf(url,"%s?t=lf&ps=%dx%d&l=%u&p=%u&c=%s",
            urlBase,
            textMode,
            rows,
            articles_per_page,
            articles_page,
            category_name_to_num(selectedTopic));
    
    net_open(0,12,0,url);
    net_status(0,&ns);
    unsigned int buf_offset = 0;
    while(ns.error == 1 && ns.bytesWaiting > 0)
    {   net_read(0,(byte *)&articles_buffer[0+buf_offset],ns.bytesWaiting);
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

    return ARTICLES_DISPLAY;
}

/**
 * @brief Transform and display article headline
 * @param s headline string
 */
char *articles_display_headline(char *s)
{
    memset(outs,0,sizeof(outs));

    strncpy(outs,s,headline_length);

    // Ellipsize if needed.
    if (strlen(outs)==headline_length)
    {
        outs[headline_length-3]=outs[headline_length-2]=outs[headline_length-1]='.';
    }

    if (textMode==32)
    {
        return strupr(outs);
    }
    else
    {
        return outs;
    }   
}

char *format_topic_line(const char *page, const char *title, int width) {
    
    memset(topic_line, 0, sizeof(topic_line));

    // Start with spaces
    memset(topic_line, ' ', width);
    topic_line[width] = '\0';

    // Place page string on the left
    int page_len = strlen(page);
    if (page_len > width) page_len = width;
    memcpy(topic_line, page, page_len);

    // Center the title
    int title_len = strlen(title);
    if (title_len > width) title_len = width;

    int title_start = (width - title_len) / 2;
    memcpy(topic_line + title_start, title, title_len);

    return topic_line;
}


/**
 * @brief _articles[ the articles response
 * @param input API response string
 * @param articles caller-provided buffer of struct Article[]
 * @param capacity max number of articles that fit in the buffer
 * @return Number of artticles on the page if success, -1 on error
 */
int parse_articles_response(char *input, struct Article *articles, int capacity)
{
    char *line;
    int article_count;

    // initialize the entire articles array to zero
    memset(articles, 0, capacity * sizeof(struct Article));
    memset(page, 0, sizeof(page));

    if (input == NULL || *input == '\0')
    {
        return -1;
    }

    article_count = 0;

    line = input;
    while (line != NULL && *line != '\0')
    {
        // find the end of the current line
        char *next_line = strchr(line, '\n');
        if (next_line)
        {
            *next_line = '\0'; // terminate the current line
            next_line++;       // move to the start of next line
        }

        if (page[0] == '\0')
        {
            // first line is the page
            memset(page, 0, sizeof(page));
            strncpy(page, line, sizeof(page) - 1);
        }
        else
        {
            if (article_count < capacity && *line != '\0')
            {
                // manually split id|timestamp|headline
                char *first_pipe = strchr(line, '|');
                if (first_pipe)
                {
                    *first_pipe = '\0';
                    char *id_str = line;

                    char *second_pipe = strchr(first_pipe + 1, '|');
                    if (second_pipe)
                    {
                        *second_pipe = '\0';
                        char *ts = first_pipe + 1;
                        char *headline = second_pipe + 1;

                        struct Article *a = &articles[article_count];
                        a->id = strtol(id_str, NULL, 10);

                        memset(a->timestamp, 0, sizeof(a->timestamp));
                        strncpy(a->timestamp, ts, sizeof(a->timestamp) - 1);

                        memset(a->headline, 0, sizeof(a->headline));
                        strncpy(a->headline, headline, sizeof(a->headline) - 1);

                        article_count++;
                    }
                }
            }
        }

        line = next_line;
    }

    return article_count;
}

/**
 * @brief Display fetched articles
 */
ArticlesState articles_display(void)
{
    int i=0;

    locate(0,0);

    articles_on_this_page = parse_articles_response(articles_buffer, _articles, MAX_ARTICLES_PER_PAGE);

    if (articles_on_this_page > 0)
    {
        for (int i = 0; i < articles_on_this_page; i++)
        {
            if (textMode == 32)
            {
                locate(0, headline_locations_32[i] - 1);
                printf("%32s", _articles[i].timestamp);
                printf("%-96s", articles_display_headline(_articles[i].headline));
            }
            else
            {
                if (textMode == 40)
                {
                    locate(0, headline_locations_40[i] - 1);
                    printf("%40s", _articles[i].timestamp);
                    printf("%-120s", articles_display_headline(_articles[i].headline));
                }
                else
                {
                    locate(0, headline_locations_80[i] - 1);
                    printf("%80s", _articles[i].timestamp);
                    printf("%-160s", articles_display_headline(_articles[i].headline));
                }
            }
        }

        if (textMode == 32)
        {
            shadow(12, 0x90);
            locate(0, menu_line - 1);
            printf("%s", format_topic_line(page, topic_titles[selectedTopic], textMode));
            bar(13);

            bar(headline_locations_32[article_cursor_pos]);
            bar(headline_locations_32[article_cursor_pos] + 1);
            bar(headline_locations_32[article_cursor_pos] + 2);
        }
        else
        {
            hd_bar(menu_line - 1, FG_BLACK, BG_GREEN, format_topic_line(page, topic_titles[selectedTopic], textMode));
         
            if (textMode == 40)
            {
                locate(0, headline_locations_40[article_cursor_pos] - 1);
            }
            else
            {
                locate(0, headline_locations_80[article_cursor_pos] - 1);
            }

            memset(indicator_buffer, '*', textMode - 20);
            indicator_buffer[textMode - 20] = '\0';
            printf("%s", indicator_buffer);
        }
    }

    return ARTICLES_MENU;
}

/**
 * @brief Update articles menu bar
 */
void articles_bar(void)
{
    if (article_cursor_pos_prev != article_cursor_pos)
    {
        if (textMode == 32)
        {
            bar(headline_locations_32[article_cursor_pos_prev]);
            bar(headline_locations_32[article_cursor_pos_prev] + 1);
            bar(headline_locations_32[article_cursor_pos_prev] + 2);
            bar(headline_locations_32[article_cursor_pos]);
            bar(headline_locations_32[article_cursor_pos] + 1);
            bar(headline_locations_32[article_cursor_pos] + 2);
        }
        else
        {
            if (textMode==40)
            {
                locate(0, headline_locations_40[article_cursor_pos_prev]-1);        
            }
            else
            {
                locate(0, headline_locations_80[article_cursor_pos_prev]-1);
            }

            memset(indicator_buffer, ' ', textMode - 20);
            indicator_buffer[textMode - 20] = '\0';
            printf("%s", indicator_buffer);

            if (textMode==40)
            {
                locate(0, headline_locations_40[article_cursor_pos]-1);        
            }
            else
            {
                locate(0, headline_locations_80[article_cursor_pos]-1);
            }

            memset(indicator_buffer, '*', textMode - 20);
            indicator_buffer[textMode - 20] = '\0';
            printf("%s", indicator_buffer);
        }   
        article_cursor_pos_prev = article_cursor_pos;
    }
}

/**
 * @brief Handle menu for current article page
 */
ArticlesState articles_menu(void)
{

    locate(0,menu_line);
    if (textMode==32)
    {
        printf(" up/dn CHOOSE brk TOPICS\n");
        printf(" lf/rt PAGE enter VIEW gO TO PG");
    }
    else if (textMode == 40)
    {
        printf(" <UP/DN> CHOOSE  <BREAK> TOPICS\n");
        printf(" <LF/RT> PAGE <ENTER> VIEW <G>O TO PAGE");
    }
    else
    { 
        // Centered on 80 character screen
        printf("                        <UP/DOWN> CHOOSE  <BREAK> TOPICS\n");
        printf("                     <LEFT/RIGHT> PAGE    <ENTER> VIEW <G>O TO PAGE");
    }
    
    articles_bar();
    locate(textMode - 1, menu_line + 1);
    putchar(article_cursor_pos);

    switch (waitkey(false))
    {
    case BREAK:
        return ARTICLES_EXIT;
    case ARROW_UP:
        return ARTICLES_UP;
    case ARROW_DOWN:
        return ARTICLES_DOWN;
    case ENTER:
        return ARTICLES_SELECT;
    case ARROW_LEFT:
        return ARTICLES_PREV_PAGE;
    case ARROW_RIGHT:
        return ARTICLES_NEXT_PAGE;
    case 'G':
    case 'g':
        return ARTICLES_GOTO_PAGE;
    }

    return ARTICLES_MENU;
}

/**
 * @brief handle moving up, for current articles page
 */
ArticlesState articles_up(void)
{
    if (article_cursor_pos >  0)
    {
        articles_bar();
        article_cursor_pos--;
    }
    else
    {
        return ARTICLES_PREV_PAGE;
    }
    
    return ARTICLES_MENU;
}

/**
 * @brief handle moving down, for current articles page
 */
ArticlesState articles_down(void)
{
    if (article_cursor_pos<articles_on_this_page-1)
    {
        articles_bar();
        article_cursor_pos++;
    }
    else
    {
        return ARTICLES_NEXT_PAGE;
    }
    
    return ARTICLES_MENU;
}

/**
 * @brief Move to previous article page, if possible.
 */
ArticlesState articles_prev_page(void)
{
    if (articles_page>1)
        articles_page--;

    article_cursor_pos=article_cursor_pos_prev=(char) articles_per_page-1;

    memset(_articles, 0, MAX_ARTICLES_PER_PAGE * sizeof(struct Article));

    return ARTICLES_FETCH;
}

/**
 * @brief Move to next article page, if possible.
 */
ArticlesState articles_next_page(void)
{
    articles_page++;

    article_cursor_pos=article_cursor_pos_prev=0;

    memset(_articles, 0, MAX_ARTICLES_PER_PAGE * sizeof(struct Article));

    return ARTICLES_FETCH;
}

/**
 * @brief Move to a specific page.
 */
ArticlesState articles_goto_page(void)
{
    char maxbuf[32];
    char *p;
    int num = 0;
    int maxpage = 1;

    p = strchr(page, '/') + 1;
    strcpy(maxbuf, p);
    maxpage = atoi(maxbuf);

    while (num < 1 || num > maxpage)
    {
        if (textMode == 32)
        {
            cls(2);
            printf("ENTER PAGE # (1-%d)\n", maxpage);
            printf("OR -1 TO QUIT: " );
            locate(15, 1);
        }
        else
        {
            cls(1);
            printf("Go to page (1-%d) or -1 to quit: ", maxpage);
            locate(33, 0);
        }

        p = readline();

        if (p[strlen(p) - 1] == '\n')
        {
            p[strlen(p) - 1] = '\0';
        }

        if (!strcmp(p, "-1"))
        {
            num = -1;
            break;

        }
        else if (is_numeric(p) == TRUE)
        {
            num = atoi(p);
        }
        else
        {
            num = 0;
        }

        if (num < 1 || num > maxpage)
        {
            sound(1, 1);
        }
    }

    if (num != -1)
    {
        articles_page = num;
    }

    article_cursor_pos=article_cursor_pos_prev=0;

    memset(_articles, 0, MAX_ARTICLES_PER_PAGE * sizeof(struct Article));

    return ARTICLES_FETCH;
}

/** 
 * @brief handle Articles state 
 */
State articles(void)
{
    articlesState=ARTICLES_RESET;
    
    while (state == ARTICLES)
    {
        switch(articlesState)
        {
        case ARTICLES_RESET:
            articlesState = articles_reset();
            break;
        case ARTICLES_FETCH:
            articlesState = articles_fetch();
            break;
        case ARTICLES_DISPLAY:
            articlesState = articles_display();
            break;
        case ARTICLES_MENU:
            articlesState = articles_menu();
            break;
        case ARTICLES_UP:
            articlesState = articles_up();
            break;
        case ARTICLES_DOWN:
            articlesState = articles_down();
            break;
        case ARTICLES_SELECT:
            article_id = _articles[article_cursor_pos].id;
            return ARTICLE;
        case ARTICLES_EXIT:
            return TOPICS;
        case ARTICLES_PREV_PAGE:
            articlesState = articles_prev_page();
            break;
        case ARTICLES_NEXT_PAGE:
            articlesState = articles_next_page();
            break;
        case ARTICLES_GOTO_PAGE:
            articlesState = articles_goto_page();
            break;
        }
    }

    return TOPICS;
}

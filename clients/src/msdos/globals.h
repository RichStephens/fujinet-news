/**
 * @brief   FujiNet News Reader for MS-DOS
 * @author  Thomas Cherryhomes
 * @email   thom dot cherryhomes at gmail dot com
 * @license gpl v. 3, see LICENSE for details.
 * @verbose Global vars
 */

#ifndef GLOBALS_H
#define GLOBALS_H

typedef enum _state
{
    TOPICS,
    ARTICLES,
    ARTICLE,
    ARTICLES_PU,
    ARTICLES_PD,
    EXIT,
    IGNORE,
} State;

extern char url[256];

extern State state;

extern unsigned char max_cols;

extern unsigned char column_offset;

extern unsigned char selected_topic;

extern const char *urlBase;

extern int selected_articles_page;
extern int selected_articles_page_max;

extern unsigned long selected_article;

extern int selected_article_page;
extern int selected_article_page_max;

#endif /* GLOBALS_H */

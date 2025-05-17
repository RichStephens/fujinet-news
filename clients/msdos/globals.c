/**
 * @brief   FujiNet News Reader for MS-DOS
 * @author  Thomas Cherryhomes
 * @email   thom dot cherryhomes at gmail dot com
 * @license gpl v. 3, see LICENSE for details.
 * @verbose Global vars
 */

#include "globals.h"

State state;

char url[256];

unsigned char selected_topic=0;

unsigned char max_cols=80;

unsigned char column_offset = 0;

int selected_articles_page = 1;

int selected_article_page = 1;

unsigned long selected_article = 0UL;

const char *urlBase = "N:HTTPS://FUJINET.ONLINE/8bitnews/news.php";

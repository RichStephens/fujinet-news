/**
 * @brief News Reader
 * @author  Thomas Cherryhomes
 * @email   thom dot cherryhomes at gmail dot com
 * @license gpl v. 3, see LICENSE for details.
 * @verbose Global Variables
 */

#include "globals.h"

/**
 * @brief Program state
 */
State state;

const char *urlBase = "N:HTTPS://FUJINET.ONLINE/8bitnews/news.php";

const char *delim = "|";

unsigned char textMode = 32;

const char *topicStrings[] = {
    " Top Stories",
    " World News",
    " Business",
    " Science",
    " Technology",
    " Health",
    " Entertainment",
    " Politics",
    " Sports"
};

/**
 * @brief Determine if string is numeric
 * @param s The string to test
 * @return true if the string is numeric, false otherwise
 */
BOOL is_numeric(const char *s)
{
    if (*s == '\0')
        return FALSE;   // empty string is not numeric

    while (*s)
    {
        if (!isdigit((unsigned char)*s))
            return FALSE;
        s++;
    }
    return TRUE;
}

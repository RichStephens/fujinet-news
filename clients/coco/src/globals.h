/**
 * @brief   News Reader
 * @author  Thomas Cherryhomes
 * @email   thom dot cherryhomes at gmail dot com
 * @license gpl v. 3, see LICENSE for details.
 * @verbose Global State variables
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#include <cmoc.h>
#include <coco.h>

#define true 1
#define false 0

#define ARROW_UP 0x5E
#define ARROW_DOWN 0x0A
#define ARROW_LEFT 0x08
#define ARROW_RIGHT 0x09
#define ENTER 0x0D
#define BREAK 0x03

#define FG_BLACK 0
#define FG_GREEN 1
#define BG_GREEN 0
#define BG_BLUE 2
#define BG_RED 4

/**
 * @brief available program states
 * @enum TOPICS current topics
 * @enum ARTICLES articles for given topic
 * @enum ARTICLE The currently selected article
 * @enum BYE exiting program
 * @enum HALT program halted.
 */
typedef enum _state
{
    TOPICS = 0,
    ARTICLES,
    ARTICLE,
    BYE,
    HALT
} State;

/**
 * @brief currently selected topic
*/
extern enum selected_topic selectedTopic;

/**
 * @brief the current program state 
 */
extern State state;

/**
 * @brief the base URL to be prepended.
 */
extern const char *urlBase;

/**
 * @brief The strtok() delimiter used for returned article lists
 */
extern const char *delim;

/**
 * @brief The text mode - 32, 40, or 80
 */
extern unsigned char textMode;

/**
 * @brief Strings representing the available topics
 */
extern const char *topicStrings[];

/**
 * @brief Determine if string is numeric
 * @param s The string to test
 * @return true if the string is numeric, false otherwise
 */
BOOL is_numeric(const char *s);

#endif /* GLOBALS_H */

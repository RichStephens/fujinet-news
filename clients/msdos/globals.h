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
    EXIT,
} State;

extern State state;

extern unsigned char max_cols;

extern unsigned char column_offset;

extern unsigned char selected_topic;

#endif /* GLOBALS_H */

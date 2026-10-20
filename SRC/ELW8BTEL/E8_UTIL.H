/* Build 26.10.19.1 10:50PM */
/*****************************************************************************
 *   E8_UTIL.H                     8Ball for Teleconference - text helpers   *
 *                                                                           *
 *   Copyright (C) 2026 Elwynor Technologies.                                *
 *                                                                           *
 *   Small text routines with no BBS dependencies, so the unit tests in      *
 *   TESTS\ can build and run them on their own.                             *
 *                                                                           *
 *   Licensed under the GNU Affero General Public License v3.0.              *
 *****************************************************************************/

#ifndef E8_UTIL_H
#define E8_UTIL_H

#include <stddef.h>

/* Copy src to dst (size bytes, always terminated), keeping only printable  */
/* characters, so a user's question can't carry ANSI codes or bells onto    */
/* other users' screens.  Runs of spaces collapse to one, and leading and   */
/* trailing spaces are dropped.  At most maxlen characters are kept.        */
void e8_cleanq(char *dst, size_t size, const char *src, size_t maxlen);

/* Remove ANSI color/cursor codes (ESC [ ... letter) and any other control */
/* characters from s, in place.  Replies are sent to other boards over     */
/* MajorLink, which should get plain text.                                  */
void e8_stripansi(char *s);

/* Split s at its first '|': s keeps the part before it and second (size   */
/* bytes) gets the part after it, both trimmed of spaces.  With no '|',     */
/* second is empty.                                                          */
void e8_splitbar(char *s, char *second, size_t size);

/* The longest question to keep.  The board drops a text variable longer   */
/* than 79 characters if it contains a '%' (wgserver MENUING.C grbtxv), so  */
/* a question with a '%' in it is kept to 79.                               */
size_t e8_qmax(const char *q, size_t normal);

/* Turn line breaks into spaces and drop trailing spaces, in place.  Used   */
/* for text that must fit on one Teleconference line.                       */
void e8_oneline(char *s);

/* Is the channel allowed by the list?  An empty list or "*" allows every   */
/* channel.  Otherwise the list is channel names separated by commas,       */
/* compared without regard to case or surrounding spaces.                   */
int e8_chanok(const char *list, const char *channel);

/* Read a whole number from s.  Returns 1 and sets *val if s is only an     */
/* optional sign and digits (surrounding spaces allowed), else returns 0.   */
int e8_parsenum(const char *s, long *val);

/* Read a yes/no answer: YES, Y, ON, 1 give 1; NO, N, OFF, 0 give 0;       */
/* anything else gives -1.                                                   */
int e8_parsebool(const char *s);

#endif /* E8_UTIL_H */

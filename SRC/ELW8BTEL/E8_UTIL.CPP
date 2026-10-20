/* Build 26.10.19.1 10:50PM */
/*****************************************************************************
 *   E8_UTIL.CPP                   8Ball for Teleconference - text helpers   *
 *                                                                           *
 *   Copyright (C) 2026 Elwynor Technologies.                                *
 *                                                                           *
 *   See E8_UTIL.H.  Nothing here may use the BBS SDK: the unit tests build  *
 *   this file on its own.                                                   *
 *                                                                           *
 *   Licensed under the GNU Affero General Public License v3.0.              *
 *****************************************************************************/

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "E8_UTIL.H"

void
e8_cleanq(char *dst, size_t size, const char *src, size_t maxlen)
{
size_t n = 0;
int space = 0;
unsigned char c;

     if (size == 0) {
          return;
     }
     while ((c = (unsigned char)*src++) != '\0') {
          if (c == ' ' || c == '\t') {
               space = (n > 0);      /* remember it; leading ones are dropped*/
               continue;
          }
          if (c < 32 || c == 127) {  /* ESC, bell, other control codes       */
               continue;
          }
          if (space) {
               if (n + 1 >= size - 1 || n + 1 >= maxlen) {
                    break;
               }
               dst[n++] = ' ';
               space = 0;
          }
          if (n >= size - 1 || n >= maxlen) {
               break;
          }
          dst[n++] = (char)c;
     }
     dst[n] = '\0';
}

void
e8_stripansi(char *s)
{
char *r = s, *w = s;
unsigned char c;

     while ((c = (unsigned char)*r) != '\0') {
          if (c == 27) {                     /* ESC: skip the whole code     */
               r++;
               if (*r == '[') {
                    r++;
                    while (*r != '\0' && !(*r >= '@' && *r <= '~')) {
                         r++;                /* parameters                   */
                    }
                    if (*r != '\0') {
                         r++;                /* the final letter             */
                    }
               }
               continue;
          }
          if (c < 32 || c == 127) {
               r++;
               continue;
          }
          *w++ = *r++;
     }
     *w = '\0';
}

static void
trim(char *s)                        /* drop leading and trailing spaces     */
{
size_t n, i = 0;

     while (s[i] == ' ') {
          i++;
     }
     if (i > 0) {
          memmove(s, s + i, strlen(s + i) + 1);
     }
     n = strlen(s);
     while (n > 0 && s[n - 1] == ' ') {
          s[--n] = '\0';
     }
}

void
e8_splitbar(char *s, char *second, size_t size)
{
char *bar = strchr(s, '|');

     if (size == 0) {
          return;
     }
     second[0] = '\0';
     if (bar != NULL) {
          *bar = '\0';
          strncpy(second, bar + 1, size - 1);
          second[size - 1] = '\0';
          trim(second);
     }
     trim(s);
}

size_t
e8_qmax(const char *q, size_t normal)
{
     return(strchr(q, '%') != NULL && normal > 79 ? 79 : normal);
}

void
e8_oneline(char *s)
{
char *p;
size_t n;

     for (p = s; *p != '\0'; p++) {
          if (*p == '\r' || *p == '\n') {
               *p = ' ';
          }
     }
     n = strlen(s);
     while (n > 0 && s[n - 1] == ' ') {
          s[--n] = '\0';
     }
}

int
e8_chanok(const char *list, const char *channel)
{
const char *p, *b, *e;
size_t len;

     for (p = list; *p == ' '; p++) {
     }
     if (*p == '\0' || strcmp(p, "*") == 0) {
          return(1);
     }
     len = strlen(channel);
     while (*p != '\0') {
          for (b = p; *b == ' '; b++) {      /* start of this name           */
          }
          for (e = b; *e != '\0' && *e != ','; e++) {
          }
          p = (*e == ',') ? e + 1 : e;       /* where the next name starts   */
          while (e > b && e[-1] == ' ') {    /* trim the end of this name    */
               e--;
          }
          if ((size_t)(e - b) == len && len > 0 && _strnicmp(b, channel, len) == 0) {
               return(1);
          }
          if (e - b == 1 && *b == '*') {
               return(1);
          }
     }
     return(0);
}

int
e8_parsenum(const char *s, long *val)
{
char *end;
long v;

     while (*s == ' ') {
          s++;
     }
     if (*s == '\0') {
          return(0);
     }
     v = strtol(s, &end, 10);
     if (end == s) {
          return(0);
     }
     while (*end == ' ') {
          end++;
     }
     if (*end != '\0') {
          return(0);
     }
     *val = v;
     return(1);
}

int
e8_parsebool(const char *s)
{
static const char *yes[] = { "YES", "Y", "ON", "1", NULL };
static const char *no[] = { "NO", "N", "OFF", "0", NULL };
int i;

     while (*s == ' ') {
          s++;
     }
     for (i = 0; yes[i] != NULL; i++) {
          if (_stricmp(s, yes[i]) == 0) {
               return(1);
          }
     }
     for (i = 0; no[i] != NULL; i++) {
          if (_stricmp(s, no[i]) == 0) {
               return(0);
          }
     }
     return(-1);
}

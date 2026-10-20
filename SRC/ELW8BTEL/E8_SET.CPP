/* Build 26.10.19.1 10:50PM */
/*****************************************************************************
 *   E8_SET.CPP                    8Ball for Teleconference - live settings  *
 *                                                                           *
 *   Copyright (C) 2026 Elwynor Technologies.                                *
 *                                                                           *
 *   Loads, seeds and saves the settings record, and describes each setting  *
 *   (its label, allowed values and default) so the "8BALL SET" editor in    *
 *   ELW8BTEL.CPP can list and change them.  See E8_SET.H.                   *
 *                                                                           *
 *   Licensed under the GNU Affero General Public License v3.0.              *
 *****************************************************************************/

#include <stddef.h>
#include <stdio.h>
#include "E8_SET.H"
#include "E8_UTIL.H"

/* The record must fit the stored size, which never changes.               */
typedef char e8s_fits[(sizeof(struct e8setdat) + 16 <= E8S_RECLEN) ? 1 : -1];

struct e8setdat e8cfg;               /* the live settings                    */
static DFAFILE *e8sdat = NULL;       /* ELW8BTEL.DAT                         */

enum { T_BOOL, T_NUM, T_KEY, T_CHAN };

struct setdef {
     INT type;
     size_t offset;                  /* where it lives in struct e8setdat    */
     INT size;                       /* text buffer size (incl. terminator)  */
     LONG min, max;                  /* number range                         */
     LONG defnum;                    /* default for numbers and yes/no       */
     const CHAR *defstr;             /* default for text                     */
     const CHAR *label;              /* what the sysop sees                  */
};

/* The defaults live here, not in ELW8BTEL.MSG: settings are changed only  */
/* online, so there is one place for them.  They seed a new install and   */
/* back "8BALL SET DEFAULTS".                                               */
#define OFS(f) offsetof(struct e8setdat, f)
static const struct setdef setdefs[] = {
     { T_BOOL, OFS(enabled),  0, 0, 1, 1, NULL,
       "8-Ball answers questions" },
     { T_KEY,  OFS(usekey),   LOKSIZ, 0, 0, 0, "",
       "Key to use the 8-Ball (blank = everyone)" },
     { T_KEY,  OFS(forcekey), LOKSIZ, 0, 0, 0, "SYSOP",
       "Key to pick a reply and skip the wait (blank = nobody)" },
     { T_NUM,  OFS(delay),    0, 0, 3600, 30, NULL,
       "Seconds a user waits between uses" },
     { T_CHAN, OFS(channels), E8S_CHNSIZ, 0, 0, 0, "",
       "Channels it works in (blank = all; names separated by commas)" },
};
#define NSET ((INT)(sizeof(setdefs) / sizeof(setdefs[0])))

static VOID *
where(INT i)
{
     return((CHAR *)&e8cfg + setdefs[i].offset);
}

static VOID
loadone(INT i)                       /* one setting to its default           */
{
const struct setdef *d = &setdefs[i];

     if (d->type == T_BOOL || d->type == T_NUM) {
          *(SHORT *)where(i) = (SHORT)d->defnum;
     }
     else {
          stzcpy((CHAR *)where(i), d->defstr, d->size);
     }
}

static VOID
sanity(VOID)                         /* keep loaded values inside limits     */
{
INT i;
const struct setdef *d;
SHORT *s;

     for (i = 0; i < NSET; i++) {
          d = &setdefs[i];
          if (d->type == T_BOOL || d->type == T_NUM) {
               s = (SHORT *)where(i);
               if (*s < d->min || *s > d->max) {
                    *s = (SHORT)d->defnum;
               }
          }
          else {
               ((CHAR *)where(i))[d->size - 1] = '\0';
          }
     }
}

static VOID
save(VOID)                           /* write e8cfg to ELW8BTEL.DAT          */
{
struct e8setrec rec;
GBOOL ok;

     setmem((CHAR *)&rec, sizeof(rec), 0);
     /* The key must be in writable memory (a stack copy, never the string  */
     /* constant): the data-file code reads keys through goodblk(), which   */
     /* treats read-only memory as empty (WG33 DFAAPI.C, ISGOODPT.C).        */
     stzcpy(rec.recid, E8S_RECID, sizeof(rec.recid));
     e8cfg.layout = 1;
     rec.d = e8cfg;
     dfaSetBlk(e8sdat);
     if (dfaAcqEQ(NULL, rec.recid, 0)) {
          ok = dfaUpdateDup(&rec);
     }
     else {
          ok = dfaInsertDup(&rec);
     }
     dfaRstBlk();
     if (!ok) {
          shocst("8BALL SETTINGS NOT SAVED", "%s", "Could not write ELW8BTEL.DAT");
     }
}

VOID
e8set_defaults(VOID)
{
INT i;

     setmem((CHAR *)&e8cfg, sizeof(e8cfg), 0);
     for (i = 0; i < NSET; i++) {
          loadone(i);
     }
     sanity();
     save();
}

VOID
e8set_open(VOID)
{
struct e8setrec rec;

     if (GetFileAttributesA(E8_SETFN) == INVALID_FILE_ATTRIBUTES) {
          struct dfaSegSpec seg;
          struct dfaKeySpec key;

          setmem((CHAR *)&seg, sizeof(seg), 0);
          seg.position = offsetof(struct e8setrec, recid);
          seg.length = sizeof(rec.recid);
          seg.type = DFAST_ZSTRING;
          setmem((CHAR *)&key, sizeof(key), 0);
          key.nSegments = 1;
          key.segs = &seg;
          /* The page must be bigger than the record (Btrieve error 28).    */
          dfaCreateSpec(E8_SETFN, FALSE, sizeof(struct e8setrec), 1024,
                        0, 0, 1, &key, NULL);
     }
     e8sdat = dfaOpen(E8_SETFN, sizeof(struct e8setrec), NULL);

     setmem((CHAR *)&rec, sizeof(rec), 0);
     stzcpy(rec.recid, E8S_RECID, sizeof(rec.recid));   /* writable key    */
     dfaSetBlk(e8sdat);
     if (dfaAcqEQ(&rec, rec.recid, 0)) {
          dfaRstBlk();
          e8cfg = rec.d;
          sanity();
          return;
     }
     dfaRstBlk();
     e8set_defaults();               /* a new install                        */
     shocst("8BALL SETTINGS CREATED", "%s", "New settings file, default values");
}

VOID
e8set_close(VOID)
{
     if (e8sdat != NULL) {
          dfaClose(e8sdat);
          e8sdat = NULL;
     }
}

INT
e8set_count(VOID)
{
     return(NSET);
}

const CHAR *
e8set_label(INT n)
{
     return(n >= 1 && n <= NSET ? setdefs[n - 1].label : "");
}

const CHAR *
e8set_value(INT n)
{
static CHAR buf[E8S_CHNSIZ + 16];
const struct setdef *d;

     if (n < 1 || n > NSET) {
          return("");
     }
     d = &setdefs[n - 1];
     switch (d->type) {
     case T_BOOL:
          return(*(SHORT *)where(n - 1) ? "Yes" : "No");
     case T_NUM:
          sprintf(buf, "%d", *(SHORT *)where(n - 1));
          return(buf);
     default:
          if (*(CHAR *)where(n - 1) == '\0') {
               return(d->type == T_CHAN ? "(all channels)"
                    : (d->offset == OFS(usekey) ? "(everyone)" : "(nobody)"));
          }
          return((CHAR *)where(n - 1));
     }
}

const CHAR *
e8set_range(INT n)
{
static CHAR buf[64];
const struct setdef *d;

     if (n < 1 || n > NSET) {
          return("");
     }
     d = &setdefs[n - 1];
     switch (d->type) {
     case T_BOOL:
          return("YES or NO");
     case T_NUM:
          sprintf(buf, "a number from %ld to %ld", d->min, d->max);
          return(buf);
     case T_KEY:
          sprintf(buf, "a key name up to %d characters, or - for blank",
                  d->size - 1);
          return(buf);
     default:
          sprintf(buf, "channel names separated by commas (up to %d"
                  " characters), or - for all", d->size - 1);
          return(buf);
     }
}

GBOOL
e8set_change(INT n, const CHAR *text)
{
const struct setdef *d;
LONG v;
INT b;
CHAR tmp[E8S_CHNSIZ];

     if (n < 1 || n > NSET) {
          return(FALSE);
     }
     d = &setdefs[n - 1];
     switch (d->type) {
     case T_BOOL:
          if ((b = e8_parsebool(text)) < 0) {
               return(FALSE);
          }
          *(SHORT *)where(n - 1) = (SHORT)b;
          break;
     case T_NUM:
          if (!e8_parsenum(text, &v) || v < d->min || v > d->max) {
               return(FALSE);
          }
          *(SHORT *)where(n - 1) = (SHORT)v;
          break;
     default:
          e8_cleanq(tmp, sizeof(tmp), text, sizeof(tmp) - 1);
          if (strlen(tmp) > (size_t)(d->size - 1)) {
               return(FALSE);
          }
          if (strcmp(tmp, "-") == 0) {
               tmp[0] = '\0';
          }
          if (d->type == T_KEY) {
               strupr(tmp);          /* a key or a lock expression           */
          }
          stzcpy((CHAR *)where(n - 1), tmp, d->size);
          break;
     }
     save();
     return(TRUE);
}

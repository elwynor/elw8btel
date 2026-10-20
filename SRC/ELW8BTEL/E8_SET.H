/* Build 26.10.19.1 10:50PM */
/*****************************************************************************
 *   E8_SET.H                      8Ball for Teleconference - live settings  *
 *                                                                           *
 *   Copyright (C) 2026 Elwynor Technologies.                                *
 *                                                                           *
 *   The settings are kept in one record in ELW8BTEL.DAT and are changed     *
 *   online, while the board runs, with "8BALL SET" in Teleconference.       *
 *   There are no CNF options: the defaults are in E8_SET.CPP, and they set  *
 *   up a new install and back "8BALL SET DEFAULTS".                         *
 *                                                                           *
 *   Adding a setting: add a field at the END of struct e8setdat, taking its *
 *   room out of spare[] so the record size and earlier fields never move,   *
 *   make an all-zero value mean "as before", and add a row to setdefs[] in  *
 *   E8_SET.CPP.                                                             *
 *                                                                           *
 *   Licensed under the GNU Affero General Public License v3.0.              *
 *****************************************************************************/

#ifndef E8_SET_H
#define E8_SET_H

#include "E8_DEFS.H"

#define E8_SETFN     "ELW8BTEL.DAT"  /* the settings file                    */
#define E8S_RECID    "SETTINGS"      /* key of the one settings record       */
#define E8S_RECLEN   512             /* stored record size; never changes    */
#define E8S_CHNSIZ   200             /* channel list, with its terminator    */

/* The record is stored on disk, so its layout must not depend on compiler */
/* padding: pack it to single bytes.                                        */
#pragma pack(push, 1)
struct e8setdat {
     SHORT layout;                   /* 1                                    */
     SHORT enabled;                  /* 1 = the 8-Ball answers               */
     SHORT delay;                    /* seconds between uses, per user       */
     CHAR  usekey[LOKSIZ];           /* key to use it; "" = everyone         */
     CHAR  forcekey[LOKSIZ];         /* key to pick a reply and skip the     */
                                     /* wait; "" = nobody                    */
     CHAR  channels[E8S_CHNSIZ];     /* allowed channels; "" = all           */
};

struct e8setrec {                    /* the record as stored                 */
     CHAR recid[16];                 /* E8S_RECID (key 0)                    */
     struct e8setdat d;
     CHAR spare[E8S_RECLEN - 16 - sizeof(struct e8setdat)];
};
#pragma pack(pop)

extern struct e8setdat e8cfg;        /* the live settings                    */

VOID e8set_open(VOID);               /* open the file; load or seed          */
VOID e8set_close(VOID);
VOID e8set_defaults(VOID);           /* every setting back to its default    */

/* For the editor.  Settings are numbered from 1.                          */
INT e8set_count(VOID);
const CHAR *e8set_label(INT n);
const CHAR *e8set_value(INT n);      /* current value, ready to show         */
const CHAR *e8set_range(INT n);      /* what may be entered                  */

/* Change setting n from the text the sysop typed, and save.  "-" clears   */
/* a text setting.  Returns TRUE if the value was accepted.                */
GBOOL e8set_change(INT n, const CHAR *text);

#endif /* E8_SET_H */

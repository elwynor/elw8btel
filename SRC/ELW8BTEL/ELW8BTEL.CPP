/* Build 26.10.28.1 12:58PM */
/*****************************************************************************
 *   ELW8BTEL.CPP                  8Ball for Teleconference - the module     *
 *                                                                           *
 *   Copyright (C) 2026 Elwynor Technologies.                                *
 *   Originally HOTELeconference 8Ball (ANXH8BAL) by Randall R. Spangler,    *
 *   annex.com, 1997.                                                        *
 *   Converted for The Major BBS v10 by Mark Laudenbach -                    *
 *   https://github.com/laudenbachm                                          *
 *                                                                           *
 *   A user in Teleconference types                                          *
 *                                                                           *
 *        8BALL Should I log off now?                                        *
 *                                                                           *
 *   and everyone in the channel sees the question and the Magic 8-Ball's    *
 *   answer.  The answer is sent the same way an ordinary chat line is, so   *
 *   it also reaches users on other boards in a linked channel such as       *
 *   MajorLink (formerly WorldLink), and it needs no 8Ball on that board.    *
 *                                                                           *
 *   The module has no menu entry.  It adds the 8BALL command to the stock   *
 *   Teleconference when the board starts.  Sysops change its settings       *
 *   online, in Teleconference, with 8BALL SET.                              *
 *                                                                           *
 *   Licensed under the GNU Affero General Public License v3.0.              *
 *****************************************************************************/

#include <stdlib.h>
#include <time.h>
#include "E8_DEFS.H"
#include "E8_TLC.H"
#include "E8_SET.H"
#include "E8_UTIL.H"
#include "ELW8BTEL.H"                /* message numbers, from ELW8BTEL.MST   */

extern "C" {
VOID EXPORT init__elw8btel(VOID);
}

static VOID e8start(VOID);
static GBOOL e8lon(VOID);
static VOID e8hup(VOID);
static VOID e8fin(VOID);
static SHORT e8cmd(LPCMDPARAM param);

/* The module block.  Only the logon, hang-up and shut-down routines are   */
/* used: users never "enter" this module, they use it from Teleconference. */
static struct module e8mod = {
     "",                             /* name, filled in from ELW8BTEL.MDF    */
     e8lon,                          /* logon: clear this channel's timer    */
     NULL,                           /* input (not used)                     */
     NULL,                           /* status input (not used)              */
     NULL,                           /* injoth (not used)                    */
     NULL,                           /* logoff (not used)                    */
     e8hup,                          /* hang-up: clear this channel's timer  */
     NULL,                           /* nightly cleanup (not used)           */
     NULL,                           /* account deleted (not used)           */
     e8fin                           /* clean shut-down                      */
};

/* The Teleconference command.  The engine keeps a pointer to this, so it  */
/* must stay put for the life of the board (tlcdyncm.cpp).                  */
static CMDSTRUCT e8cmdst;

static HMCVFILE e8mb = NULL;         /* ELW8BTEL.MCV                         */
static LONG *lastuse = NULL;         /* per channel: time of last use        */

/* Values for the text variables.  Each is filled in just before the       */
/* message that shows it.                                                   */
static CHAR tvuser[TLCUIDSIZ];
static CHAR tvquest[E8_MAXQ + 1];
static CHAR tvanswer[E8_PUBSIZ];     /* the whole answer, on one line        */
static CHAR tvline1[E8_PUBSIZ];      /* the answer as it sits in the 8-Ball  */
static CHAR tvline2[E8_PUBSIZ];      /* picture: two lines, split at '|'     */
static CHAR tvwait[16];
static CHAR tvchan[CHAN_MAX_SIZE];
static CHAR tvsetnum[8];
static CHAR tvsetnam[80];
static CHAR tvsetval[E8S_CHNSIZ + 16];
static CHAR tvsetrng[96];

static CHAR *tv_user(VOID)     { return(tvuser); }
static CHAR *tv_quest(VOID)    { return(tvquest); }
static CHAR *tv_answer(VOID)   { return(tvanswer); }
static CHAR *tv_line1(VOID)    { return(tvline1); }
static CHAR *tv_line2(VOID)    { return(tvline2); }
static CHAR *tv_wait(VOID)     { return(tvwait); }
static CHAR *tv_chan(VOID)     { return(tvchan); }
static CHAR *tv_setnum(VOID)   { return(tvsetnum); }
static CHAR *tv_setnam(VOID)   { return(tvsetnam); }
static CHAR *tv_setval(VOID)   { return(tvsetval); }
static CHAR *tv_setrng(VOID)   { return(tvsetrng); }
static CHAR *tv_version(VOID)  { return((CHAR *)ELW8BTEL_VERSION); }
static CHAR *tv_cmd(VOID)      { return((CHAR *)E8_CMDWORD); }

static CHAR *
tv_delay(VOID)                       /* the current wait, read live          */
{
static CHAR buf[8];

     sprintf(buf, "%d", e8cfg.delay);
     return(buf);
}

/***************************************************************************
 *   Start-up and shut-down                                                *
 ***************************************************************************/

VOID EXPORT
init__elw8btel(VOID)
{
     stzcpy(e8mod.descrp, gmdnam("ELW8BTEL.MDF"), MNMSIZ);
     register_module(&e8mod);
     e8mb = opnmsg("ELW8BTEL.MCV");
     lastuse = (LONG *)alczer(nterms * sizeof(LONG));
     srand((unsigned)time(NULL));

     /* Text variable names are shared by the whole board, so they all     */
     /* start with E8B_.                                                     */
     register_textvar("E8B_USER", tv_user);
     register_textvar("E8B_QUESTION", tv_quest);
     register_textvar("E8B_ANSWER", tv_answer);
     register_textvar("E8B_LINE1", tv_line1);
     register_textvar("E8B_LINE2", tv_line2);
     register_textvar("E8B_WAIT", tv_wait);
     register_textvar("E8B_DELAY", tv_delay);
     register_textvar("E8B_CHANNEL", tv_chan);
     register_textvar("E8B_SETNUM", tv_setnum);
     register_textvar("E8B_SETNAME", tv_setnam);
     register_textvar("E8B_SETVALUE", tv_setval);
     register_textvar("E8B_SETRANGE", tv_setrng);
     register_textvar("E8B_VERSION", tv_version);
     register_textvar("E8B_CMD", tv_cmd);

     e8set_open();

     if (!e8tlc_bind()) {
          shocst("8BALL NOT STARTED", "v%s: %s", ELW8BTEL_VERSION, e8tlc_error());
          return;
     }
     /* The command is added once every module has started (the board's    */
     /* "startup vectors", MAJORBBS.C), so Teleconference and the modules  */
     /* it depends on are ready whatever order the board loaded them in.   */
     hook_startup(e8start);
}

static VOID
e8start(VOID)
{
     e8tlc_init();
     /* Add 8BALL to Teleconference.  "fNSpace" = 1: the command is a word  */
     /* of its own, followed by a space and the question (tlcapi.cpp Parse).*/
     setmem((CHAR *)&e8cmdst, sizeof(e8cmdst), 0);
     stzcpy(e8cmdst.pszLong, E8_CMDWORD, MAX_CMD_SIZEL);
     e8cmdst.fNSpace = 1;
     e8cmdst.bTOnly = FALSE;         /* web Teleconference users too         */
     e8cmdst.docmdfunc = e8cmd;
     if (!e8tlc_register(&e8cmdst)) {
          shocst("8BALL NOT STARTED", "v%s: %s", ELW8BTEL_VERSION,
                 "Teleconference refused the 8BALL command");
          return;
     }
     shocst("8BALL FOR TELECONFERENCE", "v%s loaded (command %s)",
            ELW8BTEL_VERSION, E8_CMDWORD);
}

static GBOOL
e8lon(VOID)
{
     /* A new caller on this channel starts with no wait.  Done at logon as */
     /* well as hang-up, because the board reuses a channel the moment a    */
     /* caller drops, and a missed hang-up must not carry over.             */
     lastuse[usrnum] = 0L;
     return(0);                      /* carry on with the normal logon       */
}

static VOID
e8hup(VOID)
{
     lastuse[usrnum] = 0L;
}

static VOID
e8fin(VOID)
{
     /* The Teleconference has no way to remove a command, and it shuts     */
     /* down with the board, so there is nothing to undo there.             */
     e8set_close();
     if (e8mb != NULL) {
          clsmsg(e8mb);
          e8mb = NULL;
     }
}

/***************************************************************************
 *   Talking to the user                                                   *
 ***************************************************************************/

static VOID
tell(LPUSRSTRUCT u)                  /* send prfbuf to the user, privately   */
{
     /* Going through the Teleconference (not straight to the channel)      */
     /* reaches web users too, and keeps the output in step with the        */
     /* Teleconference's own prompt.                                         */
     e8tlc_usersend(u->m_pszName, u->m_pszName, NULL, RECV_TYPE_PRFBUF);
     clrprf();
}

static VOID
say(LPUSRSTRUCT u, INT msgnum)       /* one message, privately               */
{
     clrprf();
     prfmsg(msgnum);
     tell(u);
}

/***************************************************************************
 *   8BALL SET - the settings editor                                       *
 ***************************************************************************/

static VOID
setlist(LPUSRSTRUCT u)
{
INT n;

     clrprf();
     prfmsg(SETHDR);
     for (n = 1; n <= e8set_count(); n++) {
          sprintf(tvsetnum, "%d", n);
          stzcpy(tvsetnam, e8set_label(n), sizeof(tvsetnam));
          stzcpy(tvsetval, e8set_value(n), sizeof(tvsetval));
          prfmsg(SETROW);
     }
     prfmsg(SETFOOT);
     tell(u);
}

static VOID
seteditor(LPUSRSTRUCT u, CHAR *args) /* args: what follows "SET"            */
{
CHAR *val;
LONG n;
CHAR audit[128];

     while (*args == ' ') {
          args++;
     }
     if (*args == '\0') {
          setlist(u);
          return;
     }
     if (sameas(args, "DEFAULTS")) {
          e8set_defaults();
          _snprintf(audit, sizeof(audit) - 1, "%s reset all settings", u->m_pszName);
          audit[sizeof(audit) - 1] = '\0';
          shocst("8BALL SETTINGS RESET", "%s", audit);
          say(u, SETDEF);
          return;
     }
     /* "<number> <value>" */
     for (val = args; *val != '\0' && *val != ' '; val++) {
     }
     if (*val == ' ') {
          *val++ = '\0';
     }
     if (!e8_parsenum(args, &n) || n < 1 || n > e8set_count()) {
          setlist(u);
          return;
     }
     sprintf(tvsetnum, "%ld", n);
     stzcpy(tvsetnam, e8set_label((INT)n), sizeof(tvsetnam));
     while (*val == ' ') {
          val++;
     }
     if (*val == '\0' || !e8set_change((INT)n, val)) {
          stzcpy(tvsetval, e8set_value((INT)n), sizeof(tvsetval));
          stzcpy(tvsetrng, e8set_range((INT)n), sizeof(tvsetrng));
          say(u, SETBAD);
          return;
     }
     stzcpy(tvsetval, e8set_value((INT)n), sizeof(tvsetval));
     /* Audit Trail details must stay under 128 characters (AUDAPI.H).     */
     _snprintf(audit, sizeof(audit) - 1, "%s: #%ld = %.60s", u->m_pszName, n,
               tvsetval);
     audit[sizeof(audit) - 1] = '\0';
     shocst("8BALL SETTING CHANGED", "%s", audit);
     say(u, SETOK);
}

/***************************************************************************
 *   Consulting the 8-Ball                                                 *
 ***************************************************************************/

static INT
pickreply(INT forced)                /* a reply number, or -1 if none        */
{
INT ok[E8_NREPLY];
INT i, n = 0;
CHAR *t;

     /* Use every REPLYnn the sysop has filled in; blank ones are skipped,  */
     /* so replies can be added or removed just by editing the text.        */
     for (i = 0; i < E8_NREPLY; i++) {
          t = getmsg(REPLY01 + i);
          while (*t == ' ' || *t == '\r' || *t == '\n') {
               t++;
          }
          if (*t != '\0') {
               ok[n++] = i;
          }
     }
     if (n == 0) {
          return(-1);
     }
     if (forced >= 1 && forced <= E8_NREPLY) {
          for (i = 0; i < n; i++) {
               if (ok[i] == forced - 1) {
                    return(ok[i]);
               }
          }
     }
     return(ok[rand() % n]);
}

static VOID
consult(LPUSRSTRUCT u, const CHAR *text, GBOOL force)
{
INT forced = 0, r;
LONG now = (LONG)time(NULL);
CHAR pub[E8_PUBSIZ];
const CHAR *q = text;

     /* "8BALL /REPLY3 question" - a key holder picks reply 3, just as in  */
     /* the original HOTELeconference 8Ball.                                */
     if (force && sameto("/REPLY", (CHAR *)q)) {
          forced = atoi(q + 6);
          while (*q != '\0' && *q != ' ') {
               q++;
          }
     }

     if (!force && lastuse[usrnum] != 0L && now < lastuse[usrnum] + e8cfg.delay) {
          sprintf(tvwait, "%ld", lastuse[usrnum] + e8cfg.delay - now);
          say(u, WAIT);
          return;
     }
     if ((r = pickreply(forced)) < 0) {
          say(u, NOREPLY);
          return;
     }

     /* Fill in the text variables.                                        */
     stzcpy(tvuser, u->m_pszName, sizeof(tvuser));
     e8_cleanq(tvquest, sizeof(tvquest), q, e8_qmax(q, E8_MAXQ));
     /* A reply is written "first line|second line": the two lines that    */
     /* show inside the 8-Ball picture.  On one line, the '|' is a space.   */
     stzcpy(tvline1, getmsg(REPLY01 + r), sizeof(tvline1));
     e8_stripansi(tvline1);
     e8_oneline(tvline1);
     e8_splitbar(tvline1, tvline2, sizeof(tvline2));
     stzcpy(tvanswer, tvline1, sizeof(tvanswer));
     if (tvline2[0] != '\0') {
          stlcat(tvanswer, " ", sizeof(tvanswer));
          stlcat(tvanswer, tvline2, sizeof(tvanswer));
     }

     /* The line everyone else sees.  It is sent as ordinary chat from this */
     /* user, which is what lets it cross MajorLink: other boards show it   */
     /* as "From <user>: <line>" in their own wording.                      */
     clrprf();
     prfmsg(tvquest[0] != '\0' ? PUBQ : PUBNQ);
     stzcpy(pub, prfbuf, sizeof(pub));
     clrprf();
     e8_oneline(pub);

     /* The asker sees their own version first, with the 8-Ball picture... */
     prfmsg(tvquest[0] != '\0' ? ASKQ : ASKNQ);
     prfmsg(BALL);
     tell(u);

     /* ...then everyone else in the channel, here and on linked boards.   */
     /* The asker is left out: with echo off, Teleconference would only    */
     /* show them "-- Message Sent --" for their own line.                 */
     e8tlc_chansendbut(u->m_pszChannel, u->m_pszName, pub, RECV_TYPE_PUBLIC,
                       u->m_pszName);

     lastuse[usrnum] = now;
}

/***************************************************************************
 *   The 8BALL command                                                     *
 ***************************************************************************/

static SHORT
e8cmd(LPCMDPARAM param)
{
LPUSRSTRUCT u = param->m_pUser;
INT savusn = usrnum;
CHAR text[256];
CHAR *args;
GBOOL force;

     /* Only users on this board can type a command here.  Users from      */
     /* linked boards run their own board's 8Ball, if it has one.           */
     if (u == NULL || u->m_iUsrnum < 0 || u->m_iUsrnum >= nterms) {
          return(0);
     }
     if (usrnum != u->m_iUsrnum) {   /* key checks need the user's context  */
          curusr(u->m_iUsrnum);
     }
     setmbk(e8mb);

     stzcpy(text, param->m_pszCmdParam != NULL ? param->m_pszCmdParam : "",
            sizeof(text));
     for (args = text; *args == ' '; args++) {
     }

     if (sameas(args, "?") || sameas(args, "HELP")) {
          say(u, HELP);
     }
     else if (haskey("SYSOP") == 1 && (sameas(args, "SET") || sameto("SET ", args))) {
          seteditor(u, args + 3);
     }
     else if (!e8cfg.enabled) {
          say(u, RESTING);
     }
     else if (e8cfg.usekey[0] != '\0' && haskey(e8cfg.usekey) != 1) {
          say(u, NOACCESS);          /* haskey() is -1 for an unknown key    */
     }
     else if (u->m_bSquelched) {
          say(u, SQUELCH);
     }
     else if (!e8_chanok(e8cfg.channels, u->m_pszChannel)) {
          stzcpy(tvchan, u->m_pszChannel, sizeof(tvchan));
          say(u, NOCHAN);
     }
     else {
          force = (e8cfg.forcekey[0] != '\0' && haskey(e8cfg.forcekey) == 1);
          consult(u, args, force);
     }

     rstmbk();
     if (usrnum != savusn && savusn >= 0) {
          curusr(savusn);
     }
     return(1);                      /* handled: not spoken to the channel   */
}

VOID EXPORT
end_elw8btel(VOID)                   /* marks the end of the code, for       */
{                                    /* crash reports (GALEXCEP.OUT)         */
}

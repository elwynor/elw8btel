/* Build 26.10.19.1 11:15PM */
/*****************************************************************************
 *   TEST_UTIL.CPP                 8Ball for Teleconference - unit tests     *
 *                                                                           *
 *   Copyright (C) 2026 Elwynor Technologies.                                *
 *                                                                           *
 *   Tests the text helpers in E8_UTIL.CPP, the parts that can be checked    *
 *   away from a BBS: cleaning a question, removing ANSI codes from a reply, *
 *   splitting a reply into its two picture lines, the channel list, and     *
 *   reading numbers and yes/no answers typed in the settings editor.        *
 *                                                                           *
 *   Build and run: TESTS\RUN_TESTS.CMD.  Exit code 0 = all passed.          *
 *                                                                           *
 *   Licensed under the GNU Affero General Public License v3.0.              *
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include "E8_UTIL.H"

static int fails = 0;
static int checks = 0;

static void
check(int ok, const char *what)
{
    checks++;
    if (!ok) {
        fails++;
        printf("FAIL: %s\n", what);
    }
}

static void
checkstr(const char *got, const char *want, const char *what)
{
    checks++;
    if (strcmp(got, want) != 0) {
        fails++;
        printf("FAIL: %s\n      got  [%s]\n      want [%s]\n", what, got, want);
    }
}

static void
test_cleanq(void)
{
    char b[64];

    e8_cleanq(b, sizeof(b), "  Should   I\tlog off?  ", 150);
    checkstr(b, "Should I log off?", "cleanq collapses and trims spaces");

    e8_cleanq(b, sizeof(b), "red \x1b[31mtext\x07!", 150);
    checkstr(b, "red [31mtext!", "cleanq drops control bytes (ESC, BEL)");

    e8_cleanq(b, sizeof(b), "abcdefghij", 4);
    checkstr(b, "abcd", "cleanq stops at maxlen");

    e8_cleanq(b, sizeof(b), "ab cd", 3);
    checkstr(b, "ab", "cleanq never ends on a space at the cut");

    e8_cleanq(b, 6, "abcdefghij", 150);
    checkstr(b, "abcde", "cleanq respects the buffer size");

    e8_cleanq(b, sizeof(b), "", 150);
    checkstr(b, "", "cleanq of nothing");
}

static void
test_stripansi(void)
{
    char b[64];

    strcpy(b, "\x1b[0;1;37mMy Sources\x1b[30m|Say No\x1b[0m\r\n");
    e8_stripansi(b);
    checkstr(b, "My Sources|Say No", "stripansi removes whole ANSI codes and line ends");

    strcpy(b, "plain [brackets] stay");
    e8_stripansi(b);
    checkstr(b, "plain [brackets] stay", "stripansi leaves a bare '[' alone");

    strcpy(b, "cut\x1b[1;3");
    e8_stripansi(b);
    checkstr(b, "cut", "stripansi copes with a code cut off at the end");
}

static void
test_splitbar(void)
{
    char a[64], b[64];

    strcpy(a, "My Sources | Say No");
    e8_splitbar(a, b, sizeof(b));
    checkstr(a, "My Sources", "splitbar first line");
    checkstr(b, "Say No", "splitbar second line");

    strcpy(a, "YES");
    e8_splitbar(a, b, sizeof(b));
    checkstr(a, "YES", "splitbar with no bar keeps the line");
    checkstr(b, "", "splitbar with no bar gives an empty second line");
}

static void
test_oneline(void)
{
    char b[64];

    strcpy(b, "[Magic 8-Ball] Yes\r\n  ");
    e8_oneline(b);
    checkstr(b, "[Magic 8-Ball] Yes", "oneline trims the line end");

    strcpy(b, "a\rb\nc");
    e8_oneline(b);
    checkstr(b, "a b c", "oneline turns inner breaks into spaces");
}

static void
test_chanok(void)
{
    check(e8_chanok("", "Main"), "empty list allows every channel");
    check(e8_chanok("   ", "Main"), "blank list allows every channel");
    check(e8_chanok("*", "MajorLink"), "* allows every channel");
    check(e8_chanok("MajorLink", "majorlink"), "match ignores case");
    check(e8_chanok("Main, MajorLink ,Games", "MajorLink"), "match in the middle, spaces trimmed");
    check(e8_chanok("Main,MajorLink", "Main"), "match first");
    check(!e8_chanok("Main,MajorLink", "Major"), "no partial-name match");
    check(!e8_chanok("Main,MajorLink", "MajorLinkX"), "no longer-name match");
    check(!e8_chanok("Main", ""), "an empty channel name never matches a list");
    check(!e8_chanok("Main,,", "Games"), "empty entries match nothing");
    check(e8_chanok("Main, *", "Games"), "a * entry in a list allows all");
}

static void
test_parse(void)
{
    long v = -99;

    check(e8_parsenum(" 30 ", &v) && v == 30, "parsenum reads 30");
    check(e8_parsenum("-1", &v) && v == -1, "parsenum reads a sign");
    check(!e8_parsenum("30s", &v), "parsenum rejects trailing letters");
    check(!e8_parsenum("", &v), "parsenum rejects nothing");
    check(!e8_parsenum("abc", &v), "parsenum rejects words");

    check(e8_parsebool("yes") == 1 && e8_parsebool("ON") == 1 && e8_parsebool("1") == 1,
          "parsebool yes");
    check(e8_parsebool("No") == 0 && e8_parsebool("off") == 0 && e8_parsebool("0") == 0,
          "parsebool no");
    check(e8_parsebool("maybe") == -1, "parsebool rejects other words");
}

static void
test_qmax(void)
{
    check(e8_qmax("Will I win?", 150) == 150, "qmax normal question");
    check(e8_qmax("100% sure?", 150) == 79,
          "qmax: a question with % is kept to 79 (longer is dropped by the board)");
}

int
main(void)
{
    test_cleanq();
    test_stripansi();
    test_splitbar();
    test_oneline();
    test_chanok();
    test_parse();
    test_qmax();
    printf("%d checks, %d failed\n", checks, fails);
    return(fails == 0 ? 0 : 1);
}

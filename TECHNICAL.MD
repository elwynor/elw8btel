# 8Ball for Teleconference - technical notes

## Overview

ELW8BTEL is a background module: its MDF is `UNCONDITIONAL` and `INTERNAL`,
and it has no menu entry. Its init finds the Teleconference functions and
opens its files. It then registers a startup hook (`hook_startup`), which the
board runs after every module's init and before the main loop (WG33
MAJORBBS.C:573-595). That hook adds the `8BALL` command to the stock
Teleconference (GALTELE), so module load order doesn't matter. From then on,
Teleconference calls the module whenever a user types `8BALL ...`.

| File | What it holds |
|---|---|
| `ELW8BTEL.CPP` | Module start-up and shut-down, the `8BALL` command, the settings editor, text variables |
| `E8_TLC.CPP/H` | The link to the Teleconference engine |
| `E8_SET.CPP/H` | The settings record and its descriptions |
| `E8_UTIL.CPP/H` | Text helpers with no BBS dependencies (unit tested) |
| `ELW8BTEL.MST` | Message template; `TOOLS/MKMSG.PY` builds `.MSG` and `.H` from it |

## The Teleconference API used

GALTELE.DLL exports a small C API for add-ons (declared in the SDK's
`TLCAPIC.H`). The module uses four functions, found by export name at
start-up:

| Export | Use |
|---|---|
| `_init__galtele` | Called from the startup hook to make sure the engine is set up. It runs only once (static guard), so it is a no-op when the board has already started Teleconference. |
| `_tlcCommandRegister` | Adds `8BALL` as a Teleconference command (`CMDSTRUCT`, `fNSpace = 1`). |
| `_tlcUserSendText` | Sends `prfbuf` privately to one user (`RECV_TYPE_PRFBUF`), through that user's own transport (terminal or web). |
| `_tlcChannelPublicSendBut` | Sends the answer line to everyone in the channel except the asker (`RECV_TYPE_PUBLIC`, message number 0). |

The Teleconference keeps a pointer to the `CMDSTRUCT`, so it is static. The
engine has no call to remove a command, which is why the command word is
fixed (`8BALL`). When the command handler returns 1, Teleconference treats
the line as handled, and it is not spoken to the channel.

## How the answer reaches other boards

`tlcChannelPublicSendBut` hands the text to every registered Teleconference
transport. On a board running the WorldLink Teleconference (which carries
MajorLink), one of those transports forwards public text in linked channels
to the hub. Each receiving board passes it to its own users and renders it
with its own "From <user>: <text>" wording. That is the same path as a line
typed by hand, so nothing on the other board needs to know about 8Ball.
Two consequences follow:

- The line must be plain text that makes sense after "From <user>:" (PUBQ
  and PUBNQ in the `.MSG`).
- Message number 0 must be used. Any other number would be looked up in
  the receiving board's own `GALTELE.MSG`.

The asker is left out of the public send. With echo off, Teleconference
shows a sender only "-- Message Sent --" for their own line. Instead, the
asker gets a private view with the 8-Ball picture.

## Output

Private output (help, errors, the asker's picture, the settings editor) is
built in `prfbuf` with `prfmsg()` and sent with `tlcUserSendText(...,
RECV_TYPE_PRFBUF)`, so it reaches web Teleconference users as well as
terminal users. The public line is rendered with `prfmsg(PUBQ/PUBNQ)`, then
copied out of `prfbuf` and flattened to one line.

## Text variables

All dynamic text is named text variables with the `E8B_` prefix: `USER`,
`QUESTION`, `ANSWER`, `LINE1`, `LINE2`, `WAIT`, `DELAY`, `CHANNEL`, `VERSION`,
`CMD`, `SETNUM`, `SETNAME`, `SETVALUE`, `SETRANGE`.

The board doubles a lone `%` in a variable's value by itself. However, it
silently drops a natural-width value longer than 79 characters that
contains a `%` (wgserver `MENUING.C`, `grbtxv`). So a question containing
`%` is kept to 79 characters (`e8_qmax`), and answers should stay under 80.

## Settings record

`ELW8BTEL.DAT` holds one 512-byte record with key `"SETTINGS"` (key 0, a
zero-terminated string). The page size is 1024. The file is created with
`dfaCreateSpec` on first start. The record struct is packed to one byte. New
settings take their room from `spare[]`, and an all-zero value means "as
before". Settings are read at the point of use, never cached.

| Field | Type | Default |
|---|---|---|
| `layout` | SHORT | 1 |
| `enabled` | SHORT | 1 |
| `delay` | SHORT | 30 (0-3600) |
| `usekey` | CHAR[48] | "" (everyone) |
| `forcekey` | CHAR[48] | "SYSOP" ("" = nobody) |
| `channels` | CHAR[200] | "" (all) |

## Per-user state

`lastuse[nterms]` holds the time of each channel's last use. It is cleared
at logon as well as hang-up, because the board reuses a channel slot as soon
as a caller drops.

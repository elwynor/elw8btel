# 8Ball for Teleconference for The Major BBS v10

The Magic 8-Ball for the stock Major BBS v10 Teleconference. Ask it a
question in any channel and everyone there sees the answer. That includes
users on other boards in a linked channel such as MajorLink.

8Ball for Teleconference is a conversion of **HOTELeconference 8Ball 2.00**
(1997) by Randall R. Spangler of annex.com. The original was an add-on for
annex's HOTELeconference; this version works with the Teleconference that
ships with The Major BBS v10.

## Features

- Adds an `8BALL` command to the stock Teleconference, for terminal and web
  users alike. There is no menu entry or anything else to set up.
- `8BALL Should I log off now?` shows the asker the classic 8-Ball picture
  with the answer inside. Everyone else in the channel sees one line of
  chat with the question and answer.
- **Works over linked channels.** The answer is sent as ordinary chat, so
  it reaches MajorLink (formerly WorldLink) and other linked boards, and
  they don't need 8Ball installed.
- Settings are changed online, with `8BALL SET`, and take effect
  immediately. Changes are written to the Audit Trail.
- Limit the 8-Ball to chosen channels, require a key, and set the wait
  between uses.
- The 21 classic answers are included, with room for 30. Answers are added
  or removed by editing the text.
- Sysops can pick a reply (`8BALL /REPLY3 question`), as in the original.

## Installation

1. Shut down the BBS.
2. Copy `ELW8BTEL.DLL`, `ELW8BTEL.MDF`, `ELW8BTEL.MSG` and `ELW8BTEL.RLN`
   from the release into the BBS folder.
3. Start the BBS. The module loads automatically and adds `8BALL` to
   Teleconference.

Teleconference (GALTELE) is required. The module is a 32-bit (Win32) DLL,
like The Major BBS v10 itself, and runs on 64-bit Windows.

## Configuration

There are no configuration editor options. In Teleconference, a user with
the SYSOP key types `8BALL SET` to list the settings, and
`8BALL SET <number> <value>` to change one:

| # | Setting | Default |
|---|---|---|
| 1 | 8-Ball answers questions | Yes |
| 2 | Key to use the 8-Ball | blank (everyone) |
| 3 | Key to pick a reply and skip the wait | SYSOP |
| 4 | Seconds a user waits between uses | 30 |
| 5 | Channels it works in (comma-separated) | blank (all) |

The text and answers are in `ELW8BTEL.MSG` (level 6 in the configuration
editor). See `ELW8BTEL.RLN` for details.

## Usage

| Command | What it does |
|---|---|
| `8BALL question` | Consult the 8-Ball |
| `8BALL` | Shake it without a question |
| `8BALL ?` | Help |
| `8BALL SET` | Sysop settings |

## Version Information

Current version: **1.0.0**. See [CHANGELOG.MD](CHANGELOG.MD).

## License

Licensed under the GNU Affero General Public License v3.0. See
[LICENSE](LICENSE).

Copyright (c) 2026 Elwynor Technologies. Originally HOTELeconference 8Ball by
Randall R. Spangler, annex.com.

## Support

Report problems through the GitHub issues of this repository.

8Ball converted for The Major BBS v10 by Mark Laudenbach - https://github.com/laudenbachm

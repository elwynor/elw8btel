<div align="center">

```
 █████╗     ██████╗  █████╗ ██╗     ██╗
██╔══██╗    ██╔══██╗██╔══██╗██║     ██║
╚█████╔╝    ██████╔╝███████║██║     ██║
██╔══██╗    ██╔══██╗██╔══██║██║     ██║
╚█████╔╝    ██████╔╝██║  ██║███████╗███████╗
 ╚════╝     ╚═════╝ ╚═╝  ╚═╝╚══════╝╚══════╝
```

**The Magic 8-Ball for the stock Major BBS v10 Teleconference.**

By **[Elwynor Technologies](https://github.com/elwynor)** — https://github.com/elwynor/elw8btel

[![Major BBS](https://img.shields.io/badge/Major%20BBS-v10-blue)](https://github.com/elwynor/elw8btel)
[![License](https://img.shields.io/badge/license-AGPL--3.0-blue?logo=opensourceinitiative&logoColor=white)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Win32-lightgrey?logo=windows&logoColor=white)]()
[![Release](https://img.shields.io/github/v/release/elwynor/elw8btel)](https://github.com/elwynor/elw8btel/releases/latest)

📋 **[See what's new →](CHANGELOG.MD)**

</div>

---

Ask it a question in any Teleconference channel and everyone there sees the
answer, including users on other boards in a linked channel such as MajorLink
(formerly WorldLink).

8Ball for Teleconference is a conversion of **HOTELeconference 8Ball 2.00**
(1997) by Randall R. Spangler of annex.com. The original was an add-on for
annex's HOTELeconference; this version works with the Teleconference that
ships with The Major BBS v10.

---

## ✨ Features

| | Feature | Description |
|---|---|---|
| 🎱 | **`8BALL` command** | Added to the stock Teleconference for terminal and web users alike. No menu entry, nothing else to set up. |
| 🖼️ | **Classic picture** | `8BALL Should I log off now?` shows the asker the 8-Ball with the answer inside. |
| 💬 | **One line for the channel** | Everyone else sees a single chat line with the question and the answer. |
| 🌐 | **Works over linked channels** | The answer is sent as ordinary chat, so it reaches MajorLink and other linked boards, and they don't need 8Ball installed. |
| ⚙️ | **Online settings** | `8BALL SET` changes settings while the board runs. They apply immediately and are written to the Audit Trail. |
| 🔑 | **Access control** | Limit it to chosen channels, require a key, and set the wait between uses. |
| 📝 | **Your own answers** | The 21 classic answers are included, with room for 30. Edit the text to add or remove them. |
| 🛠️ | **Sysop's pick** | Sysops can choose a reply (`8BALL /REPLY3 question`), as in the original. |

---

## 🚀 Installation

1. Download `ELW8BTEL-v1.0.0.zip` from the
   [latest release](https://github.com/elwynor/elw8btel/releases/latest).
2. Shut down the BBS.
3. Copy `ELW8BTEL.DLL`, `ELW8BTEL.MDF`, `ELW8BTEL.MSG` and `ELW8BTEL.RLN`
   into the BBS folder.
4. Start the BBS. The module loads automatically and adds `8BALL` to
   Teleconference.

Teleconference (GALTELE) is required. The module is a 32-bit (Win32) DLL,
like The Major BBS v10 itself, and runs on 64-bit Windows.

### Usage

| Command | What it does |
|---|---|
| `8BALL question` | Consult the 8-Ball |
| `8BALL` | Shake it without a question |
| `8BALL ?` | Help |
| `8BALL SET` | Sysop settings |

---

## ⚙️ Configuration

There are no configuration editor options. In Teleconference, a user with
the SYSOP key types `8BALL SET` to list the settings, and
`8BALL SET <number> <value>` to change one.

<table>
<tr><th>#</th><th>Setting</th><th>Default</th></tr>
<tr><td>1</td><td>8-Ball answers questions</td><td>Yes</td></tr>
<tr><td>2</td><td>Key to use the 8-Ball</td><td>blank (everyone)</td></tr>
<tr><td>3</td><td>Key to pick a reply and skip the wait</td><td>SYSOP</td></tr>
<tr><td>4</td><td>Seconds a user waits between uses</td><td>30</td></tr>
<tr><td>5</td><td>Channels it works in (comma-separated)</td><td>blank (all)</td></tr>
</table>

The text and answers are in `ELW8BTEL.MSG` (level 6 in the configuration
editor). See `ELW8BTEL.RLN` for details.

---

## 📁 Architecture

```
SRC/ELW8BTEL/
├── ELW8BTEL.CPP      Module start-up, the 8BALL command, the settings editor
├── E8_TLC.CPP/H      Link to the Teleconference engine (GALTELE, resolved at run time)
├── E8_SET.CPP/H      Settings record (ELW8BTEL.DAT) and its descriptions
├── E8_UTIL.CPP/H     Text helpers with no BBS dependencies (unit tested)
├── ELW8BTEL.MST      Message template; builds ELW8BTEL.MSG and ELW8BTEL.H
├── ELW8BTEL.MDF      Module definition file
└── ELW8BTEL.RLN      Release notes and sysop manual
TESTS/                Unit tests for the text helpers
```

See [TECHNICAL.MD](TECHNICAL.MD) for how the answer reaches linked boards,
and [BUILDING.MD](BUILDING.MD) to build it yourself.

---

## 🏆 Credits

Converted for The Major BBS v10 by [Mark Laudenbach](https://github.com/laudenbachm)
at [Elwynor Technologies](https://github.com/elwynor).

Originally HOTELeconference 8Ball by Randall R. Spangler, annex.com.


---

## 📄 License

Licensed under the GNU Affero General Public License v3.0. See
[LICENSE](LICENSE).

Copyright (c) 2026 Elwynor Technologies.

Report problems through the GitHub issues of this repository.

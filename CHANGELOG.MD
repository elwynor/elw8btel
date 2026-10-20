# Changelog

All notable changes to 8Ball for Teleconference are recorded here. Versions
follow [Semantic Versioning](https://semver.org/).

## [1.0.0] - 2026-10-19

First release for The Major BBS v10, converted from HOTELeconference 8Ball
2.00 (ANXH8BAL, 1997).

### Added

- The `8BALL` command in the stock Teleconference, for terminal and web
  users.
- The answer reaches linked channels (MajorLink / WorldLink), because it is
  sent as an ordinary chat line.
- Online settings with `8BALL SET`, stored in `ELW8BTEL.DAT`, with changes
  written to the Audit Trail.
- A setting that limits the 8-Ball to chosen channels.
- An on/off setting.
- Up to 30 answers. Blank ones are skipped.
- Text uses named text variables (`E8B_*`).

### Changed (from the original)

- Removed the dependency on annex's HOTELeconference and its loader.
- The CNF options (KEYUSE, KEYFORCE, CMDWORD, RPCOUNT, DELAY, ROOMTYPE) are
  replaced by online settings. The command word is now always `8BALL`. The
  reply count is now the number of non-blank answers. Room types became a
  channel list.
- The answers moved from per-answer pictures to one shared picture with
  the answer text inside it.

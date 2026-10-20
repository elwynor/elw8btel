# Building 8Ball for Teleconference

8Ball for Teleconference is a Win32 DLL module for The Major BBS v10. It is
built with Microsoft Visual Studio against the MBBS v10 Module SDK.

## Prerequisites

- **Microsoft Visual Studio 2022** (or Build Tools 2022) with the C++
  desktop workload. Platform toolset **v143**.
- **Windows 10 SDK** (`WindowsTargetPlatformVersion` 10.0).
- **The Major BBS v10 Module SDK**
  (https://github.com/TheMajorBBS/MBBS-v10-module-SDK).

> **Build for x86 (Win32) only. Never x64.** The Major BBS v10 is a 32-bit
> program and cannot load a 64-bit module.

## Setup

Copy `MBBSSDK.props.example` to `MBBSSDK.props` and set `MbbsSdkDir` to your
SDK folder (with the trailing backslash). `MBBSSDK.props` is not committed.

## Two things that differ from other modules

- **No Teleconference import library.** The module finds the four
  Teleconference functions it uses in `GALTELE.DLL` by name when the board
  starts (`E8_TLC.CPP`). The SDK's `GALTELE_LIB.LIB` names a different DLL
  and different export numbers than the real `GALTELE.DLL`, so a module
  linked with it does not load. Don't add that library to the project.
- **`TLCAPI.H` is not included.** One of the files it pulls in (`TLCACT.H`)
  does not compile with Visual Studio. `E8_TLC.H` includes only the
  Teleconference structure headers it needs.

## Message file

The module's text is in `SRC/ELW8BTEL/ELW8BTEL.MSG`, and
`SRC/ELW8BTEL/ELW8BTEL.H` holds the matching message numbers. Both are
generated from `ELW8BTEL.MST`, which spells control bytes as readable
tokens (`<ESC>`, `<TV:NAME>`, `<TV:NAME:C:12>`, `<HEX:DB*3>`). The `.MSG` must
keep CRLF line endings. Message numbers are assigned in file order,
starting from 0, and the `LEVEL6` marker takes slot 0. Deploy the DLL and
`.MSG` together; the BBS compiles `ELW8BTEL.MCV` itself.

## Build

Open `ELW8BTEL.sln`, choose **Release** / **Win32**, and build. Or:

```
MSBuild.exe ELW8BTEL.sln -p:Configuration=Release -p:Platform=Win32
```

The DLL is written to `BUILD/Release/ELW8BTEL.dll`.

## Tests

`TESTS\RUN_TESTS.CMD` builds and runs the unit tests for the text helpers
(`E8_UTIL.CPP`): question cleanup, ANSI removal, the channel list, and
settings input. Exit code 0 means every test passed. The Teleconference
behaviour is tested on a BBS.

## Packaging a release

1. Set the new version in `SRC/ELW8BTEL/E8_DEFS.H` (`ELW8BTEL_VER_*`,
   `ELW8BTEL_VERSION`), `SRC/ELW8BTEL/ELW8BTEL.rc`, the `.MDF` and `.MST`
   headers, and `ELW8BTEL.RLN`. Update `CHANGELOG.MD`.
2. Rebuild the Release configuration, and check that the DLL's version
   matches `ELW8BTEL_VERSION`.
3. Copy only the deployable files into `DIST/<version>/`: `ELW8BTEL.DLL`,
   `ELW8BTEL.MDF`, `ELW8BTEL.MSG` and `ELW8BTEL.RLN`. Zip them as
   `ELW8BTEL-<version>.zip`.

Never rebuild under a version number that has already been released.
Bump the version instead.

## Release checklist

- The project builds with no errors or warnings, and the unit tests pass.
- Version numbers are updated (`E8_DEFS.H`, `ELW8BTEL.rc`, `CHANGELOG.MD`,
  `ELW8BTEL.RLN`).
- `DIST/<version>/` holds only the deployable files, and the MDF, MSG and
  RLN files have CRLF line endings.
- The DLL's properties show the right product, version, company and
  copyright.
- Installation is tested on a test board, including a linked channel.
- No credentials, internal paths or internal notes are in published files.

## About

Pivot is an addon that allows to dynamically override parts of the ROM and sound
files of your FFXI client without touching the actual files.

It provides support for multiple overlay directories as well as runtime controls
of which overlays are loaded and used by the game.

The goals behind Pivot are simple:

- no modifications to the original DAT / spw or bgw files
- no more huge folder with thousands of anonymous files
- the ability to sort Mods into separate folders for each mod
- the ability to control which Mods take precedence

## Compatibility

XIPivot works together with [Ashita v3](https://www.ashitaxi.com), [Windower 4](http://www.windower.net) as well as
the current beta version of [Ashita v4](https://github.com/AshitaXI/Ashita-v4beta).

## Installation

#### Manual and Windower Install

- Head over to the [Releases page](https://github.com/Shirk/XIPivot/releases) and pick the ZIP that matches your launcher
- Follow the README.md inside the archive  for setup and configuration instructions

- [Latest release for Windower](https://github.com/Shirk/XIPivot/releases/tag/v0.4.7)
- [Latest release for Ashita](https://github.com/Shirk/XIPivot/releases/tag/v0.4.7)
- [Latest pre-release for Ashita v4 beta](https://github.com/Shirk/XIPivot/releases/tag/v4.1.104)

#### Ashita Install

The latest version of Pivot can always be installed from the plugins list in Ashita and will be updated automatically.

### Ashita v4 (beta) Install

Follow the steps for manual installation for now.

## Building from Source

If you would like to build Pivot from source you need the Visual Studio 2020 or newer.
The Community Edition is available for free from [Microsoft](https://visualstudio.microsoft.com/vs/community/).

Otherwise it's just these three steps:

- clone the repository
- open the solution
- chose "Build -> Rebuild Solution" .

The addon and plugin versions will be placed inside the directories below `build\Release\`.

## Contributions

Contributions in the form of new interfaces (xiloader would be one *hint hint*) are always welcome
as long as they don't modify the current interface exposed by the XiPivot::Core:: namespace.

Likewise feature requests are always welcome, with reason.

Pivot has one job - to intercept XI's DAT access and XI's - and it aims to do it in a very
specific and as conservative way as possible.

The classes below the XiPivot::Core:: namespace are considered stable and code-complete and do not currently
accept random contributions or pull-requests outside of compatibility fixes in case the game client changes.

Especially the Redirector class has been written based on a number of very specific implementation details of
the game client and changes to this class can cause any number of unexpected side effects. Further it has to be
compatible with all possible interface scenarios including to be loaded way before the actual game client (see
injection inte the POL process as is done in Ashita v4 right now).

## Disclaimer

I tested Pivot to the best of my capabilities but I can not guarantee that it works without bugs for 100% of the time.
Use at your own discretion, I take no responsibility for any client crashes or data loss.

## About

XIPivot is an addon that allows to dynamically override parts of the ROM and sound
files of your FFXI client without touching the actual files.

It provides support for multiple overlay directories as well as runtime controls
of which overlays are loaded and used by the game.

The goals behind XIPivot are simple:

- no modifications to the original DAT / spw or bgw files
- no more huge folder with thousands of anonymous files
- the ability to sort Mods into separate folders for each mod
- the ability to control which Mods take precedence

## Compatibility

XIPivot works together with [Ashita v3](https://www.ashitaxi.com) as well as [Windower 4](http://www.windower.net).

## Installation

#### Manual and Windower Install

- Head over to the [Releases page](https://github.com/Shirk/XIPivot/releases) and pick the ZIP that matches your launcher
- Follow the README.md inside the archive  for setup and configuration instructions

- [Latest release for Windower](https://github.com/Shirk/XIPivot/releases/tag/v0.4.6)
- [Latest release for Ashita](https://github.com/Shirk/XIPivot/releases/tag/v0.4.5)
- [Latest pre-release for Ashita v4 beta](https://github.com/Shirk/XIPivot/releases/tag/v4.0.902)

#### Ashita Install

The latest version of XIPivot can always be installed from the plugins list in Ashita and will be updated automatically.

### Ashita v4 (beta) Install

Follow the steps for manual installation for now.

## Modifications / Contribution

If you would like to build XIPivot from source you need the Visual Studio 2019.
The Community Edition is available for free from [Microsoft](https://visualstudio.microsoft.com/vs/community/).

Otherwise it's just these three steps:

- clone the repository
- open the solution
- chose "Build -> Rebuild Solution" .

The addon and plugin versions will be placed inside the directory `build\Release\Windower` and `build\Release\Ashita`.

## Disclaimer

I tested XIPivot to the best of my capabilities but I can not guarantee that it works without bugs for 100% of the time.
Use at your own discretion, I take no responsibility for any client crashes or data loss.

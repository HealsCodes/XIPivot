## About

XIPivot is an addon that allows to dynamically override parts of the DAT / sound files
of your FFXI client without touching the actual files (just like Dats but with extras).

It provides support for multiple overlay directories as well as runtime controls
of which overlays are loaded and used by the game.

The goals behind XIPivot are simple:

- no modifications to the original DAT / spw / bgw files
- no more huge folder with thousands of anonymous files
- the ability to sort Mods into separate folders for each mod
- the ability to control which Mods take precedence

## Setup

- 1) Add 'pivot' to the list of plugins to load in your ashita profile's `[ashita.polplugins]` section
- 2) If this is your first time setting up pivot rename `pivot.sample.ini` in Ashita's `config/pivot/` directory to `pivot.ini`.
- 3) Create a subdirectory inside the `polplugins\DATs\` folder for each overlay you want to use
- 4) -- manual approach -- jump to 6 for in-game GUI version
   - Edit `pivot.ini`:
   - add the directory names from 3) for each overlay that should be enabled in the form "I=overlay_name".
     - _I_ in the above example is a positive number, starting at 0 that determines when to load this overlay
     - numbers mist be successive as loading will stop at the first number not listed
   - *order matters* - overlays will be used in order, with the first hit for a file being used.
- 5) -- OPTIONAL --
   - Instead of following 4) you can use the command `/pivot` in-game to open the configuration GUI

## Using a custom configuration in your ashita profile

If required you can specify the basename for pivots configuration file (the default is 'pivot').
This can be used to have different pivot settings for different Ashita .ini files:

- 1) Open your ashita profile and find the section `[ashita.polplugins.args]`
- 2) add a line like this: `pivot = foobar`

In this case `foobar` is the name of your custom configuration and pivot will look for `<ashita>\config\pivot\foobar.ini`.


### Example - XI-View, XI-Vision and Ammelila's HD-Remake:

Directory structure:

```
<Ashita Install>
   +-- config/
   |    +-- pivot/
   |         +-- pivot.ini
   |
   +-- docs/
   |    +-- pivot/
   |         +-- README.md
   |
   +-- polplugins/
        +-- pivot.dll
        +-- DATs/
             +-- README.txt
             +-- XI-View/
             |    +-- ROM/
             |
             +-- XI-Vision/
             |    +-- ROM/
             |    +-- ROM2/
             |    +-- ROM3/
             |    +-- ROM4/
             |    +-- ROM4/
             |
             +-- FFXI-HD/
                  +-- ROM/
                  +-- ROM2/
                  +-- ROM3/
```

pivot.ini:

```ini
[settings]
debug_log=false

[overlays]
0=XI-View
1=XI-Vision
2=FFXI-HD
```

Pivot defaults to look for your overlays inside the `DATs/` directory next to the `pivot.dll`.
If you prefer a different path you can add the key `'root_path'` to `pivot.ini` like so:

```ini
[settings]
debug_log=false
root_path=C:\my\awesome\addons

[overlays]
0=XI-View
1=XI-Vision
2=FFXI-HD
```

This instructs to pivot to search for overlays inside `"C:\my\awesome\addons"` instead.
(If you're unsure **don't set this parameter** and things should just work).

## In-Game commands

Pivot makes the in-game command `/pivot` available to load and unload overlays at runtime.
If the command `/pivot` is used without parameters it will open an in-game GUI for configuration.

The following parameters are supported:

- d/dump                 -- write the current overlay list and cache settungs to (`<Ashita>\logs\pivot-dump.txt`)
- h/help                 -- print this text

These commands all support a short first letter version (c/h).
Changes made through the configuration UI will be reflected in `pivot.ini` automatically.


Please note that adding and removing overlays way after the game launches can have side effects.
XI will load some DAT files right at the start and then never look at them again (some menu and landscape textures)
other DAT files are loaded on-demand and overlay changes are visible once that happens (maps, some menu icons, Mog House and a few other locations)

## Overlays with sound files / music

XI is pretty unforgiving when replacing BGW music files at runtime and will crash if you do something stupid.

**It is not advised to change sound Mods at runtime**
**Support for sound / music overlays is considered EXPERIMENTAL**

## Limitations

Currently none known

## Disclaimer

I tested Pivot to the best of my capabilities but I can not guarantee that it works without bugs for 100% of the time.
Use at your own discretion, I take no responsibility for any client crashes or data loss.


# LavenderHook - QoL DDA Automation Software

LavenderHook is an in-game overlay for the sole purpose of making AFKing
in Dungeon Defenders: Awakened easier by providing tools that automatically
perform actions or help with a few in game issues.

----------------------------------------------------------------------------------

# Installation

Browse your Dungeon Defenders: Awakened installation directory something like:

`steamapps\common\Dungeon Defenders Awakened`

You may drop the **Engine** folder in and replace files. Otherwise,
manually place the files as follows:

Replace:\
`Dungeon Defenders Awakened\Engine\Binaries\ThirdParty\Ogg\Win64\libogg_64.dll`\
`Dungeon Defenders Awakened\Engine\Binaries\ThirdParty\Ogg\Win64\PluginLoader.ini`

Create Plugins folder and add:\
`Dungeon Defenders Awakened\Engine\Binaries\ThirdParty\Plugins\LavenderHook.dll`

After installation, launching the game normally should automatically
load the overlay.

----------------------------------------------------------------------------------

# Main Features

## Menu Access

Press **Insert** or **CTRL + F1** to open the overlay menu.

## Exit Button

Force closes the game when held long enough.\
Note: It will **NOT** cause the game to save the data before closing.


## Settings Button

Opens the main settings menu.


## Settings

The settings window will have the main settings for the entire overlay. These
include:

### Main Settings

- **Info Overlay**\
When enabled, an indicator of active buttons will be displayed in the top-right
corner of the process.

- **General Window**\
When enabled, the General window will be visible; otherwise, invisible.

- **Misc Window**\
When enabled, the Misc window will be visible; otherwise, invisible.

- **Console**\
When enabled, the Console will be visible; otherwise, invisible.

- **Menu Logo**\
When enabled, the menu logo (randomized from 8 different chibi witch girl images)
will be displayed in the bottom-left corner of the process.

- **Stop on Fail**\
When enabled, it will stop all running buttons associated with input automation
when a core gets destroyed and also play a sound. (Note: Only when hosting.)

- **Process Overlay on Hide**\
When enabled, it will display a small video overlay that is uninteractable outside
of moving it around and resizing it when the Hide Window button is currently enabled.

- **Performance Overlay**\
When enabled, it will display resource stats of the computer, which can also
individually be toggled on or off, such as:\
FPS / Frames per Second\
RAM Usage\
CPU Usage\
GPU Usage

### Audio Settings

- **Volume Slider**\
A volume slider for all the overlay sounds.

- **Mute Button Clicks**\
Mutes the sounds when a button gets clicked or toggled on/off.

- **Mute Stop on Fail**\
Mutes the Stop on Fail sound that plays.

### Theme Colors

- **R G B Colors**\
RGB colors for the main, alt, and bright colors. These affect the window icons,
sliders, hovered/active buttons, window buttons, main menu buttons, checkbox buttons,
process overlay, and notifications.

- **Reset to Default**\
A button that resets the theme colors back to their default values.

----------------------------------------------------------------------------------

## General Window

This window has the main buttons for input automation, all of which are also configurable
to some extent. These may include:

- **Ready Up**\
By default: Automatically holds down G for 2 seconds and then pauses for 3 seconds.\
This is used to automatically ready up for the next wave.

- - You may configure:\
Hotkey - Up to 2 combo hotkeys to quickly toggle the button. (ESC binds to None.)\
Key - The key it holds down.\
Hold - The duration the key is held down for.\
Delay - The time it pauses after holding down the key before repeating its actions.

- **Flash Buff**\
By default: Automatically presses F once every 8 seconds.\
This is used to automatically use the Flash Heal ability on Summoner along with the\
Flash Buff rune to buff your defenses.

- - You may configure:\
Hotkey - Up to 2 combo hotkeys to quickly toggle the button. (ESC binds to None.)\
Key - The key it holds down.\
Interval - The time between each repeating action.

- **Force Ready Up**\
By default: Automatically holds down CTRL + G for 5 seconds and then pauses for 10 seconds.\
This is used to automatically force the team to ready up for the next wave.

- - You may configure:\
Hotkey - Up to 2 combo hotkeys to quickly toggle the button. (ESC binds to None.)\
CTRL - The primary key it holds down.\
G - The secondary key it holds down.\
Hold - The duration the keys are held down for.\
Delay - The time it pauses after holding down the keys before repeating its actions.

- **Skip Cutscene**\
By default: Automatically holds down Space for 2 seconds and then pauses for 4 seconds.\
This is used to automatically skip cinematics, mainly boss cinematics to save time.

- - You may configure:\
Hotkey - Up to 2 combo hotkeys to quickly toggle the button. (ESC binds to None.)\
Key - The key it holds down.\
Hold - The duration the key is held down for.\
Delay - The time it pauses after holding down the key before repeating its actions.

- **Auto Clicker**\
By default: Automatically presses down the right mouse button once every 100 milliseconds.\
This is mainly used to upgrade a large number of towers.

- - You may configure:\
Hotkey - Up to 2 combo hotkeys to quickly toggle the button. (ESC binds to None.)\
Interval - The time between each repeating action.


----------------------------------------------------------------------------------

## Misc Window

This window has functions that are not directly tied to automation functions but rather QoL
functions.

- **Flash Heal Fix**\
This will simulate a forced cursor in the center of the process. This is mainly to help
Flash Heal not bug out and perform a sideways Flash Heal that does not actually heal
anything when the main cursor is outside of the window.

- - You may configure:\
Hotkey - Up to 2 combo hotkeys to quickly toggle the button. (ESC binds to None.)

- **Always Cursor**\
This will always display a custom cursor in the game. This is mainly used when the player
cursor, specifically in the Summoner's Overlord mode, suddenly becomes invisible and
hard to locate the actual cursor position.

- - You may configure:\
Hotkey - Up to 2 combo hotkeys to quickly toggle the button. (ESC binds to None.)

- **Hide Window**\
This will turn the window invisible and uninteractable (or far off the screen in certain cases)
to keep the window still active and allow the functions and game to progress correctly,
unlike minimizing the game. When enabled, it will display a small notification at the start
telling you that an icon in the system tray will appear that can be used to restore the
window back to its original form.

- - You may configure:\
Hotkey - Up to 2 combo hotkeys to quickly toggle the button. (ESC binds to None.)

----------------------------------------------------------------------------------

# Licensing

## LavenderHook License (MIT License)

This project is licensed under the MIT License.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.

----------------------------------------------------------------------------------

# Third-Party Libraries

All Licenses can be found under the `ThirdParty Licenses` folder or their respective
directories.

## MinHook

MinHook is licensed under the 2-clause BSD License. It allows free use,
modification, and redistribution with attribution.

## miniaudio

miniaudio is released into the public domain under the Unlicense. It may
be freely used, modified, and distributed without restriction.

## Dear ImGui

Dear ImGui is licensed under the MIT License. You may freely use,
modify, and distribute it under MIT terms.

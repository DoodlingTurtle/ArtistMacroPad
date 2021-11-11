# ArtistMacroPad
An application inspired by the Windows only TabletPro ArtistPad.
Basically an artist pad for linux.

This application is optimized for Linux Tablets and Touchscreens you will need to install touch drivers for your Device.
(meaning I can't garantie it will work on all Linux installations, but it is free, so there is nothing to lose by trying 😉).

As the backend for this application I'm using xdotool to send mouse and keyboard command to the X-Server.

This application is still in a very early alpha stage so if you want to help me to squash bugs, I would be more then happy about feedback. Either here on github or on Twitter @TurtleDoodling


# Installation

Grab the x86, 64bit precompiled binary in the release section: https://github.com/DoodlingTurtle/ArtistMacroPad/releases

, or compile the project by your self.

There are 3 dependencys / packages that must be installed in addition (both for building the project and for running it). 
SDL2, SDL2Image and xdotool

on Linux Mint / Ubuntu / Debian you can install them like so:
```bash
sudo apt install libsdl2-dev libsdl2-image-dev xdotool
```

after that, you can use the `launcher.sh` of this project, to start the application.

# Defining a profile
If you start the ArtistMacroPad (refered to as AMP from now on) you must give it a Profile to start. 

The `laucher.sh` will allow you to select a Profile based on a Window you can select.

if you want to start the AMP with as specific window in mind, you can call the `main` or `debug.main` binary and give the path to the profile - ini as the first parameter.

For example for blender or for kdenlive:
```bash
cd [directory where you have saved the AMP application]
./debug.main ./assets/Blender.ini
or
./debug.main ./assets/kdenlive.ini
```
if the file you want to use does not exist, the application will let you know.
Example:
```
Error: Could not find ini file 'assets/Mate-terminal.ini'
```

# Creating a Profile
The ./assets folder of the application contains a few example profiles.

**WARNING**: The profiles are used to execute shell commands. 
so please **check all ini files you get from other sources before using them**.

here is an example for what a profile ini could look like:
```ini
[window]
width = 96
height = 480

[Frame Left]
label = F - L 
command = key Left 
x = 0 
y = 41
w = 50 
h = 7 

[Frame Right]
label = F - R 
command = key Right 
x = 50 
y = 41
w = 50 
h = 7 

[Key Z]
label = Z
command = key z
group = Undo # Redo
x = 0
y = 7
w = 50
h = 7

[#Undo]
label = Undo 
command = key ctrl+z
x = 0
y = 0
w = 50
h = 7
r = 128 
g = 0 
b = 64 

[#Redo]
label = Redo 
command = key ctrl+shift+z
x = 0
y = 14
w = 50
h = 7
r = 128 
g = 0 
b = 64 
```

## The `[window]` section is mandetory
it will define the area, you can place your buttons in.
the values are in pixels.
Keep in mind, that the window will also try to scale it self acording to your screens dpi.
So windows get scaled up on high dpi displays and scaled down for low dpi displays.
(This was made, so that the window will always have roughly the same visual size, when you look at it)
You can also scale the window afterwards.

## Defing buttons
Every ini group that is not `[window]` is defining a button. 

The group name `[Press the left key]` is for readability. If you  define multiple groups with the same name, they will override each other. 
Groups that start with a `#` (example `[#hidden group/button]`) will be ignored.

`label` is what is shown on the buttons.
`command` will be executed, as soon as you interact with the button. (see the Command section for more details).

`x`, `y`, `w`, `h` are the x/y position, with and height of the button inside the window area. all values are percentages from 0-100 (they need to be Integers though).

`r`, `g`, `b` are for red, green and blue these values are from 0 - 255 and define the color of the button.
They are optional if they are not set, they will be given the value 96

## Group Buttons via the `group` property
If the button contains a `group` property, it will become a "Group Button" 
that means, its action only triggers uppon release.
If the Button is pressed all "Hidden Buttons" listed in this property will be shown as long as the mouse is held down.
When the mouse releases, the command of the button below the mouse is executed.
`group` can contain multiple Button definitions thouse must be separated via '#' symbol. 
(see the "Key Z" button group in the example above)

In the exaple above, the "Undo" and "Redo" Buttons are only shown, if the "Z" Button is pressed down.

# Commands 
Each button can trigger one or more commands.
Each command consists of the first keyword followed by the words parameter.

You can chain multiple commands together via the `#` symbol

available words are:
 - `key`  presses and releases a key
 - `keydown` presses a key, but does not release it
 - `keyup` releases a pressed key
 - `sleep` waits for a number of seconds (the number is a float, so you can do `sleep 0.5` for a break of half a second)
 - `term` will allow you to execute any shell command you want **as said previously be carefull with profiles you did not write your self**

All `key...` commands follow the xdotool guidelines.
https://manpages.ubuntu.com/manpages/trusty/man1/xdotool.1.html

Quote:
```
Generally, any valid X Keysym string will work. Multiple keys are separated by '+'.
Aliases exist for "alt", "ctrl", "shift", "super", and "meta" which all map to Foo_L,
such as Alt_L and Control_L, etc. 
```
Available keys can be found here: https://gitlab.com/cunidev/gestures/-/wikis/xdotool-list-of-key-codes
 

## Command examples
Press the "a" key
```ini
command = key a 
```

Undo 
```ini
command = key ctrl+z 
```

Hold ALT for 2 Seconds (for example to pick a color in your paint programm of choice)
```ini
command = keydown alt # sleep 2 # keyup alt
```

Hold Ctrl+z for half a second (no idea why but its an option)
```ini
command = keydown ctrl+z # sleep 0.5 # keyup ctrl+z
```

Launch another program ( in my example the "OnBoard" Onscreen keybaord)
```ini
command = term onboard
```

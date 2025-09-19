
# THUG SDL

A mod to improve the gameplay experience of the PC version of Tony Hawk's Underground. It adds stability and usability fixes, utilizes the SDL2 game library and offers an open base for script-mod development.

## How to install

Download the latest version from the relase tab and extract the files from the zip-archive into the game directory.<br>
 - Default game directory: `C:\Program Files (x86)\Activision\Tony Hawk's Underground\Game`

Run `thugsdlpatcher.exe` in the Game directory where THUG.exe should already be. It will generate THUG-SDL.exe which will be used to start the game from now on. This only needs to be done once.<br>There is also a brief [video](https://youtu.be/717bshkWhMw) showcasing the process.<br><br>*Important: This only works for the No-CD THUG.exe, which is approximately 2.6 MB in size.*

## Features

### General

- Unofficial port of [partymod-thaw](https://github.com/PARTYMANX/partymod-thaw)
- Replaced input system entirely with new, modern system using the SDL2 library
- Improved window handling allowing for custom resolutions and configurable windowing (4k resolution, (borderless) windowed mode)
- Minimal launcher to configure game and mod settings
- In game mod settings menu
  - Saves to and loads from thugsdl.ini automatically
- Stability fixes
- Increased default clipping distance for large levels
- Added pause buffer for speedrunning (menu to game transition with pause button held)
- Option to toggle blur effects
- Option to toggle the fullscreen gamma correction which makes the game looks too dark
- Restored the onscreen keyboard
- Increased speed when exiting the game
- Modloader for script mods: [Section Modloader](#modloader)
- Supported languages: English, French, German
- Adjustable button prompts: PC, Ps2, Xbox, Ngc
- Console window with adjustable log level
- Option to print the console output to a file (overwrite / append)
- Intro movie skip
- Higher quality shadows
- Toggle for boardscuffs
- Option to disable all script mods (speedrunner mode)
- Option to store the window position
- Create-A-Skater: Increased 3 axes scaling
- Create-A-Skater: Visible scaling numbers
- Create-A-Skater: Increased color ranges
- Create-A-Skater: Board scaling
- Exception Handler to generate crash reports (may not work on Windows 11)
- Removed player limits for Created Parks
- Option to enable free cam from the Select button (only during observe mode in net games)
- Buttslap counter
- Enable leftover levels: School, Philadelphia, Downhill Jam

### Gameplay
- Land pivots (including the ability to buttslap or air manual after)
- Single tap THPS4 Boostplants

### Controls
- Native gamepad support
- Restore original menu navigation for Xbox and Ps2 layouts
- Option to use "grind" in menus to go back (original Ps2 layout)
- Support for Ps2 controls on gamepad and keyboard
- Fix incorrect button prompts in textboxes and helper texts
- Adjustable keys for Rail Drop Downs including directional DDs
- Adjustable keys for Caveman
- Adjustable keys for ladder/rail grab
- Option to invert the x and/or y camera axis
- Option to disable the x and/or y camera axis

### Multiplayer
- Openspy support
- Replace GameSpy with OpenSpy in texts and images
- Multiplayer Observe menu 
  - Choose next / previous on ollie / grab or arrow keys
  - Warp to players during freeskate
  - Toggle hud
  - Possible to start observe during games and rejoin freeskate after a round ended
- Menu to load online settings
- Longer text input in quick chat
- No chat reset after net games started / ended
- Removed short freeze time before level changes
- Option for chat size scaling
- Option for chat message time
- Restart game option added to the ranking screen and pause menu


## MODLOADER
This allows you to load user-defined `.pre` and `.qb` files from a separate folder. 
Activate "Additional Mods" in the launcher's General Page and define your folder path relative to the game folder: `data/pre/mymod`.
- Place mod contents inside your mod folder at `C:\<thug-install-path>\Game\Data\pre\mymod`  
- Create a `mod.ini` file in that folder (it defines which original files you want to replace with your custom ones)

**THUG-SDL uses its own script mods by default. Select "Disable Script Mods" in the launcher or your changes could get overwritten!**<br><br>

Example mod.ini:
```
[MODINFO]
Name=My Custom Mod

[PRE]
qb.pre=modded_qb.pre
anims.pre=modded_anims.pre
netanims.pre=modded_netanims.pre

[QB]
scripts\game\skater\airtricks.qb=modded_airtricks.qb
scripts\game\skater\manualtricks.qb=modded_manualtricks.qb
levels\mainmenu\mainmenu_scripts.qb=modded_mainmenu_scripts.qb
```
The folder contents should look like this:
<br>![image](https://github.com/atljp/thps-modding-resources/blob/main/img/custommod_screenshot1.png)

### Levels
Level files cannot be loaded automatically from the mod folder without adjusting the `load_level` script.<br>
There need to be additional checks for a "custom_folder" parameter, see the example file here: [Levels.q](https://github.com/atljp/THUG-SDL/blob/main/src/Mod/Levels.q)<br><br>
When creating the level struct, add the paths and the "custom_folder" parameter like this:
```
File: scripts\Game\Levels.q

Level_AU = { 
	 pre = "mymod\\Levels\\AU.pre"
	 scnpre = "mymod\\Levels\\AUscn.pre"
	 colpre = "mymod\\Levels\\AUcol.pre"
	 pedpre = "mymod\\Levels\\AUped.pre"
	 [...]
	 custom_folder = "mymod\\Levels\\"
}
```

## TROUBLESHOOTING

### Issues with write permissions

The game settings are stored in `thugsdl.ini` which needs write permission when you change and save settings. <br>

If the directory your game is installed to is protected, e.g. Program Files (x86), the ini file needs to be located in  
`C:\Users\<name>\AppData\Local\VirtualStore`. <br>The game will use the VirtualStore location to load and save settings.

### Gamepad bindings and controls

The control settings in the launcher expect the default gamepad binds. When you change to a non-default setup and want to change the controls, you have to account for the bind changes.<br>
Example 1:
- Default Ps2 layout: Spin left on L1, Spin right on R1. Nollie on L2, Switch/Revert on R2
- - In your custom binds you want to swap L1 <-> L2 and R1 <-> R2. This can be done directly in the "Gamepad" tab in the launcher
- - When you want to set the Caveman key to be on the L1 key, you would have to select L2 in the "Controls" tab in the launcher

Example 2:
- Default Xbox layout: Spin left and Nollie on L2, Spin Right and Revert on R2, Caveman on either L1 or R1
- - In your custom binds you want Spin left and Nollie on L1, Spin right and revert on L2, Caveman on R1 and Dropdown on R2.
- - In the "Gamepad" tab in the launcher, set Spin left on L1, Spin right on L2, Caveman on R2
- - In the "Controls" tab in the launcher, set Caveman to R1 and Drop down to L1 (you moved the default bind from L1 to R2 so it will apply R2). The controls tab only knows about the default binds.

## BUILDING FROM SOURCE

### Prerequisites
The build requires the following tools:
- [CMake](https://cmake.org/)
- [Visual Studio](https://visualstudio.microsoft.com/)
- [Recommended: git](https://git-scm.com/download/win)
- [vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-cmd)
- [SDL2](https://www.libsdl.org/)

First download and install Visual Studio and CMake like any other GUI application.<br>
Install git via the Installer from the website or directly in Powershell:
- `winget install --id Git.Git -e --source winget`

Then install vcpkg via git as described in the Microsoft documentation:
- `git clone https://github.com/microsoft/vcpkg.git`
- Run the bootstrap script `cd vcpkg && .\bootstrap-vcpkg.bat`

Install SDL2 via vcpkg: `.\vcpkg.exe install SDL2:x86-windows`, for x64: `.\vcpkg.exe install SDL2`

### Generating the project file
Create a THUG-SDL/build directory. Then build the VS Solution with CMake from that directory via Powershell:
```
cmake .. -A win32 -DCMAKE_TOOLCHAIN_FILE=C:/[vcpkg directory]/scripts/buildsystems/vcpkg.cmake
```
Set the optimization for the DLL to O0 (disable optimization) because MSVC seems to break certain functions when optimization is enabled.
<br><br>
The `.sln` file is in the `build` directory. Make sure to select win32 and Release as the project's build options in Visual Studio. Build the actual DLL with `Ctrl+Shift+B`.
<br><br>
When building the launcher, make sure to set the SubSystem to Windows. In Visual Studio go to the project properties, Linker, System.

## CREDITS

- PARTYMANX: Original development of Partymod. Thanks for the help and patience!
- %.gone.: Clownjob'd mod for THUG which provided useful insights
- Zedek the Plague Doctor ™: LazyStruct and LazyArray classes, Logger and ExceptionHandler and lots of script-related helper code
- Drake9X: Scaling values in C-A-S scale sliders
- Hardronix: DirectX9 and general code support
- Edem: Gameplay and script support
- Thanks to everyone who tested this mod during development!

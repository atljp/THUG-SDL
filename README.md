
# THUG SDL

A mod to improve the gameplay experience of the PC version of Tony Hawk's Underground. It adds stability and usability fixes, utilizes the SDL2 game library and offers an open base for script-mod development.

## How to install

Download the latest version from the relase tab and extract the files from the zip-archive into the game directory.<br>
 - Default game directory: `C:\Program Files (x86)\Activision\Tony Hawk's Underground\Game`

Run `thugsdlpatcher.exe` in the Game directory where THUG.exe should already be. It will generate THUG-SDL.exe which will be used to start the game from now on.<br>This only needs to be done once.<br><br>*Important: This only works for the No-CD THUG.exe which is around 2.6 MB in size*.

## Features

### General

- Unofficial port of [partymod-thaw](https://github.com/PARTYMANX/partymod-thaw)
- Replaced input system entirely with new, modern system using the SDL2 library
- Improved window handling allowing for custom resolutions and configurable windowing (4k resolution, (borderless) windowed mode)
- Minimal launcher to configure game and mod settings
- Stability fixes
- Increased default clipping distance for large levels
- Added pause buffer for speedrunning (menu to game transition with pause button held)
- Option to toggle blur effects
- Option to toggle the fullscreen gamma correction which makes the game looks too dark
- Restored the onscreen keyboard (gamepad only)
- Increased speed when exiting the game
- Modloader for script mods: [Section Modloader](#modloader)
- Supported languages: English, French, German
- Adjustable button prompts: PC, Ps2, Xbox, Ngc
- Console window with adjustable log level
- Option to print the console output to a file (overwrite / append)
- Intro movie skip
- Higher quality shadows
- Toggle for boardscuffs
- NoAdditionalScriptMods flag (disables additional modifications)
- Option to store the window position
- Create-A-Skater: Unlimited 3 axes scaling
- Create-A-Skater: Increased color ranges
- Create-A-Skater: Board scaling
- Exception Handler to generate crash reports (may not work on Windows 11)

### Controls
- Native gamepad support
- Restore original menu navigation for Xbox and Ps2 layouts
- Option to use "grind" in menus to go back (original Ps2 layout)
- Support for Ps2 controls on gamepad and keyboard
- Fix incorrect button prompts in textboxes and helper texts
- Adjustable keys for Rail Drop Downs
- Adjustable keys for Caveman
- Adjustable keys for ladder/rail grab
- Option to invert the x and/or y camera axis
- Option to disable the x and/or y camera axis

### Online
- Openspy support
- Replace GameSpy with OpenSpy in texts and images
- Menu to load online settings
- Longer text input in quick chat
- No chat reset after net games started / ended
- Removed short freeze time before level changes
- Option for chat size scaling
- Option for chat message time


## MODLOADER
This allows you to load user-defined `.pre` and `.qb` files from a separate folder. 
Activate "Additional Mods" in the launcher's General Page and define your folder path relative to the game folder: `data/pre/mymod`.
- Place mod contents inside your mod folder at `C:\<thug2-install-path>\Game\Data\pre\mymod`  
- Add the file `mod.ini` to your folder (it defines which original files you want to replace with your custom ones)
- Level data has to be treated differently which is described below

Example mod.ini:
```
[MODINFO]
Name=My Custom Mod

[PRE]
qb.pre=modded_qb.pre
anims.pre=modded_anims.pre
netanims.pre=modded_netanims.pre

[QB]
game\skater\airtricks.qb=modded_airtricks.qb
levels\mainmenu\mainmenu_scripts.qb=modded_mainmenu_scripts.qb
scripts\game\skater\manualtricks.qb=modded_manualtricks.qb
```
It should look like this:
<br>![image](https://github.com/atljp/thps-modding-resources/blob/main/img/custommod_screenshot1.png)

### Levels
For this example it is assumed that level files are placed inside the `data\pre\mymod\Levels` folder.<br><br>
The paths for levels are defined in the `scripts\Game\Levels.q` script.<br>
To make the game load a level from your new folder, add the path to the pre file names and also add the `custom_folder` parameter:
```
Level_AU = { 
	 pre = "mymod\\Levels\\AU.pre"
	 scnpre = "mymod\\Levels\\AUscn.pre"
	 colpre = "mymod\\Levels\\AUcol.pre"
	 pedpre = "mymod\\Levels\\AUped.pre"
	 [...]
	 custom_folder = "mymod\\Levels\\"
}
```


For netgames, the "scn_net" and "scn_col" suffixes are automatically added to the pre filename in the `load_level` script. <br>It needs to be adjusted so that it adds the custom path for these files as well. **This only needs to be done once.**

```
SCRIPT load_level
  [...]
  IF gotparam custom_folder
    printf "@@@ LOADING SCN_NET FROM CUSTOM PATH" 
    LoadLevelPreFile ( <custom_folder> + <level> + "scn_net.pre" )
  ELSE
    LoadLevelPreFile ( <level> + "scn_net.pre" )
  ENDIF
  [...]
  IF gotparam custom_folder
    printf "@@@ UNLOADING SCN_NET FROM CUSTOM PATH" 
    UnloadPreFile ( <custom_folder> + <level> + "scn_net.pre" ) dont_assert 
  ELSE
    UnloadPreFile ( <level> + "scn_net.pre" ) dont_assert 
  ENDIF
  [...]		
  IF gotparam custom_folder
    printf "@@@ LOADING COL_NET FROM CUSTOM PATH" 
    LoadPipPre ( <custom_folder> + <level> + "col_net.pre" ) heap = bottomup 
  ELSE
    LoadPipPre ( <level> + "col_net.pre" ) heap = bottomup 
  ENDIF
  [...]
ENDSCRIPT
``` 
An example file can be found here: [Levels.q](https://github.com/atljp/THUG-SDL/blob/main/src/Mod/Levels.q)

## TROUBLESHOOTING

### Issues with write permissions

The game settings are stored in `thugsdl.ini` which needs write permission when you change and save settings. <br>

If the directory your game is installed to is protected, e.g. Program Files (x86), the ini file needs to be located in  
`C:\Users\<name>\AppData\Local\VirtualStore`. <br>The game will use the VirtualStore location to load and save settings.

### Gamepad bindings and controls

The control settings in the launcher expect the default gamepad binds. When you change to a non-default setup and want to change the controls, you have to account for the bind changes.<br>
Example:
- Default Ps2 layout: Spin left on L1, Spin right on R1. Nollie on L2, Switch/Revert on R2
- - In your custom binds you swap L1 <-> L2 and R1 <-> R2
- - When you want to set the Caveman key to be on the L1 key, you would have to select L2 in the control options

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
Create a thug2-sdl/build directory. Then build the VS Solution with CMake from that directory via Powershell:
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
- Hardronix: DirectX9 and general code support
- Thanks to everyone who tested this mod during development!

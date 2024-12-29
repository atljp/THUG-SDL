# THUG SDL

A mod to improve the gameplay experience of the PC version of Tony Hawk's Underground. It adds stability and usability fixes, utilizes the SDL2 game library and offers an open base for script-mod development.

## How to install

Download the latest version from the relase tab and extract the files from the zip-archive into the game directory.<br>
Running any THUG.exe (except THUGONE.exe from Clownjob'd) will automatically load the mod from d3d9.dll.
 - Default game directory: `C:\Program Files (x86)\Activision\Tony Hawk's Underground\Game`

### Issues with write permissions

`thugsdl.ini` is the settings-database and needs write permission for storing window positions. <br>
If your game is installed at `C:\Program Files (x86)`, the ini and savegame files have to be located in:
- C:\Users\<name>\AppData\Local\VirtualStore

## Features

### General

- Unofficial port of [partymod-thaw](https://github.com/PARTYMANX/partymod-thaw)
- Replaced input system entirely with new, modern system using the SDL2 library
- Improved window handling allowing for custom resolutions and configurable windowing (4k resolution, (borderless) windowed mode)
- Stability fixes
- Increased default clipping distance for large levels
- Added pause buffer for speedrunning (menu to game transition with pause button held)
- Option to toggle blur effects
- Option to toggle the fullscreen gamma correction which makes the game looks too dark
- Restored the onscreen keyboard (gamepad only)
- Increased speed when exiting the game

### Controls
- Native gamepad support
- Restore original menu navigation for Xbox and Ps2 layouts
- Option to use "grind" in menus to go back (original ps2 layout)
- Fix incorrect button prompts in textboxes and helper texts
- Adjustable keys for Rail Drop Downs
- Adjustable keys for Caveman
- Adjustable keys for ladder/rail grab
- Option for the original Ps2 controller layout
- Option to invert the x or y camera axis
- Option to disable the x or y camera axis

### Online
- Openspy support
- Replace GameSpy with OpenSpy in texts and images
- Menu to load online settings
- Longer text input in quick chat
- No chat reset after net games started/ended
- Removed short freeze time before level changes
- Option for chat size scaling
- Option for chat message time

### Misc
- Supported languages: English, French, German
- Adjustable button prompts: PC, Ps2, Xbox, Ngc
- Console window with adjustable log level
- Intro movie skip
- Higher quality shadows
- Toggle for boardscuffs
- NoAdditionalScriptMods flag
- Option to store the window position
- Create-A-Skater: Unlimited 3 axes scaling
- Create-A-Skater: Increased color ranges
- Create-A-Skater: Board scaling

## TODO
- Modloader
- Crash logs

## BUILDING

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
Set the optimization for the partymod dll to O0 (disable optimization) because MSVC seems to break certain functions when optimization is enabled.
<br><br>
The `.sln` file is in the `build` directory. Make sure to select win32 and Release as the project's build options in Visual Studio. Build the actual DLL with `Ctrl+Shift+B`.

## CREDITS

- PARTYMANX: Original development of Partymod. Thanks for the help and patience!
- %.gone.: Clownjob'd mod for THUG which provided useful insights
- Zedek the Plague Doctor ™: LazyStruct and LazyArray classes, Logger and lots of script-related helper code
- Hardronix: DirectX9 and general code support
- Thanks to everyone who tested this mod during development!
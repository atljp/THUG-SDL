#include <stdio.h>

#include <Windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <uxtheme.h>

#include <SDL2/SDL.h>
#include "pgui.h"
#include "global.h"

// configuration application for PARTYMOD originally written by PARTYMANX
// github.com/PARTYMANX

struct settings settings;
struct keybinds keybinds;
struct controls controls;
struct controllerbinds padbinds;

char configFile[1024];
int doing_keybind = 0;

void do_setting_checkbox(pgui_control* control, int value, int* target) {
	*target = value;
}

void set_menu_combobox_no_offset(pgui_control* control, int value, int* target) {
	*target = value;
}

void set_menu_combobox(pgui_control* control, int value, int* target) {
	*target = value + 1;
}

void writeIniBool(char *section, char *key, int val, char *file) {
	if (val) {
		WritePrivateProfileString(section, key, "1", file);
	} else {
		WritePrivateProfileString(section, key, "0", file);
	}
}

void writeIniInt(char *section, char *key, int val, char *file) {
	char buf[16];
	itoa(val, buf, 10);
	WritePrivateProfileString(section, key, buf, file);
}

void writeIniString(char* section, char* key, char* val, char* file) {
	WritePrivateProfileString(section, key, val, file);
}


void defaultSettings() {
	settings.resX = 0;
	settings.resY = 0;
	settings.windowed = 1;
	settings.borderless = 0;
	settings.savewinpos = 0;

	settings.hq_shadows = 2;
	settings.antialiasing = 1;
	settings.distance_clipping = 0;
	settings.clipping_distance = 100;
	settings.fog = 0;

	settings.disable_blur = 1;
	settings.disable_fsgamma = 0;

	settings.button_font = 2;
	settings.language = 1;
	settings.console = 0;
	settings.writefile = 0;
	settings.appendlog = 0;
	settings.exceptionhandler = 0;
	settings.intromovies = 0;
	settings.boardscuffs = 1;
	settings.noadditionalscriptmods = 0;
	settings.chatsize = 3;
	settings.chatwaittime = 30;
	settings.additionalmods = 0;
	strcpy(settings.additionalmods_folder, "data/pre/mymod");

	controls.ps2_controls = 1;
	controls.dropdownkey = 1;
	controls.laddergrabkey = 1;
	controls.cavemankey = 1;
	controls.menubuttons = 1;
	controls.invertrxp1 = 0;
	controls.invertryp1 = 0;
	controls.disablerxp1 = 0;
	controls.disableryp1 = 0;

	keybinds.ollie = SDL_SCANCODE_KP_2;
	keybinds.grab = SDL_SCANCODE_KP_6;
	keybinds.flip = SDL_SCANCODE_KP_4;
	keybinds.grind = SDL_SCANCODE_KP_8;
	keybinds.spinLeft = SDL_SCANCODE_KP_7;
	keybinds.spinRight = SDL_SCANCODE_KP_9;
	keybinds.nollie = SDL_SCANCODE_KP_1;
	keybinds.switchRevert = SDL_SCANCODE_KP_3;
	keybinds.caveman = SDL_SCANCODE_KP_1;
	keybinds.caveman2 = SDL_SCANCODE_KP_3;

	keybinds.item_up = SDL_SCANCODE_HOME;
	keybinds.item_down = SDL_SCANCODE_END;
	keybinds.item_left = SDL_SCANCODE_DELETE;
	keybinds.item_right = SDL_SCANCODE_PAGEDOWN;

	keybinds.forward = SDL_SCANCODE_W;
	keybinds.backward = SDL_SCANCODE_S;
	keybinds.left = SDL_SCANCODE_A;
	keybinds.right = SDL_SCANCODE_D;
	keybinds.feeble = SDL_SCANCODE_LSHIFT;

	keybinds.camUp = SDL_SCANCODE_Z;
	keybinds.camDown = SDL_SCANCODE_X;
	keybinds.camLeft = SDL_SCANCODE_C;
	keybinds.camRight = SDL_SCANCODE_V;
	keybinds.viewToggle = SDL_SCANCODE_TAB;
	keybinds.swivelLock = SDL_SCANCODE_GRAVE;	// unbound

	padbinds.menu = CONTROLLER_BUTTON_START;
	padbinds.cameraToggle = CONTROLLER_BUTTON_BACK;
	padbinds.cameraSwivelLock = CONTROLLER_BUTTON_RIGHTSTICK;

	padbinds.grind = CONTROLLER_BUTTON_Y;
	padbinds.grab = CONTROLLER_BUTTON_B;
	padbinds.ollie = CONTROLLER_BUTTON_A;
	padbinds.kick = CONTROLLER_BUTTON_X;

	padbinds.leftSpin = CONTROLLER_BUTTON_LEFTSHOULDER;
	padbinds.rightSpin = CONTROLLER_BUTTON_RIGHTSHOULDER;
	padbinds.nollie = CONTROLLER_BUTTON_LEFTTRIGGER;
	padbinds.switchRevert = CONTROLLER_BUTTON_RIGHTTRIGGER;
	padbinds.caveman = CONTROLLER_BUTTON_RIGHTTRIGGER;
	padbinds.caveman2 = CONTROLLER_BUTTON_LEFTTRIGGER;

	padbinds.right = CONTROLLER_BUTTON_DPAD_RIGHT;
	padbinds.left = CONTROLLER_BUTTON_DPAD_LEFT;
	padbinds.up = CONTROLLER_BUTTON_DPAD_UP;
	padbinds.down = CONTROLLER_BUTTON_DPAD_DOWN;

	padbinds.movement = CONTROLLER_STICK_LEFT;
	padbinds.camera = CONTROLLER_STICK_RIGHT;
}

void defaultSettings_tabonly(int tab) {

	if (!tab) {
		settings.resX = 0;
		settings.resY = 0;
		settings.windowed = 1;
		settings.borderless = 0;
		settings.savewinpos = 0;

		settings.hq_shadows = 2;
		settings.antialiasing = 1;
		settings.distance_clipping = 0;
		settings.clipping_distance = 100;
		settings.fog = 0;

		settings.disable_blur = 1;
		settings.disable_fsgamma = 0;

		settings.button_font = 2;
		settings.language = 1;
		settings.console = 0;
		settings.writefile = 0;
		settings.appendlog = 0;
		settings.exceptionhandler = 0;
		settings.intromovies = 0;
		settings.boardscuffs = 1;
		settings.noadditionalscriptmods = 0;
		settings.chatsize = 3;
		settings.chatwaittime = 30;
		settings.additionalmods = 0;
		strcpy(settings.additionalmods_folder, "data/pre/mymod");
	}
	else if (tab == 1) {
		keybinds.ollie = SDL_SCANCODE_KP_2;
		keybinds.grab = SDL_SCANCODE_KP_6;
		keybinds.flip = SDL_SCANCODE_KP_4;
		keybinds.grind = SDL_SCANCODE_KP_8;
		keybinds.spinLeft = SDL_SCANCODE_KP_7;
		keybinds.spinRight = SDL_SCANCODE_KP_9;
		keybinds.nollie = SDL_SCANCODE_KP_1;
		keybinds.switchRevert = SDL_SCANCODE_KP_3;
		keybinds.caveman = SDL_SCANCODE_KP_1;
		keybinds.caveman2 = SDL_SCANCODE_KP_3;

		keybinds.item_up = SDL_SCANCODE_HOME;
		keybinds.item_down = SDL_SCANCODE_END;
		keybinds.item_left = SDL_SCANCODE_DELETE;
		keybinds.item_right = SDL_SCANCODE_PAGEDOWN;

		keybinds.forward = SDL_SCANCODE_W;
		keybinds.backward = SDL_SCANCODE_S;
		keybinds.left = SDL_SCANCODE_A;
		keybinds.right = SDL_SCANCODE_D;
		keybinds.feeble = SDL_SCANCODE_LSHIFT;

		keybinds.camUp = SDL_SCANCODE_Z;
		keybinds.camDown = SDL_SCANCODE_X;
		keybinds.camLeft = SDL_SCANCODE_C;
		keybinds.camRight = SDL_SCANCODE_V;
		keybinds.viewToggle = SDL_SCANCODE_TAB;
		keybinds.swivelLock = SDL_SCANCODE_GRAVE;	// unbound
	}
	else if (tab == 2) {
		padbinds.menu = CONTROLLER_BUTTON_START;
		padbinds.cameraToggle = CONTROLLER_BUTTON_BACK;
		padbinds.cameraSwivelLock = CONTROLLER_BUTTON_RIGHTSTICK;

		padbinds.grind = CONTROLLER_BUTTON_Y;
		padbinds.grab = CONTROLLER_BUTTON_B;
		padbinds.ollie = CONTROLLER_BUTTON_A;
		padbinds.kick = CONTROLLER_BUTTON_X;

		padbinds.leftSpin = CONTROLLER_BUTTON_LEFTSHOULDER;
		padbinds.rightSpin = CONTROLLER_BUTTON_RIGHTSHOULDER;
		padbinds.nollie = CONTROLLER_BUTTON_LEFTTRIGGER;
		padbinds.switchRevert = CONTROLLER_BUTTON_RIGHTTRIGGER;
		padbinds.caveman = CONTROLLER_BUTTON_RIGHTTRIGGER;
		padbinds.caveman2 = CONTROLLER_BUTTON_LEFTTRIGGER;

		padbinds.right = CONTROLLER_BUTTON_DPAD_RIGHT;
		padbinds.left = CONTROLLER_BUTTON_DPAD_LEFT;
		padbinds.up = CONTROLLER_BUTTON_DPAD_UP;
		padbinds.down = CONTROLLER_BUTTON_DPAD_DOWN;

		padbinds.movement = CONTROLLER_STICK_LEFT;
		padbinds.camera = CONTROLLER_STICK_RIGHT;
	}
	else if (tab == 3) {
		controls.ps2_controls = 1;
		controls.dropdownkey = 1;
		controls.laddergrabkey = 1;
		controls.cavemankey = 1;
		controls.menubuttons = 1;
		controls.invertrxp1 = 0;
		controls.invertryp1 = 0;
		controls.disablerxp1 = 0;
		controls.disableryp1 = 0;
	}
}

char *getConfigFile() {
	char executableDirectory[1024];
	int filePathBufLen = 1024;
	GetModuleFileName(NULL, &executableDirectory, filePathBufLen);

	// find last slash
	char *exe = strrchr(executableDirectory, '\\');
	if (exe) {
		*(exe + 1) = '\0';
	}

	sprintf(configFile, "%s%s", executableDirectory, CONFIG_FILE_NAME);

	return configFile;
}

void loadSettings() {
	char *configFile = getConfigFile();

	// GRAPHICS
	settings.resX = GetPrivateProfileInt("Graphics", "ResolutionX", 0, configFile);
	settings.resY = GetPrivateProfileInt("Graphics", "ResolutionY", 0, configFile);
	settings.windowed = getIniBool("Graphics", "Windowed", 0, configFile);
	settings.borderless = getIniBool("Graphics", "Borderless", 0, configFile);
	settings.savewinpos = getIniBool("Graphics", "SaveWindowPosition", 0, configFile);
	settings.hq_shadows = GetPrivateProfileInt("Graphics", "HQShadows", 0, configFile);
	settings.antialiasing = getIniBool("Graphics", "AntiAliasing", 0, configFile);
	settings.distance_clipping = getIniBool("Graphics", "DistanceClipping", 0, configFile);
	settings.clipping_distance = GetPrivateProfileInt("Graphics", "ClippingDistance", 100, configFile);
	settings.fog = getIniBool("Graphics", "Fog", 0, configFile);
	settings.disable_blur = getIniBool("Graphics", "DisableBlur", 1, configFile);
	settings.disable_fsgamma = getIniBool("Graphics", "DisableFullscreenGamma", 1, configFile);

	// LOGGER
	settings.console = GetPrivateProfileInt("Logger", "Console", 0, configFile);
	settings.writefile = getIniBool("Logger", "WriteFile", 0, configFile);
	settings.appendlog = getIniBool("Logger", "AppendLog", 0, configFile);
	settings.exceptionhandler = getIniBool("Logger", "ExceptionHandler", 0, configFile);

	// MISC
	settings.intromovies = getIniBool("Miscellaneous", "IntroMovies", 0, configFile);
	settings.language = GetPrivateProfileInt("Miscellaneous", "Language", 1, configFile);
	settings.button_font = GetPrivateProfileInt("Miscellaneous", "ButtonFont", 2, configFile);
	settings.boardscuffs = getIniBool("Miscellaneous", "Boardscuffs", 1, configFile);
	settings.noadditionalscriptmods = getIniBool("Miscellaneous", "NoAdditionalScriptMods", 0, configFile);
	
	// CONTROLS
	controls.ps2_controls = getIniBool("Controls", "Ps2Controls", 1, configFile);
	controls.dropdownkey = GetPrivateProfileInt("Controls", "DropDownControl", 1, configFile);
	controls.laddergrabkey = GetPrivateProfileInt("Controls", "LadderGrabKey", 1, configFile);
	controls.cavemankey = GetPrivateProfileInt("Controls", "CavemanKey", 1, configFile);
	controls.menubuttons = GetPrivateProfileInt("Controls", "MenuButtons", 1, configFile);
	controls.invertrxp1 = getIniBool("Controls", "InvertRXPlayer1", 0, configFile);
	controls.invertryp1 = getIniBool("Controls", "InvertRYPlayer1", 0, configFile);
	controls.disablerxp1 = getIniBool("Controls", "DisableRXPlayer1", 0, configFile);
	controls.disableryp1 = getIniBool("Controls", "DisableRYPlayer1", 0, configFile);

	// CHAT
	settings.chatsize = GetPrivateProfileInt("Chat", "ChatSize", 3, configFile);
	settings.chatwaittime = GetPrivateProfileInt("Chat", "ChatWaitTime", 30, configFile);

	// ADDITIONAL MODS
	settings.additionalmods = getIniBool("AdditionalMods", "UseMod", 0, configFile);
	GetPrivateProfileString("AdditionalMods", "Folder", "data/pre/mymod", &settings.additionalmods_folder, MAX_PATH, configFile);


	// KEYBINDS
	keybinds.ollie = GetPrivateProfileInt("Keybinds", "Ollie", SDL_SCANCODE_KP_2, configFile);
	keybinds.grab = GetPrivateProfileInt("Keybinds", "Grab", SDL_SCANCODE_KP_6, configFile);
	keybinds.flip = GetPrivateProfileInt("Keybinds", "Flip", SDL_SCANCODE_KP_4, configFile);
	keybinds.grind = GetPrivateProfileInt("Keybinds", "Grind", SDL_SCANCODE_KP_8, configFile);

	keybinds.spinLeft = GetPrivateProfileInt("Keybinds", "SpinLeft", SDL_SCANCODE_KP_7, configFile);
	keybinds.spinRight = GetPrivateProfileInt("Keybinds", "SpinRight", SDL_SCANCODE_KP_9, configFile);
	keybinds.nollie = GetPrivateProfileInt("Keybinds", "Nollie", SDL_SCANCODE_KP_1, configFile); // Not intended for kb
	keybinds.switchRevert = GetPrivateProfileInt("Keybinds", "Switch", SDL_SCANCODE_KP_3, configFile); // Not intended for kb
	keybinds.caveman = GetPrivateProfileInt("Keybinds", "Caveman", SDL_SCANCODE_KP_1, configFile);
	keybinds.caveman2 = GetPrivateProfileInt("Keybinds", "Caveman2", SDL_SCANCODE_KP_3, configFile);

	keybinds.item_up = GetPrivateProfileInt("Keybinds", "ItemUp", SDL_SCANCODE_HOME, configFile);
	keybinds.item_down = GetPrivateProfileInt("Keybinds", "ItemDown", SDL_SCANCODE_END, configFile);
	keybinds.item_left = GetPrivateProfileInt("Keybinds", "ItemLeft", SDL_SCANCODE_DELETE, configFile);
	keybinds.item_right = GetPrivateProfileInt("Keybinds", "ItemRight", SDL_SCANCODE_PAGEDOWN, configFile);

	keybinds.forward = GetPrivateProfileInt("Keybinds", "Forward", SDL_SCANCODE_W, configFile);
	keybinds.backward = GetPrivateProfileInt("Keybinds", "Backward", SDL_SCANCODE_S, configFile);
	keybinds.left = GetPrivateProfileInt("Keybinds", "Left", SDL_SCANCODE_A, configFile);
	keybinds.right = GetPrivateProfileInt("Keybinds", "Right", SDL_SCANCODE_D, configFile);
	keybinds.feeble = GetPrivateProfileInt("Keybinds", "Feeble", SDL_SCANCODE_LSHIFT, configFile);

	keybinds.camUp = GetPrivateProfileInt("Keybinds", "CameraUp", SDL_SCANCODE_Z, configFile);
	keybinds.camDown = GetPrivateProfileInt("Keybinds", "CameraDown", SDL_SCANCODE_X, configFile);
	keybinds.camLeft = GetPrivateProfileInt("Keybinds", "CameraLeft", SDL_SCANCODE_C, configFile);
	keybinds.camRight = GetPrivateProfileInt("Keybinds", "CameraRight", SDL_SCANCODE_V, configFile);
	keybinds.viewToggle = GetPrivateProfileInt("Keybinds", "ViewToggle", SDL_SCANCODE_TAB, configFile);
	keybinds.swivelLock = GetPrivateProfileInt("Keybinds", "SwivelLock", SDL_SCANCODE_GRAVE, configFile);

	// PADBINDS
	
	padbinds.menu = GetPrivateProfileInt("Gamepad", "Pause", CONTROLLER_BUTTON_START, configFile);
	padbinds.cameraToggle = GetPrivateProfileInt("Gamepad", "ViewToggle", CONTROLLER_BUTTON_BACK, configFile);
	padbinds.cameraSwivelLock = GetPrivateProfileInt("Gamepad", "SwivelLock", CONTROLLER_BUTTON_RIGHTSTICK, configFile);

	padbinds.grind = GetPrivateProfileInt("Gamepad", "Grind", CONTROLLER_BUTTON_Y, configFile);
	padbinds.grab = GetPrivateProfileInt("Gamepad", "Grab", CONTROLLER_BUTTON_B, configFile);
	padbinds.ollie = GetPrivateProfileInt("Gamepad", "Ollie", CONTROLLER_BUTTON_A, configFile);
	padbinds.kick = GetPrivateProfileInt("Gamepad", "Flip", CONTROLLER_BUTTON_X, configFile);

	padbinds.leftSpin = GetPrivateProfileInt("Gamepad", "SpinLeft", CONTROLLER_BUTTON_LEFTSHOULDER, configFile);
	padbinds.rightSpin = GetPrivateProfileInt("Gamepad", "SpinRight", CONTROLLER_BUTTON_RIGHTSHOULDER, configFile);
	padbinds.nollie = GetPrivateProfileInt("Gamepad", "Nollie", CONTROLLER_BUTTON_LEFTTRIGGER, configFile);
	padbinds.switchRevert = GetPrivateProfileInt("Gamepad", "Switch", CONTROLLER_BUTTON_RIGHTTRIGGER, configFile);
	padbinds.caveman = GetPrivateProfileInt("Gamepad", "Caveman", CONTROLLER_BUTTON_RIGHTTRIGGER, configFile);
	padbinds.caveman2 = GetPrivateProfileInt("Gamepad", "Caveman2", CONTROLLER_BUTTON_LEFTTRIGGER, configFile);

	padbinds.right = GetPrivateProfileInt("Gamepad", "Right", CONTROLLER_BUTTON_DPAD_RIGHT, configFile);
	padbinds.left = GetPrivateProfileInt("Gamepad", "Left", CONTROLLER_BUTTON_DPAD_LEFT, configFile);
	padbinds.up = GetPrivateProfileInt("Gamepad", "Forward", CONTROLLER_BUTTON_DPAD_UP, configFile);
	padbinds.down = GetPrivateProfileInt("Gamepad", "Backward", CONTROLLER_BUTTON_DPAD_DOWN, configFile);

	padbinds.movement = GetPrivateProfileInt("Gamepad", "MovementStick", CONTROLLER_STICK_LEFT, configFile);
	padbinds.camera = GetPrivateProfileInt("Gamepad", "CameraStick", CONTROLLER_STICK_RIGHT, configFile);
}

void saveSettings() {

	struct settings mSettings;
	struct keybinds mKeybinds;
	struct controls mControls;
	struct controllerbinds mPadbinds;

	set_general_settings(&mSettings);
	set_keybind_settings(&mKeybinds);
	set_control_settings(&mControls);
	set_padbind_settings(&mPadbinds);

	char *configFile = getConfigFile();

	if (settings.resX < 640 && settings.resX != 0 && settings.resY != 0) {
		settings.resX = 640;
	}
	if (settings.resY < 480 && settings.resX != 0 && settings.resY != 0) {
		settings.resY = 480;
	}

	writeIniInt("Graphics", "ResolutionX", mSettings.resX, configFile);
	writeIniInt("Graphics", "ResolutionY", mSettings.resY, configFile);
	writeIniBool("Graphics", "Windowed", mSettings.windowed, configFile);
	writeIniBool("Graphics", "Borderless", mSettings.borderless, configFile);
	writeIniBool("Graphics", "SaveWindowPosition", mSettings.savewinpos, configFile);
	writeIniBool("Graphics", "DisableFullscreenGamma", settings.disable_fsgamma, configFile);
	writeIniBool("Graphics", "DisableBlur", settings.disable_blur, configFile);

	writeIniInt("Graphics", "HQShadows", mSettings.hq_shadows, configFile);
	writeIniBool("Graphics", "AntiAliasing", mSettings.antialiasing, configFile);
	writeIniBool("Graphics", "DistanceClipping", mSettings.distance_clipping, configFile);
	writeIniInt("Graphics", "ClippingDistance", mSettings.clipping_distance, configFile);
	writeIniBool("Graphics", "Fog", mSettings.fog, configFile);

	writeIniBool("Controls", "PS2Controls", mControls.ps2_controls, configFile);
	writeIniInt("Controls", "DropDownControl", mControls.dropdownkey, configFile);
	writeIniInt("Controls", "CavemanKey", mControls.cavemankey, configFile);
	writeIniInt("Controls", "LadderGrabKey", mControls.laddergrabkey, configFile);
	writeIniInt("Controls", "MenuButtons", mControls.menubuttons, configFile);
	writeIniBool("Controls", "InvertRXPlayer1", mControls.invertrxp1, configFile);
	writeIniBool("Controls", "InvertRYPlayer1", mControls.invertryp1, configFile);
	writeIniBool("Controls", "DisableRXPlayer1", mControls.disablerxp1, configFile);
	writeIniBool("Controls", "DisableRYPlayer1", mControls.disableryp1, configFile);

	writeIniInt("Miscellaneous", "ButtonFont", mSettings.button_font, configFile);
	writeIniInt("Logger", "Console", mSettings.console, configFile);
	writeIniInt("Logger", "WriteFile", mSettings.writefile, configFile);
	writeIniInt("Logger", "AppendLog", mSettings.appendlog, configFile);
	writeIniInt("Logger", "ExceptionHandler", mSettings.exceptionhandler, configFile);
	writeIniInt("Miscellaneous", "IntroMovies", mSettings.intromovies, configFile);
	writeIniInt("Miscellaneous", "Language", mSettings.language, configFile);
	writeIniInt("Miscellaneous", "Boardscuffs", mSettings.boardscuffs, configFile);
	writeIniInt("Miscellaneous", "NoAdditionalScriptMods", mSettings.noadditionalscriptmods, configFile);
	writeIniInt("Chat", "ChatSize", mSettings.chatsize, configFile);
	writeIniInt("Chat", "ChatWaitTime", mSettings.chatwaittime, configFile);
	writeIniInt("AdditionalMods", "UseMod", mSettings.additionalmods, configFile);
	writeIniString("AdditionalMods", "Folder", mSettings.additionalmods_folder, configFile);

	writeIniInt("Keybinds", "Ollie", mKeybinds.ollie, configFile);
	writeIniInt("Keybinds", "Grab", mKeybinds.grab, configFile);
	writeIniInt("Keybinds", "Flip", mKeybinds.flip, configFile);
	writeIniInt("Keybinds", "Grind", mKeybinds.grind, configFile);
	writeIniInt("Keybinds", "SpinLeft", mKeybinds.spinLeft, configFile);
	writeIniInt("Keybinds", "SpinRight", mKeybinds.spinRight, configFile);
	writeIniInt("Keybinds", "Nollie", mKeybinds.nollie, configFile);
	writeIniInt("Keybinds", "Switch", mKeybinds.switchRevert, configFile);
	writeIniInt("Keybinds", "Caveman", mKeybinds.caveman, configFile);
	writeIniInt("Keybinds", "Caveman2", mKeybinds.caveman2, configFile);

	writeIniInt("Keybinds", "ItemUp", mKeybinds.item_up, configFile);
	writeIniInt("Keybinds", "ItemDown", mKeybinds.item_down, configFile);
	writeIniInt("Keybinds", "ItemLeft", mKeybinds.item_left, configFile);
	writeIniInt("Keybinds", "ItemRight", mKeybinds.item_right, configFile);

	writeIniInt("Keybinds", "Forward", mKeybinds.forward, configFile);
	writeIniInt("Keybinds", "Backward", mKeybinds.backward, configFile);
	writeIniInt("Keybinds", "Left", mKeybinds.left, configFile);
	writeIniInt("Keybinds", "Right", mKeybinds.right, configFile);
	writeIniInt("Keybinds", "Feeble", mKeybinds.feeble, configFile);

	writeIniInt("Keybinds", "CameraUp", mKeybinds.camUp, configFile);
	writeIniInt("Keybinds", "CameraDown", mKeybinds.camDown, configFile);
	writeIniInt("Keybinds", "CameraLeft", mKeybinds.camLeft, configFile);
	writeIniInt("Keybinds", "CameraRight", mKeybinds.camRight, configFile);
	writeIniInt("Keybinds", "ViewToggle", mKeybinds.viewToggle, configFile);
	writeIniInt("Keybinds", "SwivelLock", mKeybinds.swivelLock, configFile);

	writeIniInt("Gamepad", "Pause", mPadbinds.menu, configFile);
	writeIniInt("Gamepad", "ViewToggle", mPadbinds.cameraToggle, configFile);
	writeIniInt("Gamepad", "SwivelLock", mPadbinds.cameraSwivelLock, configFile);

	writeIniInt("Gamepad", "Grind", mPadbinds.grind, configFile);
	writeIniInt("Gamepad", "Grab", mPadbinds.grab, configFile);
	writeIniInt("Gamepad", "Ollie", mPadbinds.ollie, configFile);
	writeIniInt("Gamepad", "Flip", mPadbinds.kick, configFile);

	writeIniInt("Gamepad", "SpinLeft", mPadbinds.leftSpin, configFile);
	writeIniInt("Gamepad", "SpinRight", mPadbinds.rightSpin, configFile);
	writeIniInt("Gamepad", "Nollie", mPadbinds.nollie, configFile);
	writeIniInt("Gamepad", "Switch", mPadbinds.switchRevert, configFile);
	writeIniInt("Gamepad", "Caveman", mPadbinds.caveman, configFile);
	writeIniInt("Gamepad", "Caveman2", mPadbinds.caveman2, configFile);

	writeIniInt("Gamepad", "Right", mPadbinds.right, configFile);
	writeIniInt("Gamepad", "Left", mPadbinds.left, configFile);
	writeIniInt("Gamepad", "Forward", mPadbinds.up, configFile);
	writeIniInt("Gamepad", "Backward", mPadbinds.down, configFile);

	writeIniInt("Gamepad", "MovementStick", mPadbinds.movement, configFile);
	writeIniInt("Gamepad", "CameraStick", mPadbinds.camera, configFile);
}

// SDL stuff - for keybinds

void cursedSDLSetup() {
	// in order to key names, SDL has to have created a window
	// this function achieves that to initialize key binds

	SDL_Init(SDL_INIT_EVENTS);

	SDL_Window *win = SDL_CreateWindow("uhh... you're not supposed to see this", 0, 0, 0, 0, SDL_WINDOW_HIDDEN);

	SDL_DestroyWindow(win);

	SDL_Quit();
}

void setBindText(pgui_control *control, SDL_Scancode key) {
	// NOTE: requires SDL to be initialized
	if (key == 0) {
		pgui_textbox_set_text(control, "Unbound");
	} else {
		pgui_textbox_set_text(control, SDL_GetKeyName(SDL_GetKeyFromScancode(key)));
	}
}

void doKeyBind(char *name, SDL_Scancode *target, pgui_control *control) {
	if (doing_keybind) {
		// trying to do keybind while we're already trying to process one hangs the program, so let's put this off until later
		// NOTE: this causes a mildly annoying bug where selecting another control to bind fails
		return;
	}

	doing_keybind = 1;

	pgui_textbox_set_text(control, "Press key...");

	SDL_Init(SDL_INIT_EVENTS);

	char namebuf[64];
	sprintf(namebuf, "Press Key To Bind To %s", name);

	RECT windowRect;

	if (!GetWindowRect(control->hwnd, &windowRect)) {
		printf("Failed to get window rect!!\n");
		return;
	}

	// create 1x1 window in top left where it's least likely to be noticed
	SDL_Window *inputWindow = SDL_CreateWindow(namebuf, 0, 0, 1, 1, SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_BORDERLESS);

	if (!inputWindow) {
		printf("%s\n", SDL_GetError());
		SDL_Quit();
		return;
	}

	int doneInput = 0;
	SDL_Event e;
	while (!doneInput) {
		//printf("still running event loop\n");
		while(SDL_PollEvent(&e)) {
			if (e.type == SDL_KEYDOWN) {
				SDL_KeyCode keyCode = SDL_GetKeyFromScancode(e.key.keysym.scancode);
				printf("Pressed key %s\n", SDL_GetKeyName(keyCode));
				*target = e.key.keysym.scancode;
					
				doneInput = 1;
			} else if (e.type == SDL_QUIT) {
				doneInput = 1;
			} else if (e.type == SDL_WINDOWEVENT) {
				if (e.window.event == SDL_WINDOWEVENT_HIDDEN || e.window.event == SDL_WINDOWEVENT_FOCUS_LOST || e.window.event == SDL_WINDOWEVENT_MINIMIZED) {
					doneInput = 1;
				}
			}
		}
	}
	SDL_DestroyWindow(inputWindow);

	setBindText(control, *target);

	SDL_Quit();

	doing_keybind = 0;
}



void callback_ok(pgui_control *control, void *data) {
	saveSettings();
	PostQuitMessage(0);
}


void callback_save(pgui_control* control, void* data) {
	saveSettings();
	MessageBox(0, "Settings have been saved successfully!", "THUG SDL", MB_OK); //TODO ADD BACK
}


void callback_cancel(pgui_control *control, void *data) {
	PostQuitMessage(0);
}

void callback_default(pgui_control *control, void *data) {
	defaultSettings();
	update_general_page();
	update_control_page();
	setAllBindText();
	setAllPadBindText();
}

HWND test;
void callback_default_tabonly(pgui_control* control, void* data) {
	int currentTabIndex = TabCtrl_GetCurSel(test); 
	if (currentTabIndex != -1) {

		defaultSettings_tabonly(currentTabIndex);
		switch (currentTabIndex) {
		case 0:
			update_general_page();
			break;
		case 1:
			setAllBindText();
			break;
		case 2:
			setAllPadBindText();
			break;
		case 3:
			update_control_page();
			break;
		default:
			break;
		}		
	}
}

int main(int argc, char **argv) {
	cursedSDLSetup();
	loadSettings();

	pgui_control *window = pgui_window_create(430, 480, "Tony Hawk's Underground SDL Launcher");

	pgui_control *restore_default_button = pgui_button_create(8, window->h - 42 + 8, 70, 26, "Defaults (all)", window);
	pgui_control* restore_default_tabonly_button = pgui_button_create(8 + 8 + 71, window->h - 42 + 8, 70, 26, "Defaults (tab)", window);
	pgui_control* cancel_button = pgui_button_create(window->w - (88 * 3), window->h - 42 + 8, 80, 26, "Cancel", window);
	pgui_control* save_button = pgui_button_create(window->w - (88 * 2), window->h - 42 + 8, 80, 26, "Save settings", window);
	pgui_control *ok_button = pgui_button_create(window->w - 88, window->h - 42 + 8, 80, 26, "OK", window);

	pgui_button_set_on_press(restore_default_button, callback_default, NULL);
	pgui_button_set_on_press(restore_default_tabonly_button, callback_default_tabonly, window);
	pgui_button_set_on_press(save_button, callback_save, NULL);
	pgui_button_set_on_press(cancel_button, callback_cancel, NULL);
	pgui_button_set_on_press(ok_button, callback_ok, NULL);

	char *tab_names[4] = {
		"General",
		"Keyboard",
		"Gamepad",
		"Controls",
	};

	pgui_control *tabs = pgui_tabs_create(8, 8, window->w - 16, window->h - 8 - 32 - 8, tab_names, 4, window);
	test = tabs->hwnd;

	build_general_page(tabs->children[0]);
	build_keyboard_page(tabs->children[1]);
	build_gamepad_page(tabs->children[2]);
	build_control_page(tabs->children[3]);

	pgui_control_set_hidden(tabs->children[1], 1);	// bug workaround: controls don't check that the hierarchy is hidden when created, so let's just re-hide it
	pgui_control_set_hidden(tabs->children[2], 1);	// bug workaround: controls don't check that the hierarchy is hidden when created, so let's just re-hide it
	pgui_control_set_hidden(tabs->children[3], 1);	// bug workaround: controls don't check that the hierarchy is hidden when created, so let's just re-hide it

	update_general_page();
	update_control_page();
	setAllBindText();
	setAllPadBindText();
	
	pgui_window_show(window);
	
	// Run the message loop.

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}
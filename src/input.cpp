#pragma once

#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <SDL.h>
#include <patch.h>
#include <config.h>
#include "keycodes.h"
#include <QB/LazyStruct.h>
#include <QB/Qb.h>
#include <Util/sb.h>

/******************************************************************/
/*                                                                */
/*  Definitions													  */
/******************************************************************/

#define MAX_PLAYERS 2

enum EEditorState
{
	vINACTIVE = 0xd443a2bc,		// "off"
	vEDITING = 0x7eca21e5,		// "edit"
	vTEST_PLAY = 0xa40f0b8a,	// "test_play"
	vREGULAR_PLAY = 0xcd3682c1,	// "regular_play"

	// use this as parameter only
	vKEEP_SAME_STATE = 0,
};

struct EdCParkEditorInstance
{
	char unk[100];
	float m_camAngle;
	float m_camAngleVert;
	float m_camDist;
	char unk2[8];
	uint32_t m_cursor_state;
	uint32_t v_start_cb;
	uint32_t v_stop_cb;
	uint32_t mp_park_manager;
	uint32_t m_cursor_pos;
	uint32_t mp_cursor;
	uint32_t mp_menu_manager;
	uint32_t m_last_time;
	EEditorState m_state;
	bool m_paused;
	char unk3[7];
	bool ScriptSetParkEditorTimeOfDay;

};

struct CGoalManagerInstance
{
	char unk[76];
	uint32_t GetLevelStructureName;
};

struct SkateInstance
{
	/* singleton of Skate::Instance() */
	char unk[920];
	uint32_t m_cur_level;
	uint32_t m_prev_level;
	bool m_new_record;
	char unk2[7];
	CGoalManagerInstance GetGoalManager;
};

typedef struct {
	uint32_t vtablePtr;
	uint32_t node;
	uint32_t type;
	uint32_t port;
	uint32_t slot;
	uint32_t isValid;
	uint32_t unk_24;
	uint8_t controlData[32];	// PS2 format control data
	uint8_t vibrationData_align[32];
	uint8_t vibrationData_direct[32];
	uint8_t vibrationData_max[32];
	uint8_t vibrationData_oldDirect[32];
	uint32_t unk5;
	uint32_t actuatorsDisabled;
	uint32_t capabilities;
	uint32_t unk6;
	uint32_t num_actuators;
	uint32_t unk7;
	uint32_t state;
	uint32_t test;
	uint32_t index;
	uint32_t isPluggedIn;
	uint32_t unplugged_counter;
	uint32_t unplugged_retry;
	uint32_t pressed;
	uint32_t start_or_a_pressed;

} device;

struct playerslot {
	SDL_GameController* controller;
	uint8_t lockedOut;
	//uint64_t signin;	// time until controller can be used after sign-in
	//SDL_GameControllerButton lockedButton;	// button that player used to sign-in, to be ignored until release
};

// Separate menu controls
uint8_t* isMenu = (uint8_t*)0x0076A7A7;

// For pause buffer
uint8_t* otherIsMenu = (uint8_t*)0x0076A7A4;
uint8_t isInMenu = 0;

// Check for key_on_screen flag: If a text entry prompt is on-screen, keybinds don't interfere with text entry confirmation/cancellation
uint8_t* keyboard_on_screen = (uint8_t*)0x0076A7A6;

// Create-A-Goal should technically be detected as a menu. By default it uses default keybindings
uint8_t* isCAG = (uint8_t*)0x0076A7A4; //0x006A0350;

SDL_GameController** controllerList;
uint8_t isUsingKeyboard = 1;
EdCParkEditorInstance* ParkEd;
CGoalManagerInstance* GoalMan;
SkateInstance* Skate;
int buffer = 0;
bool bSpace_released = true;
bool bReturn_released = true;
bool bBackspace_released = true;
bool bEscape_released = true;
bool instances_initialized = false;
char* executableDirectory3[MAX_PATH];
uint8_t numplayers = 0;
int controllerCount;
int controllerListSize;
struct modsettings inputsettings;
struct keybinds keybinds;
struct controllerbinds padbinds;
struct playerslot players[MAX_PLAYERS] = { { NULL, 0 }, { NULL, 0 } };
int* shouldQuit = (int*)0x007CEC8C;

typedef bool __cdecl ScriptObjectExists_NativeCall(Script::LazyStruct* params);
ScriptObjectExists_NativeCall* ScriptObjectExists_Native = (ScriptObjectExists_NativeCall*)(0x0041FD90);

typedef CGoalManagerInstance* __cdecl GameGetGoalManager_NativeCall();
GameGetGoalManager_NativeCall* GameGetGoalManager_Native = (GameGetGoalManager_NativeCall*)(0x004EC8D0);

typedef void (key_input)(int32_t key, uint32_t param);
key_input* m_keyinput = (key_input*)0x00581BF0;


/******************************************************************/
/*                                                                */
/*  SDL setup													  */
/******************************************************************/

void addController(int idx) {
	SDL_GameController* controller = SDL_GameControllerOpen(idx);

	SDL_GameControllerSetPlayerIndex(controller, -1);

	if (controller) {
		if (controllerCount == controllerListSize) {
			int tmpSize = controllerListSize + 1;
			SDL_GameController** tmp = (SDL_GameController**)realloc(controllerList, sizeof(SDL_GameController*) * tmpSize);
			if (!tmp) {
				return;
			}

			controllerListSize = tmpSize;
			controllerList = tmp;
		}
		controllerList[controllerCount] = controller;
		controllerCount++;
	}
}

void addplayer(SDL_GameController* controller) {
	if (numplayers < 2) {
		// Find open slot
		uint8_t found = 0;
		int i = 0;
		for (; i < MAX_PLAYERS; i++) {
			if (!players[i].controller) {
				found = 1;
				break;
			}
		}
		if (found) {
			SDL_GameControllerSetPlayerIndex(controller, i);
			players[i].controller = controller;

			if (numplayers > 0) {
				players[i].lockedOut = 1;
				SDL_JoystickRumble(SDL_GameControllerGetJoystick(controller), 0xffff, 0xffff, 250);
			}
			numplayers++;
			Log::TypedLog(CHN_SDL, "Added player %d: %s\n", i + 1, SDL_GameControllerName(controller));	
		}
	}
	else {
		Log::TypedLog(CHN_SDL, "Already two players, not adding\n");
	}
}

void pruneplayers() {
	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (players[i].controller && !SDL_GameControllerGetAttached(players[i].controller)) {
			Log::TypedLog(CHN_SDL, "Pruned player %d\n", i + 1);

			players[i].controller = NULL;
			numplayers--;
			Log::TypedLog(CHN_SDL, "Remaining players: %d\n", numplayers);
		}
	}
}

void removeController(SDL_GameController* controller) {
	Log::TypedLog(CHN_SDL, "Controller \"%s\" disconnected\n", SDL_GameControllerName(controller));

	int i = 0;

	while (i < controllerCount && controllerList[i] != controller) {
		i++;
	}

	if (controllerList[i] == controller) {
		SDL_GameControllerClose(controller);

		int playerIdx = SDL_GameControllerGetPlayerIndex(controller);
		if (playerIdx != -1) {
			Log::TypedLog(CHN_SDL, "Removed player %d\n", playerIdx + 1);
			players[playerIdx].controller = NULL;
			numplayers--;
		}

		pruneplayers();

		for (; i < controllerCount - 1; i++) {
			controllerList[i] = controllerList[i + 1];
		}
		controllerCount--;
	}
	else {
		Log::TypedLog(CHN_SDL, "Did not find disconnected controller in list\n");
	}
}

void initSDLControllers() {
	Log::TypedLog(CHN_SDL, "Initializing Controller Input\n");

	controllerCount = 0;
	controllerListSize = 1;
	controllerList = (SDL_GameController**)malloc(sizeof(SDL_GameController*) * controllerListSize);
	int detected = 0;

	for (int i = 0; i < SDL_NumJoysticks(); i++) {
		if (SDL_IsGameController(i)) {
			addController(i);
			//if (!(detected)) Log::TypedLog(CHN_SDL, "Detected controller \"%s\"\n", SDL_GameControllerNameForIndex(i));
			//detected = 1;
		}
	}
}

uint8_t axisAbs(uint8_t val) {
	if (val > 127) {
		// Positive, just remove top bit
		return val & 0x7F;
	}
	else {
		// Negative
		return ~val & 0x7F;
	}
}

uint8_t getButton(SDL_GameController* controller, controllerButton button) {
	if (button == CONTROLLER_BUTTON_LEFTTRIGGER) {
		uint8_t pressure = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT) >> 7;
		return pressure > 0x80;
	}
	else if (button == CONTROLLER_BUTTON_RIGHTTRIGGER) {
		uint8_t pressure = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) >> 7;
		return pressure > 0x80;
	}
	else {
		return SDL_GameControllerGetButton(controller, (SDL_GameControllerButton)button);
	}
}

void getStick(SDL_GameController* controller, controllerStick stick, uint8_t* xOut, uint8_t* yOut) {
	uint8_t result_x, result_y;

	if (stick == CONTROLLER_STICK_LEFT) {
		result_x = (uint8_t)((SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX) >> 8) + 128);
		result_y = (uint8_t)((SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY) >> 8) + 128);
	}
	else if (stick == CONTROLLER_STICK_RIGHT) {
		result_x = inputsettings.disableRXplayer1 ? 128 : ((inputsettings.invertRXplayer1 && !(ParkEd->m_state == EEditorState::vEDITING)) ? 255 - (uint8_t)((SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX) >> 8) + 128) : (uint8_t)((SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX) >> 8) + 128));
		result_y = inputsettings.disableRYplayer1 ? 128 : ((inputsettings.invertRYplayer1 && !(ParkEd->m_state == EEditorState::vEDITING)) ? 255 - (uint8_t)((SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY) >> 8) + 128) : (uint8_t)((SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY) >> 8) + 128));
	}
	else {
		result_x = 0x80;
		result_y = 0x80;
	}

	if (axisAbs(result_x) > axisAbs(*xOut)) {
		*xOut = result_x;
	}

	if (axisAbs(result_y) > axisAbs(*yOut)) {
		*yOut = result_y;
	}
}

void __cdecl set_actuators(int port, uint16_t high, uint16_t low) {
	for (int i = 0; i < controllerCount; i++) {
		if (SDL_GameControllerGetAttached(controllerList[i]) && SDL_GameControllerGetPlayerIndex(controllerList[i]) == port) {
			SDL_JoystickRumble(SDL_GameControllerGetJoystick(controllerList[i]), low, high, 0);
		}
	}
}

uint32_t convert_SDL_to_OIS_keycode(uint8_t sdlKeyCode) {
	// Lookup the SDL keycode in the map
	auto it = SDL_to_OIS_map.find(sdlKeyCode);
	if (it != SDL_to_OIS_map.end()) {
		// Return the corresponding OIS keycode
		return it->second;
	}
	else {
		// If no mapping found, return an unspecified keycode
		return (uint32_t)OIS_KC_UNASSIGNED;
	}
}

void setUsingKeyboard(uint8_t usingKeyboard) {
	isUsingKeyboard = usingKeyboard;
}

bool shouldUseMenuControls() {
	return (*isMenu && !(ParkEd->m_state == EEditorState::vEDITING) || *isCAG || ((ParkEd->m_state == EEditorState::vEDITING) && ParkEd->m_paused));
}


/******************************************************************/
/*                                                                */
/*  Device polling												  */
/******************************************************************/

void pollController(device* dev, SDL_GameController* controller) {

	if (SDL_GameControllerGetAttached(controller)) {
		dev->isValid = 1;
		dev->isPluggedIn = 1;

		// We don't check for keyboard_onscreen since the onscreen keyboard is displayed
		if (ParkEd) {

			if (shouldUseMenuControls()) {

				/********************************************************************/
				/*																	*/
				/*  Menu controls													*/
				/********************************************************************/

				if (getButton(controller, padbinds.menu)) {
					dev->controlData[2] |= 0x01 << 3;
				}
				if (getButton(controller, padbinds.grind)) {
					dev->controlData[3] |= 0x01 << 4;
				}
				if (getButton(controller, padbinds.grab)) {
					dev->controlData[3] |= 0x01 << 5;
				}
				if (getButton(controller, padbinds.ollie)) {
					dev->controlData[3] |= 0x01 << 6;
				}
				if (getButton(controller, padbinds.kick)) {
					dev->controlData[3] |= 0x01 << 7;
				}
				if (*isCAG) {
					if (inputsettings.isPs2Controls) {
						if (getButton(controller, padbinds.leftSpin)) {
							dev->controlData[3] |= 0x01 << 3;
						}
						if (getButton(controller, padbinds.rightSpin)) {
							dev->controlData[20] |= 0x01 << 0;
						}
						if (getButton(controller, padbinds.nollie)) {
							dev->controlData[3] |= 0x01 << 2;
						}
						if (getButton(controller, padbinds.switchRevert)) {
							dev->controlData[20] |= 0x01 << 1;
						}
					}
					else {
						if (getButton(controller, padbinds.leftSpin)) {
							dev->controlData[3] |= 0x01 << 2;
						}
						if (getButton(controller, padbinds.rightSpin)) {
							dev->controlData[3] |= 0x01 << 3;
						}
						if (getButton(controller, padbinds.caveman)) {
							dev->controlData[20] |= 0x01 << 1;
						}
						if (getButton(controller, padbinds.caveman2)) {
							dev->controlData[20] |= 0x01 << 0;
						}
					}
				}
				else {
					if (inputsettings.isPs2Controls) {
						if (getButton(controller, padbinds.leftSpin)) {
							dev->controlData[3] |= 0x01 << 2;
						}
						if (getButton(controller, padbinds.rightSpin)) {
							dev->controlData[3] |= 0x01 << 3;
						}
						if (getButton(controller, padbinds.nollie)) {
							dev->controlData[20] |= 0x01 << 1;
						}
						if (getButton(controller, padbinds.switchRevert)) {
							dev->controlData[20] |= 0x01 << 0;
						}
					}
					else {
						if (getButton(controller, padbinds.leftSpin)) {
							dev->controlData[3] |= 0x01 << 2;
						}
						if (getButton(controller, padbinds.rightSpin)) {
							dev->controlData[3] |= 0x01 << 3;
						}
						if (getButton(controller, padbinds.caveman)) {
							dev->controlData[20] |= 0x01 << 1;
						}
						if (getButton(controller, padbinds.caveman2)) {
							dev->controlData[20] |= 0x01 << 0;
						}
					}
				}
				if (SDL_GameControllerGetButton(controller, (SDL_GameControllerButton)padbinds.up)) {
					dev->controlData[2] |= 0x01 << 4;
				}
				if (SDL_GameControllerGetButton(controller, (SDL_GameControllerButton)padbinds.right)) {
					dev->controlData[2] |= 0x01 << 5;
				}
				if (SDL_GameControllerGetButton(controller, (SDL_GameControllerButton)padbinds.down)) {
					dev->controlData[2] |= 0x01 << 6;
				}
				if (SDL_GameControllerGetButton(controller, (SDL_GameControllerButton)padbinds.left)) {
					dev->controlData[2] |= 0x01 << 7;
				}
				getStick(controller, padbinds.camera, &(dev->controlData[4]), &(dev->controlData[5]));
				getStick(controller, padbinds.movement, &(dev->controlData[6]), &(dev->controlData[7]));
			}
			else {
				if (ParkEd->m_state == EEditorState::vEDITING) {

					/******************************************************************/
					/*                                                                */
					/*  Gameplay (CAP editing) controls								  */
					/******************************************************************/

					if (getButton(controller, padbinds.menu)) {
						dev->controlData[2] |= 0x01 << 3;
					}
					if (getButton(controller, padbinds.grind)) {
						dev->controlData[3] |= 0x01 << 4;
					}
					if (getButton(controller, padbinds.grab)) {
						dev->controlData[3] |= 0x01 << 5;
					}
					if (getButton(controller, padbinds.ollie)) {
						dev->controlData[3] |= 0x01 << 6;
					}
					if (getButton(controller, padbinds.kick)) {
						dev->controlData[3] |= 0x01 << 7;
					}
					if (inputsettings.isPs2Controls) {
						if (getButton(controller, padbinds.leftSpin)) {
							dev->controlData[3] |= 0x01 << 2;
						}
						if (getButton(controller, padbinds.rightSpin)) {
							dev->controlData[20] |= 0x01 << 0;
						}
						if (getButton(controller, padbinds.nollie)) {
							dev->controlData[3] |= 0x01 << 0;
						}
						if (getButton(controller, padbinds.switchRevert)) {
							dev->controlData[20] |= 0x01 << 1;
						}
					}
					else {
						if (getButton(controller, padbinds.leftSpin)) {
							dev->controlData[3] |= 0x01 << 0; // Lower
						}
						if (getButton(controller, padbinds.rightSpin)) {
							dev->controlData[3] |= 0x01 << 2; // Raise
						}
						if (getButton(controller, padbinds.caveman)) {
							dev->controlData[20] |= 0x01 << 1; // White
						}
						if (getButton(controller, padbinds.caveman2)) {
							dev->controlData[20] |= 0x01 << 0; // Black
						}
					}
				}
				else {

					/******************************************************************/
					/*                                                                */
					/*  Gameplay (in level) controls								  */
					/******************************************************************/

					// Buttons
					if (getButton(controller, padbinds.menu)) {
						dev->controlData[2] |= 0x01 << 3;
					}
					if (getButton(controller, padbinds.cameraToggle)) {
						dev->controlData[2] |= 0x01 << 0;
					}
					if (getButton(controller, padbinds.cameraSwivelLock)) {
						dev->controlData[2] |= 0x01 << 2;
					}
					if (getButton(controller, padbinds.grind)) {
						dev->controlData[3] |= 0x01 << 4;
						dev->controlData[12] = 0xff;
					}
					if (getButton(controller, padbinds.grab)) {
						dev->controlData[3] |= 0x01 << 5;
						dev->controlData[13] = 0xff;
					}
					if (getButton(controller, padbinds.ollie)) {
						dev->controlData[3] |= 0x01 << 6;
						dev->controlData[14] = 0xff;
					}
					if (getButton(controller, padbinds.kick)) {
						dev->controlData[3] |= 0x01 << 7;
						dev->controlData[15] = 0xff;
					}
					// Shoulders
					if (inputsettings.isPs2Controls) {
						if (getButton(controller, padbinds.leftSpin)) {
							dev->controlData[3] |= 0x01 << 2;
							dev->controlData[16] = 0xff;
						}
						if (getButton(controller, padbinds.rightSpin)) {
							dev->controlData[3] |= 0x01 << 3;
							dev->controlData[17] = 0xff;
						}
						if (getButton(controller, padbinds.nollie)) {
							dev->controlData[3] |= 0x01 << 0;
							dev->controlData[18] = 0xff;
						}
						if (getButton(controller, padbinds.switchRevert)) {
							dev->controlData[3] |= 0x01 << 1;
							dev->controlData[19] = 0xff;
						}
						//Two button caveman on spinkeys
						if ((getButton(controller, padbinds.leftSpin)) && getButton(controller, padbinds.rightSpin)) {
							dev->controlData[20] |= 0x01 << 0;
						}
					}
					else // Xbox controls
					{
						if (getButton(controller, padbinds.leftSpin)) {
							dev->controlData[3] |= 0x01 << 2;
							dev->controlData[16] = 0xff;
							dev->controlData[3] |= 0x01 << 0;
							dev->controlData[18] = 0xff;
						}
						if (getButton(controller, padbinds.rightSpin)) {
							dev->controlData[3] |= 0x01 << 3;
							dev->controlData[17] = 0xff;
							dev->controlData[3] |= 0x01 << 1;
							dev->controlData[19] = 0xff;
						}
						if (getButton(controller, padbinds.caveman)) {
							dev->controlData[20] |= 0x01 << 1;
						}
						if (getButton(controller, padbinds.caveman2)) {
							dev->controlData[20] |= 0x01 << 0;
						}
					}
				}
				// D-Pad
				if (SDL_GameControllerGetButton(controller, (SDL_GameControllerButton)padbinds.up)) {
					dev->controlData[2] |= 0x01 << 4;
					dev->controlData[10] = 0xFF;
					//dev->controlData[7] = 0;
				}
				if (SDL_GameControllerGetButton(controller, (SDL_GameControllerButton)padbinds.right)) {
					dev->controlData[2] |= 0x01 << 5;
					dev->controlData[8] = 0xFF;
					//dev->controlData[6] = 255;
				}
				if (SDL_GameControllerGetButton(controller, (SDL_GameControllerButton)padbinds.down)) {
					dev->controlData[2] |= 0x01 << 6;
					dev->controlData[11] = 0xFF;
					//dev->controlData[7] = 255;
				}
				if (SDL_GameControllerGetButton(controller, (SDL_GameControllerButton)padbinds.left)) {
					dev->controlData[2] |= 0x01 << 7;
					dev->controlData[9] = 0xFF;
					//dev->controlData[6] = 0;
				}
				// Sticks
				getStick(controller, padbinds.camera, &(dev->controlData[4]), &(dev->controlData[5]));
				getStick(controller, padbinds.movement, &(dev->controlData[6]), &(dev->controlData[7]));
			}
		}
		else if (!ParkEd) { // Only needed to be able to skip intro movies manually. The ParkEd instance isn't initialized yet when movies are displayed
			if (getButton(controller, padbinds.ollie)) {
				dev->controlData[2] |= 0x01 << 3;
			}
		}
	}
}

void pollKeyboard(device* dev) {

	dev->isValid = 1;
	dev->isPluggedIn = 1;

	uint8_t* keyboardState = (uint8_t*)SDL_GetKeyboardState(NULL);

	// Add a bit of delay for certain key presses
	if (buffer > 0)
		buffer--;

	/********************************************************************/
	/*  STATIC MENU KEYS												*/
	/*																	*/
	/*																	*/
	/*  spinleft = rotate left(2)										*/
	/*  spinright = rotate right(1)										*/
	/*  Grind->R														*/
	/*  Flip->E															*/
	/*  caveman1->ß zoom out											*/
	/*  caveman2->´ zoom in												*/
	/*  Arrow keys for menu												*/
	/*  S/X->camera up down												*/
	/*  Y/C->camera left right											*/
	/*  Keybind for Ollie is disabled									*/
	/*																	*/
	/*  ParkEd singleton is initialized after polling begins			*/
	/*  When a keyboard is on screen, use m_keyinput instead			*/
	/********************************************************************/

	if (!*keyboard_on_screen && ParkEd) {

		// Setup for pause buffer
		isInMenu = ((*isMenu || *otherIsMenu) && !(ParkEd->m_state == EEditorState::vEDITING));

		if (shouldUseMenuControls()) {

			/********************************************************************/
			/*																	*/
			/*  Menu controls													*/
			/********************************************************************/

			if (keyboardState[SDL_SCANCODE_SPACE] && bSpace_released) {
				dev->controlData[2] |= 0x01 << 3;
				buffer = 20;
				bSpace_released = false;
			}
			if (keyboardState[SDL_SCANCODE_ESCAPE] && bEscape_released && !buffer) {
				// In menus, ESC simulates the grab button
				// The ParkEditor is detected as a menu so ESC doesn't work there when only checking for the menu flag
				// That's why the ParkEd state is checked as well
				dev->controlData[3] |= 0x01 << 5;
				bEscape_released = false;
				buffer = 20;
			}
			if (keyboardState[SDL_SCANCODE_BACKSPACE] && bBackspace_released) {
				dev->controlData[3] |= 0x01 << 5;
				bBackspace_released = false;
			}
			if (keyboardState[SDL_SCANCODE_RETURN] && bReturn_released) {
				dev->controlData[3] |= 0x01 << 6;
				bReturn_released = false;
			}
			if (keyboardState[SDL_SCANCODE_E]) {
				dev->controlData[3] |= 0x01 << 7;
			}
			if (keyboardState[SDL_SCANCODE_R]) {
				dev->controlData[3] |= 0x01 << 4;
			}
			if (keyboardState[SDL_SCANCODE_MINUS]) {
				dev->controlData[20] |= 0x01 << 0; // black
			}
			if (keyboardState[SDL_SCANCODE_EQUALS]) {
				dev->controlData[20] |= 0x01 << 1; // white
			}
			if (keyboardState[SDL_SCANCODE_S]) {
				dev->controlData[5] = 0;
			}
			if (keyboardState[SDL_SCANCODE_X]) {
				dev->controlData[5] = 255;
			}
			if (keyboardState[SDL_SCANCODE_Z]) {
				dev->controlData[4] = 0;
			}
			if (keyboardState[SDL_SCANCODE_C]) {
				dev->controlData[4] = 255;
			}
			if (keyboardState[SDL_SCANCODE_UP] || keyboardState[keybinds.up]) {
				dev->controlData[7] = 0;
			}
			if (keyboardState[SDL_SCANCODE_DOWN] || keyboardState[keybinds.down]) {
				dev->controlData[7] = 255;
			}
			if (keyboardState[SDL_SCANCODE_LEFT] || keyboardState[keybinds.left]) {
				dev->controlData[6] = 0;
			}
			if (keyboardState[SDL_SCANCODE_RIGHT] || keyboardState[keybinds.right]) {
				dev->controlData[6] = 255;
			}
			if (keyboardState[SDL_SCANCODE_1]) {
				dev->controlData[3] |= 0x01 << 2;
			}
			if (keyboardState[SDL_SCANCODE_2]) {
				dev->controlData[3] |= 0x01 << 3;
			}
		}
		else {
			if (ParkEd->m_state == EEditorState::vEDITING) {

				/******************************************************************/
				/*                                                                */
				/*  Gameplay (ParkEd editing) PC controls						  */
				/******************************************************************/

				if (keyboardState[SDL_SCANCODE_ESCAPE] && !buffer) {
					dev->controlData[2] |= 0x01 << 3;
					buffer = 20;
				}
				if (keyboardState[keybinds.grind]) {
					dev->controlData[3] |= 0x01 << 4;
				}
				if (keyboardState[keybinds.grab]) {
					dev->controlData[3] |= 0x01 << 5;
				}
				if (keyboardState[keybinds.ollie] && !buffer) {
					dev->controlData[3] |= 0x01 << 6;
					buffer = 20;
				}
				if (keyboardState[keybinds.kick]) {
					dev->controlData[3] |= 0x01 << 7;
				}
				if (inputsettings.isPs2Controls) {
					if (keyboardState[keybinds.rightSpin]) {
						dev->controlData[20] |= 0x01 << 0; // Raise
					}
					if (keyboardState[keybinds.leftSpin]) {
						dev->controlData[3] |= 0x01 << 2; // Lower
					}
					if (keyboardState[keybinds.nollie]) {
						dev->controlData[3] |= 0x01 << 0;
					}
					if (keyboardState[keybinds.switchRevert]) {
						dev->controlData[20] |= 0x01 << 1;
					}
				}
				else {
					if (keyboardState[keybinds.rightSpin]) {
						dev->controlData[3] |= 0x01 << 2; // Raise
					}
					if (keyboardState[keybinds.leftSpin]) {
						dev->controlData[3] |= 0x01 << 0; // Lower
					}
					if (keyboardState[keybinds.caveman]) { // white L1 swapped to act as black on kb
						dev->controlData[20] |= 0x01 << 0;
					}
					if (keyboardState[keybinds.caveman2]) { // black R1 swapped to act as white on kb
						 dev->controlData[20] |= 0x01 << 1;
					}
				}
			}
			else {

				/******************************************************************/
				/*                                                                */
				/*  Gameplay (in level) controls								  */
				/******************************************************************/

				if (keyboardState[SDL_SCANCODE_ESCAPE] && bEscape_released && !buffer) {
					dev->controlData[2] |= 0x01 << 3;
					bEscape_released = false;
					buffer = 20;
				}
				if (keyboardState[keybinds.cameraToggle]) {
					dev->controlData[2] |= 0x01 << 0;
				}
				if (keyboardState[keybinds.cameraSwivelLock]) {
					dev->controlData[2] |= 0x01 << 2;
				}
				if (keyboardState[keybinds.grind]) {
					dev->controlData[3] |= 0x01 << 4;
					dev->controlData[12] = 0xff;
				}
				if (keyboardState[keybinds.grab]) {
					dev->controlData[3] |= 0x01 << 5;
					dev->controlData[13] = 0xff;
				}
				if (keyboardState[keybinds.ollie]) {
					dev->controlData[3] |= 0x01 << 6;
					dev->controlData[14] = 0xff;
				}
				if (keyboardState[keybinds.kick]) {
					dev->controlData[3] |= 0x01 << 7;
					dev->controlData[15] = 0xff;
				}
				// Shoulders
				if (inputsettings.isPs2Controls) {
					if (keyboardState[keybinds.rightSpin]) {
						dev->controlData[3] |= 0x01 << 3;
						dev->controlData[17] = 0xff;
					}
					if (keyboardState[keybinds.switchRevert]) {
						dev->controlData[3] |= 0x01 << 1;
						dev->controlData[19] = 0xff;
					}
					if (keyboardState[keybinds.leftSpin]) {
						dev->controlData[3] |= 0x01 << 2;
						dev->controlData[16] = 0xff;
					}
					if (keyboardState[keybinds.nollie]) {
						dev->controlData[3] |= 0x01 << 0;
						dev->controlData[18] = 0xff;
					}
					if (keyboardState[keybinds.leftSpin] && keyboardState[keybinds.nollie]) {
						dev->controlData[20] |= 0x01 << 0;
					}
				}
				else {
					// Merge Switch/Revert + Right Spin for PC controls
					if (keyboardState[keybinds.rightSpin]) {
						// Right spin
						dev->controlData[3] |= 0x01 << 3;
						dev->controlData[17] = 0xff;
						// Revert
						dev->controlData[3] |= 0x01 << 1;
						dev->controlData[19] = 0xff;
					}
					// Merge Nollie + Left Spin for PC controls
					if (keyboardState[keybinds.leftSpin]) {
						// Spin left
						dev->controlData[3] |= 0x01 << 2;
						dev->controlData[16] = 0xff;
						// Nollie
						dev->controlData[3] |= 0x01 << 0;
						dev->controlData[18] = 0xff;
					}
					if (keyboardState[keybinds.caveman]) {
						dev->controlData[20] |= 0x01 << 1;
					}
					if (keyboardState[keybinds.caveman2]) {
						dev->controlData[20] |= 0x01 << 0;
					}
				}
			}
			// ParkEditor item control
			if (keyboardState[keybinds.item_up]) {
				dev->controlData[2] |= 0x01 << 4;
				dev->controlData[10] = 0xFF;
			}
			if (keyboardState[keybinds.item_right]) {
				dev->controlData[2] |= 0x01 << 5;
				dev->controlData[8] = 0xFF;
			}
			if (keyboardState[keybinds.item_down]) {
				dev->controlData[2] |= 0x01 << 6;
				dev->controlData[11] = 0xFF;
			}
			if (keyboardState[keybinds.item_left]) {
				dev->controlData[2] |= 0x01 << 7;
				dev->controlData[9] = 0xFF;
			}
			// Sticks - NOTE: Because these keys are very rarely used/important, SOCD handling is just to cancel
			// right
			// x
			if (keyboardState[keybinds.cameraLeft] && !keyboardState[keybinds.cameraRight]) {
				dev->controlData[4] = 0;
			}
			if (keyboardState[keybinds.cameraRight] && !keyboardState[keybinds.cameraLeft]) {
				dev->controlData[4] = 255;
			}
			// y
			if (keyboardState[keybinds.cameraUp] && !keyboardState[keybinds.cameraDown]) {
				dev->controlData[5] = 0;
			}
			if (keyboardState[keybinds.cameraDown] && !keyboardState[keybinds.cameraUp]) {
				dev->controlData[5] = 255;
			}
			// left
			// x
			if (keyboardState[keybinds.left] && !keyboardState[keybinds.right]) {
				dev->controlData[6] = 0;
			}
			if (keyboardState[keybinds.right] && !keyboardState[keybinds.left]) {
				dev->controlData[6] = 255;
			}
			// y
			if (keyboardState[keybinds.up] && !keyboardState[keybinds.down]) {
				dev->controlData[7] = 0;
			}
			if (keyboardState[keybinds.down] && !keyboardState[keybinds.up]) {
				dev->controlData[7] = 255;
			}
		}
	}
	else if (!ParkEd) { // Only needed to be able to skip intro movies manually. The ParkEd instance isn't initialized yet when movies are displayed
		if (keyboardState[keybinds.ollie] || keyboardState[SDL_SCANCODE_ESCAPE]) {
			dev->controlData[2] |= 0x01 << 3;
		}
	}
}

// EXTREME HACK: keep pause buffer behavior for speedrunning
// when transitioning from menu to game and pause is held, pulse off for one frame
// this only applies to controller at the moment, the bind switching makes it a bit too complex for keyboard while not breaking the CAS menu
// Thanks PARTYMANX
uint8_t prev_menu_button_pause_buffer = 0;

void doPauseBuffer(device* dev) {

	uint8_t buttonPressed = (dev->controlData[2] & (0x01 << 3)) != 0;

	if (isInMenu == 1 && buttonPressed && prev_menu_button_pause_buffer == 1) {
		dev->controlData[2] &= ~(0x01 << 3);
	}

	if (isInMenu && buttonPressed) {
		prev_menu_button_pause_buffer = 1;
	}
	else {
		prev_menu_button_pause_buffer = 0;
	}
}


/******************************************************************/
/*                                                                */
/* Text input													  */
/******************************************************************/

void do_key_input(SDL_KeyCode key) {
	if (!*keyboard_on_screen) {
		if (key == SDLK_RETURN) {
			m_keyinput(0x19, 0);
		}
		else if (key == SDLK_F1) {
			//0x1B already taken by ESC, lookup is patched to detect F1
			m_keyinput(0x1F, 0);
		}
		else if (key == SDLK_F2) {
			m_keyinput(0x1C, 0);
		}
		else if (key == SDLK_F3) {
			m_keyinput(0x1D, 0);
		}
		else if (key == SDLK_F4 && !(uint8_t*)SDL_GetKeyboardState(NULL)[0xE2]) {
			m_keyinput(0x1E, 0);
		}
		return;
	}

	int32_t key_out = 0;
	uint8_t modstate = SDL_GetModState();
	uint8_t shift = SDL_GetModState() & KMOD_SHIFT;
	uint8_t caps = SDL_GetModState() & KMOD_CAPS;

	if (key == SDLK_RETURN) {
		key_out = 0x0D;    // CR
	}
	else if (key == SDLK_BACKSPACE) {
		key_out = 0x08;    // BS
	}
	else if (key == SDLK_ESCAPE && bEscape_released) {
		bEscape_released = false;
		buffer = 20;
		key_out = 0x1B;    // ESC
	}
	else if (key == SDLK_KP_ENTER) {
		key_out = 0x0D;
	}
	else {
		key_out = -1;
	}
	if (key_out != -1) {
		m_keyinput(key_out, 0);
	}
}

void do_text_input(char* text) {

	m_keyinput(text[0], 0);
	/*
	if (strlen(text) == 1) {
		m_keyinput(text[0], 0);
	}
	else {
		Log::TypedLog(CHN_SDL, "Input text '%s' > 1 byte!!\n");
	}
	*/
}


/******************************************************************/
/*                                                                */
/* Event processing												  */
/******************************************************************/

void handleInputEvent(SDL_Event* e) {
	switch (e->type) {
	case SDL_CONTROLLERDEVICEADDED:
		if (SDL_IsGameController(e->cdevice.which)) {
			Log::TypedLog(CHN_SDL, "Adding controller: %d\n", e->cdevice.which);
			addController(e->cdevice.which);
		}
		else {
			Log::TypedLog(CHN_SDL, "Not a game controller: %s\n", SDL_JoystickNameForIndex(e->cdevice.which));
		}
		return;
	case SDL_CONTROLLERDEVICEREMOVED: {
		SDL_GameController* controller = SDL_GameControllerFromInstanceID(e->cdevice.which);
		if (controller) {
			Log::TypedLog(CHN_SDL, "Removed controller\n");
			removeController(controller);
		}
		return;
	}
	case SDL_JOYDEVICEADDED:
		Log::TypedLog(CHN_SDL, "Joystick added: %s\n", SDL_JoystickNameForIndex(e->jdevice.which));
		return;
	case SDL_KEYDOWN:
		setUsingKeyboard(1);
		do_key_input((SDL_KeyCode)e->key.keysym.sym);
		return;
	case SDL_KEYUP:
		if (e->key.keysym.scancode == SDL_SCANCODE_SPACE && !bSpace_released)
			bSpace_released = true;
		else if (e->key.keysym.scancode == SDL_SCANCODE_BACKSPACE && !bBackspace_released)
			bBackspace_released = true;
		else if (e->key.keysym.scancode == SDL_SCANCODE_ESCAPE && !bEscape_released)
			bEscape_released = true;
		else if (e->key.keysym.scancode == SDL_SCANCODE_RETURN && !bReturn_released)
			bReturn_released = true;
		return;
	case SDL_CONTROLLERBUTTONDOWN: {
		SDL_GameController* controller = SDL_GameControllerFromInstanceID(e->cdevice.which);

		int idx = SDL_GameControllerGetPlayerIndex(controller);
		if (idx == -1) {
			addplayer(controller);
		}
		else if (players[idx].lockedOut) {
			players[idx].lockedOut++;
		}
		setUsingKeyboard(0);
		return;
	}
	case SDL_CONTROLLERBUTTONUP: {
		SDL_GameController* controller = SDL_GameControllerFromInstanceID(e->cdevice.which);

		int idx = SDL_GameControllerGetPlayerIndex(controller);
		if (idx != -1 && players[idx].lockedOut) {
			players[idx].lockedOut--;
		}
		return;
	}
	case SDL_CONTROLLERAXISMOTION:
		setUsingKeyboard(0);
		return;
	case SDL_TEXTINPUT:
		do_text_input(e->text.text);
		return;
	default:
		return;
	}
}

void handleQuitEvent(SDL_Event* e) {
	switch (e->type) {
	case SDL_QUIT: {
		if (inputsettings.savewindowposition)
			dumpWindowPosition();

		*shouldQuit = 1;
		return;
	}
	default:
		return;
	}
}

void fastExit() {
	exit(0);
}

void __cdecl processController(device* dev) {
	// Cheating: replace type with index
	//dev->type = 70;

	dev->capabilities = 0x0003;
	dev->num_actuators = 2;
	dev->vibrationData_max[0] = 255;
	dev->vibrationData_max[1] = 255;
	dev->state = 2;
	//dev->actuatorsDisabled = 0;

	handleEvents();
	
	dev->isValid = 0;
	dev->isPluggedIn = 0;

	dev->controlData[0] = 0;
	dev->controlData[1] = 0x70;

	// Buttons bitmap
	// This bitmap may not work how you would expect. each bit is 1 if the button is *up*, not down
	// The original code sets this initial value at 0xff and XOR's each button bit with the map
	// This scheme cannot be continuously composited, so instead we OR each button with the bitmap and bitwise negate it after all controllers are processed
	dev->controlData[2] = 0x00;
	dev->controlData[3] = 0x00;

	// Buttons
	dev->controlData[12] = 0;
	dev->controlData[13] = 0;
	dev->controlData[14] = 0;
	dev->controlData[15] = 0;

	// shoulders
	dev->controlData[16] = 0;
	dev->controlData[17] = 0;
	dev->controlData[18] = 0;
	dev->controlData[19] = 0;

	// d-pad
	dev->controlData[8] = 0;
	dev->controlData[9] = 0;
	dev->controlData[10] = 0;
	dev->controlData[11] = 0;

	// sticks
	dev->controlData[4] = 127;
	dev->controlData[5] = 127;
	dev->controlData[6] = 127;
	dev->controlData[7] = 127;

	dev->controlData[20] = 0;

	if (dev->port == 0) {
		dev->isValid = 1;
		dev->isPluggedIn = 1;

		if (!*keyboard_on_screen) {
			pollKeyboard(dev);
		}
	}
	if (dev->port < MAX_PLAYERS) {
		if (players[dev->port].controller && !players[dev->port].lockedOut) {
			pollController(dev, players[dev->port].controller);
		}
	}

	if (dev->port == 0) {
		doPauseBuffer(dev);
	}

	dev->controlData[2] = ~dev->controlData[2];
	dev->controlData[3] = ~dev->controlData[3];

	if (0xFFFF ^ ((dev->controlData[2] << 8) | dev->controlData[3])) {
		dev->pressed = 1;
	}
	else {
		dev->pressed = 0;
	}
	if (~dev->controlData[2] & 0x01 << 3 || ~dev->controlData[3] & 0x01 << 6) {
		dev->start_or_a_pressed = 1;
	}
	else {
		dev->start_or_a_pressed = 0;
	}
}


/******************************************************************/
/*                                                                */
/* Initialization												  */
/******************************************************************/

void __stdcall initManager() {
	GetModuleFileName(NULL, (LPSTR)&executableDirectory3, MAX_PATH);

	// Find last slash
	char* exe = strrchr((LPSTR)executableDirectory3, '\\');
	if (exe) {
		*(exe + 1) = '\0';
	}

	// Init SDL here
	SDL_Init(SDL_INIT_GAMECONTROLLER);
	SDL_SetHint(SDL_HINT_GAMECONTROLLER_USE_BUTTON_LABELS, "false");

	char controllerDbPath[MAX_PATH];
	int result = sprintf_s(controllerDbPath, MAX_PATH, "%s%s", executableDirectory3, "gamecontrollerdb.txt");

	if (result) {
		result = SDL_GameControllerAddMappingsFromFile(controllerDbPath);
		if (result) {
			Log::TypedLog(CHN_SDL, "Loaded mappings\n");
		}
		else {
			Log::TypedLog(CHN_SDL, "Failed to load %s\n", controllerDbPath);
		}

	}
	loadSettings(&inputsettings);
	loadKeyBinds(&keybinds);
	loadControllerBinds(&padbinds);

	initSDLControllers();

	registerEventHandler(handleInputEvent);

	// Fast exit
	registerEventHandler(handleQuitEvent);
	patchCall((void*)0x004BFF1E, fastExit);

	if (inputsettings.isPs2Controls) {
		patchPs2Buttons();
	}
	
	// Add missing key info: Since we don't use the launcher, no registry values for our keybinds are set.
	// The game normally loads keybinds found in the registry and stores them at these addresses (starting at 0x007D1160).
	// These patched values simulate the launcher's behavior of storing its defined keybinds in memory so that they can be displayed in game 
	// Without this, no user-defined keybinds would be displayed for things like the edit tricks menu (e.g. "<- + KP4" would only be "<- +   ")
	patchDWord((void*)0x007D1160, convert_SDL_to_OIS_keycode(keybinds.ollie));
	patchDWord((void*)0x007D1164, convert_SDL_to_OIS_keycode(keybinds.grab));
	patchDWord((void*)0x007D1168, convert_SDL_to_OIS_keycode(keybinds.kick));
	patchDWord((void*)0x007D116C, convert_SDL_to_OIS_keycode(keybinds.grind));
	patchDWord((void*)0x007D1180, convert_SDL_to_OIS_keycode(keybinds.leftSpin));
	patchDWord((void*)0x007D1184, convert_SDL_to_OIS_keycode(keybinds.rightSpin));
	patchDWord((void*)0x007D1178, convert_SDL_to_OIS_keycode(keybinds.caveman));
	patchDWord((void*)0x007D117C, convert_SDL_to_OIS_keycode(keybinds.caveman2));
}

void patchPs2Buttons() {
	Log::TypedLog(CHN_SDL, "Patching PS2 Buttons\n");
	patchByte((void*)(0x00452226 + 2), 0x05);	// Change PC platform to gamecube. This just makes it default to ps2 controls. Needed for rail DD on R2

	// walk acid drop.
	// R2 on PS2
	patchBytesM((void*)0x00463773, (BYTE*)"\x0F\x85\x09\x00\x00\x00", 6);

	//in_air_acid_drop
	patchBytesM((void*)0x0056CAA8, (BYTE*)"\x75\x09", 2);

	//in_air_to_break
	patchBytesM((void*)0x0056C00D, (BYTE*)"\x75\x09", 2);

	//r2l2_air_recover
	patchBytesM((void*)0x0056BB6F, (BYTE*)"\x75\x07", 2);

	//??
	patchNop((void*)0x0046376C, 13);

	//??
	patchNop((void*)0x0046383C, 13);

	//r2l2_break_vert1
	patchBytesM((void*)0x00565488, (BYTE*)"\x0F\x85\x0D\x00\x00\x00", 6);

	//r2l2_break_vert2
	patchBytesM((void*)0x005654A2, (BYTE*)"\x0F\x85\x0D\x00\x00\x00", 6);
}

void initializeInstance() {

	if (!instances_initialized) {
		ParkEd = (EdCParkEditorInstance*)*(uint32_t*)(0x007C992C);
		Skate = (SkateInstance*)*(uint32_t*)(0x0076A788);
		//GoalMan = GameGetGoalManager_Native(); //Points to start of GoalMan. Not needed (yet)

		if (ParkEd && Skate)
			instances_initialized = true;
	}
	else {
		// Gross? Callee cleans up
		patchCall((void*)0x00408123, (void*)0x004A4F00);
	}
}

void patchInput() {

	// Patch SIO::Device
	// Process
	patchThisToCdecl((void*)0x00581A20, &processController);
	patchBytesM((void*)(0x00581A20 + 7), (BYTE*)"\xC2\x04\x00", 3); //ret 4

	// set_actuator
	// Don't call read_data in activate_actuators
	patchNop((void*)0x00581730, 7); //call_device_read
	patchJump((void*)0x005C4200, set_actuators);

	// Initialize ParkEdInstance handle here
	patchCall((void*)0x00408123, initializeInstance);

	// Init input patch - nop direct input setup
	patchNop((void*)0x005C56AB, 47);
	patchCall((void*)(0x005C56AB + 5), &initManager);

	// Some config call relating to the dinput devices
	patchNop((void*)0x004C084D, 5);

	// Patch F1 button for taunt
	patchByte((void*)(0x004C82E5+1), 0x1F);

	// Init events
	initEvents();
	patchJump((void*)0x005C30E0, handleEvents);
}
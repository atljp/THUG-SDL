#include <windows.h>

#include <stdio.h>
#include <stdint.h>

#include <patch.h>
//#include <global.h>

#include <SDL.h>
#include <SDL_syswm.h>

void patchInput();
void patchPs2Buttons();
void patchInput();
void __cdecl set_actuators(int port, uint16_t hight, uint16_t low);
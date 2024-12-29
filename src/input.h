#include <windows.h>

#include <stdio.h>
#include <stdint.h>
#include <patch.h>
#include <event.h>


#include <SDL.h>
#include <SDL_syswm.h>

void patchInput();
void patchPs2Buttons();
void patchInput();
void processEvent(SDL_Event* e);
void __cdecl set_actuators(int port, uint16_t hight, uint16_t low);
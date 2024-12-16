#pragma once
#include <map>
#include <SDL.h>
#include <SDL_syswm.h>
//https://github.com/wgois/OIS/blob/27928350a7a7be9dd7e12e44d534a20265784ae2/includes/OISKeyboard.h#L80
//SDL Scancodes from SDL2 - 2.28.3\include\SDL_scancode.h

// Define constants for OIS keycodes
const uint8_t OIS_KC_UNASSIGNED = 0;
const uint8_t OIS_KC_A = 0x1E;
const uint8_t OIS_KC_B = 0x30;
const uint8_t OIS_KC_C = 0x2E;
const uint8_t OIS_KC_D = 0x20;
const uint8_t OIS_KC_E = 0x12;
const uint8_t OIS_KC_F = 0x21;
const uint8_t OIS_KC_G = 0x22;
const uint8_t OIS_KC_H = 0x23;
const uint8_t OIS_KC_I = 0x17;
const uint8_t OIS_KC_J = 0x24;
const uint8_t OIS_KC_K = 0x25;
const uint8_t OIS_KC_L = 0x26;
const uint8_t OIS_KC_M = 0x32;
const uint8_t OIS_KC_N = 0x31;
const uint8_t OIS_KC_O = 0x18;
const uint8_t OIS_KC_P = 0x19;
const uint8_t OIS_KC_Q = 0x10;
const uint8_t OIS_KC_R = 0x13;
const uint8_t OIS_KC_S = 0x1F;
const uint8_t OIS_KC_T = 0x14;
const uint8_t OIS_KC_U = 0x16;
const uint8_t OIS_KC_V = 0x2F;
const uint8_t OIS_KC_W = 0x11;
const uint8_t OIS_KC_X = 0x2D;
const uint8_t OIS_KC_Y = 0x15;
const uint8_t OIS_KC_Z = 0x2C;
const uint8_t OIS_KC_1 = 0x02;
const uint8_t OIS_KC_2 = 0x03;
const uint8_t OIS_KC_3 = 0x04;
const uint8_t OIS_KC_4 = 0x05;
const uint8_t OIS_KC_5 = 0x06;
const uint8_t OIS_KC_6 = 0x07;
const uint8_t OIS_KC_7 = 0x08;
const uint8_t OIS_KC_8 = 0x09;
const uint8_t OIS_KC_9 = 0x0A;
const uint8_t OIS_KC_0 = 0x0B;
const uint8_t OIS_KC_ESC = 0x01;
const uint8_t OIS_KC_MINUS = 0x0C;
const uint8_t OIS_KC_EQUALS = 0x0D;
const uint8_t OIS_KC_BACK = 0x0E;
const uint8_t OIS_KC_TAB = 0x0F;
const uint8_t OIS_KC_LBRACKET = 0x1A;
const uint8_t OIS_KC_RBRACKET = 0x1B;
const uint8_t OIS_KC_RETURN = 0x1C;
const uint8_t OIS_KC_LCONTROL = 0x1D;
const uint8_t OIS_KC_SEMICOLON = 0x27;
const uint8_t OIS_KC_APOSTROPHE = 0x28;
const uint8_t OIS_KC_GRAVE = 0x29;
const uint8_t OIS_KC_LSHIFT = 0x2A;
const uint8_t OIS_KC_BACKSLASH = 0x2B;
const uint8_t OIS_KC_COMMA = 0x33;
const uint8_t OIS_KC_PERIOD = 0x34;
const uint8_t OIS_KC_SLASH = 0x35;
const uint8_t OIS_KC_RSHIFT = 0x36;
const uint8_t OIS_KC_LMENU = 0x38;
const uint8_t OIS_KC_SPACE = 0x39;
const uint8_t OIS_KC_CAPITAL = 0x3A;
const uint8_t OIS_KC_SCROLL = 0x46;
const uint8_t OIS_KC_NUMLOCK = 0x45;
const uint8_t OIS_KC_ADD = 0x4E;
const uint8_t OIS_KC_SUBTRACT = 0x4A;
const uint8_t OIS_KC_MULTIPLY = 0x37;
const uint8_t OIS_KC_DECIMAL = 0x53;
const uint8_t OIS_KC_NUMPAD0 = 0x52;
const uint8_t OIS_KC_NUMPAD1 = 0x4F;
const uint8_t OIS_KC_NUMPAD2 = 0x50;
const uint8_t OIS_KC_NUMPAD3 = 0x51;
const uint8_t OIS_KC_NUMPAD4 = 0x4B;
const uint8_t OIS_KC_NUMPAD5 = 0x4C;
const uint8_t OIS_KC_NUMPAD6 = 0x4D;
const uint8_t OIS_KC_NUMPAD7 = 0x47;
const uint8_t OIS_KC_NUMPAD8 = 0x48;
const uint8_t OIS_KC_NUMPAD9 = 0x49;
const uint8_t OIS_KC_RCONTROL = 0x9D;
const uint8_t OIS_KC_END = 0xCF;
const uint8_t OIS_KC_PGUP = 0xC9;
const uint8_t OIS_KC_PGDOWN = 0xD1;
const uint8_t OIS_KC_INSERT = 0xD2;
const uint8_t OIS_KC_DELETE = 0xD3;

/*
TODO
    SDL_SCANCODE_RETURN = 40,
    SDL_SCANCODE_BACKSPACE = 42,
    SDL_SCANCODE_TAB = 43,
    SDL_SCANCODE_MINUS = 45,
    SDL_SCANCODE_EQUALS = 46,
    SDL_SCANCODE_LEFTBRACKET = 47,
    SDL_SCANCODE_RIGHTBRACKET = 48,
    SDL_SCANCODE_BACKSLASH = 49,
*/

// Create a map to convert SDL keycodes to OIS keycodes
std::map<uint8_t, uint8_t> SDL_to_OIS_map = {
    {SDLK_UNKNOWN, OIS_KC_UNASSIGNED},
    {SDL_SCANCODE_A, OIS_KC_A},
    {SDL_SCANCODE_B, OIS_KC_B},
    {SDL_SCANCODE_C, OIS_KC_C},
    {SDL_SCANCODE_D, OIS_KC_D},
    {SDL_SCANCODE_E, OIS_KC_E},
    {SDL_SCANCODE_F, OIS_KC_F},
    {SDL_SCANCODE_G, OIS_KC_G},
    {SDL_SCANCODE_H, OIS_KC_H},
    {SDL_SCANCODE_I, OIS_KC_I},
    {SDL_SCANCODE_J, OIS_KC_J},
    {SDL_SCANCODE_K, OIS_KC_K},
    {SDL_SCANCODE_L, OIS_KC_L},
    {SDL_SCANCODE_M, OIS_KC_M},
    {SDL_SCANCODE_N, OIS_KC_N},
    {SDL_SCANCODE_O, OIS_KC_O},
    {SDL_SCANCODE_P, OIS_KC_P},
    {SDL_SCANCODE_Q, OIS_KC_Q},
    {SDL_SCANCODE_R, OIS_KC_R},
    {SDL_SCANCODE_S, OIS_KC_S},
    {SDL_SCANCODE_T, OIS_KC_T},
    {SDL_SCANCODE_U, OIS_KC_U},
    {SDL_SCANCODE_V, OIS_KC_V},
    {SDL_SCANCODE_W, OIS_KC_W},
    {SDL_SCANCODE_X, OIS_KC_X},
    {SDL_SCANCODE_Y, OIS_KC_Y},
    {SDL_SCANCODE_Z, OIS_KC_Z},
    {SDL_SCANCODE_1, OIS_KC_1},
    {SDL_SCANCODE_2, OIS_KC_2},
    {SDL_SCANCODE_3, OIS_KC_3},
    {SDL_SCANCODE_4, OIS_KC_4},
    {SDL_SCANCODE_5, OIS_KC_5},
    {SDL_SCANCODE_6, OIS_KC_6},
    {SDL_SCANCODE_7, OIS_KC_7},
    {SDL_SCANCODE_8, OIS_KC_8},
    {SDL_SCANCODE_9, OIS_KC_9},
    {SDL_SCANCODE_0, OIS_KC_0},
    {SDL_SCANCODE_ESCAPE, OIS_KC_ESC},
    {SDL_SCANCODE_SPACE, OIS_KC_SPACE},
    {SDL_SCANCODE_RCTRL, OIS_KC_RCONTROL},
    {SDL_SCANCODE_END, OIS_KC_END},
    {SDL_SCANCODE_PAGEUP, OIS_KC_PGUP},
    {SDL_SCANCODE_PAGEDOWN, OIS_KC_PGDOWN},
    {SDL_SCANCODE_INSERT, OIS_KC_INSERT},
    {SDL_SCANCODE_DELETE, OIS_KC_DELETE},
    {SDL_SCANCODE_KP_0, OIS_KC_NUMPAD0},
    {SDL_SCANCODE_KP_1, OIS_KC_NUMPAD1},
    {SDL_SCANCODE_KP_2, OIS_KC_NUMPAD2},
    {SDL_SCANCODE_KP_3, OIS_KC_NUMPAD3},
    {SDL_SCANCODE_KP_4, OIS_KC_NUMPAD4},
    {SDL_SCANCODE_KP_5, OIS_KC_NUMPAD5},
    {SDL_SCANCODE_KP_6, OIS_KC_NUMPAD6},
    {SDL_SCANCODE_KP_7, OIS_KC_NUMPAD7},
    {SDL_SCANCODE_KP_8, OIS_KC_NUMPAD8},
    {SDL_SCANCODE_KP_9, OIS_KC_NUMPAD9},
};
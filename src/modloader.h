#pragma once
#include "config.h"
#include <unordered_set>
#include <string>


bool getModIni();
void InitMod();
void getWindowTitle(struct extmodsettings* extmodsettingsOut);
void PIPLoadPre_Wrapper(uint8_t* p_data);
void __fastcall PreMgrLoadPre_Wrapper(int ecx, int edx, uint8_t* p_data, void* arg1, void* arg2, bool arg3);
//void ParseQB_Patched(const char* p_fileName, uint8_t* p_qb, int unused, int assertIfDuplicateSymbols, bool allocateChecksumNameLookupTable);
uint8_t* __cdecl pipLoadWrapper(char* p_fileName, int unk1, int unk2, int unk3, int unk4);
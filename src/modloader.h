#pragma once
#include "config.h"
#include <unordered_set>
#include <string>


bool getModIni();
void InitMod();
void getWindowTitle(struct extmodsettings* extmodsettingsOut);
void PIPLoadPre_Wrapper(uint8_t* p_data);
void __fastcall PreMgrLoadPre_Wrapper(void* arg1, void* unused, uint8_t* p_data, char* arg3, char* arg4, char arg5);
//void ParseQB_Patched(const char* p_fileName, uint8_t* p_qb, int unused, int assertIfDuplicateSymbols, bool allocateChecksumNameLookupTable);
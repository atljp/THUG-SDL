#pragma once

#include <stdint.h>
#include <cstring>
#include <float.h>
#include <string>
#include <windows.h>
#include <iostream>

#include <QB/LazyStruct.h>
#include <patch.h>

#define CHN_MEMORY	"Mem"
#define CHN_DLL		"DLL"
#define CHN_SAVE	"Save"
#define CHN_MOD		"MOD"
#define CHN_LOG		"Log"
#define CHN_DEBUG	"Debug"
#define CHN_INI		"INI"
#define CHN_SDL		"SDL"
#define CHN_IMAGE	"Image"
#define CHN_AUDIO	"Audio"
#define CHN_NET		"Net"

namespace Log
{

	void Initialize();
	//void CoreLog(const char* to_log, const char* category = CHN_LOG);
	void PrintLog(const char* Format, ...);
	void TypedLog(const char* category, const char* Format, ...);
	void StringFromParams(char* print_dest, Script::LazyStruct* pParams, void* pScript);
	bool CFunc_PrintF(Script::LazyStruct* pParams, void* pScript);
    //void CoreWarn(char* buf, bool is_warning);
	//void Warn(const char* Format, ...);
	//void Error(const char* Format, ...);
	//void PatchCFuncs();
    //void PatchLogger();
    //void PrintStructItem(LazyStructItem *item);
	//void PrintStruct(LazyStruct *struc);
	//void PrintStruct(LazyStruct *struc, bool force);
    
   

}

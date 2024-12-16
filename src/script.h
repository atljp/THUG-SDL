#pragma once
#include "QB/LazyStruct.h"
#include "QB/LazyArray.h"
#include "QB/Qb.h"
#include "stdio.h"
#include <map>
#include "patch.h"
#include "Logger/Logger.h"
#include "QB/CFuncs.h"
#include "QB/scriptcontent.h"
#include "config.h"
#include "malloc.h"

enum ESymbolType {
	ESYMBOLTYPE_NONE = 0,
	ESYMBOLTYPE_INTEGER,
	ESYMBOLTYPE_FLOAT,
	ESYMBOLTYPE_STRING,
	ESYMBOLTYPE_LOCALSTRING,
	ESYMBOLTYPE_PAIR,
	ESYMBOLTYPE_VECTOR,
	ESYMBOLTYPE_QSCRIPT,
	ESYMBOLTYPE_CFUNCTION,
	ESYMBOLTYPE_MEMBERFUNCTION,
	ESYMBOLTYPE_STRUCTURE,
	ESYMBOLTYPE_STRUCTUREPOINTER,
	ESYMBOLTYPE_ARRAY,
	ESYMBOLTYPE_NAME,
	ESYMBOLTYPE_INTEGER_ONE_BYTE,
	ESYMBOLTYPE_INTEGER_TWO_BYTES,
	ESYMBOLTYPE_UNSIGNED_INTEGER_ONE_BYTE,
	ESYMBOLTYPE_UNSIGNED_INTEGER_TWO_BYTES,
	ESYMBOLTYPE_ZERO_INTEGER,
	ESYMBOLTYPE_ZERO_FLOAT,
};

struct DummyScript;

void patchScripts();
void patchCFuncs();
void initScriptPatch();
bool IsPS2_Patched(void* pParams, DummyScript* pScript);
bool IsXBOX_Patched(void* pParams, DummyScript* pScript);
bool GetMemCardSpaceAvailable_Patched(void* pParams, DummyScript* pScript, uint32_t a, ULARGE_INTEGER b, ULARGE_INTEGER c, ULARGE_INTEGER d, ULARGE_INTEGER e, ULARGE_INTEGER f, uint8_t p_card);
bool CreateScreenElement_Patched(Script::LazyStruct* pParams, DummyScript* pScript);
bool SetScreenElementProps_Patched(Script::LazyStruct* pParams, DummyScript* pScript);
bool SetButtonEventMappings_Patched(Script::LazyStruct* pParams, DummyScript* pScript);
void editScriptsInMemory();
void __fastcall sCreateScriptSymbol_Wrapper(uint32_t size, uint32_t nameChecksum, uint32_t contentsChecksum, const uint8_t* p_data, const char* p_fileName);
uint32_t __fastcall removeScript(uint32_t partChecksum);
void setCavemanKeys();
void setLadderGrabKeys();
void setDropDownKeys();
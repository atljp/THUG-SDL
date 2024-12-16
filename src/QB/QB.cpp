//------------------------------------------------
// Contains generic QB functions.
//------------------------------------------------

#include "QB.h"
#include <QB/LazyStruct.h>
#include <config.h>
#include <windows.h>

//---------------------------------------
// Run a script.
//---------------------------------------

typedef bool __cdecl RunScript_NativeCall(uint32_t name, void* params, void* object, void* object2);
RunScript_NativeCall* RunScript_Native = (RunScript_NativeCall*)(0x0040FD00);

bool RunScript(uint32_t name, Script::LazyStruct* params, void* object, void* object2)
{
	return RunScript_Native(name, params, object, 0);
}

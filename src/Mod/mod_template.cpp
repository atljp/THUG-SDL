#include "mod_template.h"
/*
* Add custom CFuncs or patches for your mod here!
* The added CFuncs can be called from the scripts, in this example: 
	MYMOD_ToggleWallrideAnywhere On
	MYMOD_ToggleWallrideAnywhere Off
* 
* 
* 
bool CFunc_ToggleWallrideAnywhere(Script::LazyStruct* pParams, DummyScript* pScript) {
	if (pParams->ContainsFlag(0xF649D637)) { //on
		//Do something if parameter was on
		patchJump((void*)0x005603EF, (void*)0x005AD5D5); //Jump to codecave
		if (*(int*)0x005AD5D5 == 0xCC) //Only patch once
			patchBytesM((void*)0x005AD5D5, (BYTE*)"\x80\xBE\x28\x02\x00\x00\x00\x75\x07\xC6\x86\x2A\x02\x00\x00\x01\x5E\x5B\x83\xC4\x10\xC2\x08\x00", 24);
	}
	else if (pParams->ContainsFlag(0xD443A2BC)) { //off
		// Do something if parameter was off
		patchBytesM((void*)0x005603EF, (BYTE*)"\x5E\x5B\x83\xC4\x10", 5); //Restore original code
	}
	return true;
}

//Add a call to this function in DllMain
void addCustomCFuncs() {
	Log::TypedLog(CHN_DLL, "Adding CFuncs\n");
	CFuncs::AddFunction("MYMOD_ToggleWallrideAnywhere", CFunc_ToggleWallrideAnywhere);
}
*/


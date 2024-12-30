//------------------------------------------------
//
//  ERROR MANAGER
//  (Doesn't work on Windows 11)
//  Shows information about registers and the callstack when the game crashes.
//  Credit: Zedek the Plague Doctor (Guitar Hero Worldtour Definitive Edition / reTHAWed)
//
//------------------------------------------------

#pragma once

namespace ErrorManager
{
    enum ExceptionHandlerResult
    {
        EHR_IGNORE = 0,
        EHR_CONTINUE = 1,
        EHR_TERMINATE = 2
    };
    
	struct ErrorEntry
	{
		uint32_t code;
		const char* codeName;
        ExceptionHandlerResult codeResult;
	};
	
	extern bool b_HandlerAssigned;
	void AssignHandler();
	void UnassignHandler();
    void Initialize();
    
    void IgnoreVectoredExceptions(bool ignored);
}

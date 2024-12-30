#include <cstdio>
#include <cstring>
#include <stdio.h>
#include <windows.h>
#include <dbghelp.h>


#include <Logger/Logger.h>
#include "ErrorManager.h"

//~ #define NUM_ERRORCODES 19
#define NUM_ERRORCODES 19
#define ERR_LENGTH 4096

namespace ErrorManager
{
    bool b_Enabled = false;
	bool b_HandlerAssigned = false;
    bool b_HasErrored = false;
    bool b_IgnoreVectoredExceptions = false;
    
    PVOID curVectorHandler;
	
	ErrorEntry error_codes[NUM_ERRORCODES] = {
		{0xC0000005, "EXCEPTION_ACCESS_VIOLATION", EHR_TERMINATE},
		{0xC000008C, "EXCEPTION_ARRAY_BOUNDS_EXCEEDED", EHR_TERMINATE},
		{0x80000003, "EXCEPTION_BREAKPOINT", EHR_CONTINUE},
		{0x80000002, "EXCEPTION_DATATYPE_MISALIGNMENT", EHR_TERMINATE},
		{0xC000008D, "EXCEPTION_FLT_DENORMAL_OPERAND", EHR_TERMINATE},
		{0xC000008F, "EXCEPTION_FLT_INEXACT_RESULT", EHR_TERMINATE},
		{0xC0000090, "EXCEPTION_FLT_INVALID_OPERATION", EHR_TERMINATE},
		{0xC0000091, "EXCEPTION_FLT_OVERFLOW", EHR_TERMINATE},
		{0xC0000092, "EXCEPTION_FLT_STACK_CHECK", EHR_TERMINATE},
		{0xC0000093, "EXCEPTION_FLT_UNDERFLOW", EHR_TERMINATE},
		{0xC000001D, "EXCEPTION_ILLEGAL_INSTRUCTION", EHR_TERMINATE},
		{0xC0000006, "EXCEPTION_IN_PAGE_ERROR", EHR_TERMINATE},
		{0xC0000094, "EXCEPTION_INT_DIVIDE_BY_ZERO", EHR_TERMINATE},
		{0xC0000095, "EXCEPTION_INT_OVERFLOW", EHR_TERMINATE},
		{0xC0000026, "EXCEPTION_INVALID_DISPOSITION", EHR_CONTINUE},
		{0xC0000025, "EXCEPTION_NONCONTINUABLE_EXCEPTION", EHR_TERMINATE},
		{0xC0000096, "EXCEPTION_PRIV_INSTRUCTION", EHR_TERMINATE},
		{0x80000004, "EXCEPTION_SINGLE_STEP", EHR_CONTINUE},
		{0xC00000FD, "EXCEPTION_STACK_OVERFLOW", EHR_TERMINATE}
	};
	
	// ------------------------------------
	
	void FormatExceptionRecords(PCONTEXT context, char* buffer)
	{
		context -> ContextFlags = CONTEXT_FULL;
		
		STACKFRAME64 StackFrame;
		memset(&StackFrame, 0, sizeof(StackFrame));
		
		// process, thread, i386, eip, ebp, esp
		// process, thread, machine, pc, frameptr, stackptr
		
		StackFrame.AddrPC.Offset = context -> Eip;
		StackFrame.AddrFrame.Offset = context -> Ebp;
		StackFrame.AddrStack.Offset = context -> Esp;
		
		StackFrame.AddrPC.Mode = AddrModeFlat;
		StackFrame.AddrFrame.Mode = AddrModeFlat;
		StackFrame.AddrStack.Mode = AddrModeFlat;
		
		HANDLE curThread = GetCurrentThread();
		HANDLE curProcess = GetCurrentProcess();
		
		snprintf(buffer, ERR_LENGTH, "%s\n\n", buffer);
		
		// Let's print our registry values (as QBKeys!)
		// Some of them might be stored in memory and could be helpful
		
		snprintf(buffer, ERR_LENGTH, "%sEAX: 0x%08x\n", buffer, context->Eax);
		snprintf(buffer, ERR_LENGTH, "%sEBX: 0x%08x\n", buffer, context->Ebx);
		snprintf(buffer, ERR_LENGTH, "%sECX: 0x%08x\n", buffer, context->Ecx);
		snprintf(buffer, ERR_LENGTH, "%sEDX: 0x%08x\n", buffer, context->Edx);
		snprintf(buffer, ERR_LENGTH, "%sEBP: 0x%08x\n", buffer, context->Ebp);
		snprintf(buffer, ERR_LENGTH, "%sEDI: 0x%08x\n", buffer, context->Edi);
		snprintf(buffer, ERR_LENGTH, "%sESP: 0x%08x\n", buffer, context->Esp);
		
        while (true)
		{
			if (StackWalk64(IMAGE_FILE_MACHINE_I386, curProcess, curThread, &StackFrame, context, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL) == 0)
				break;
				
			if (StackFrame.AddrFrame.Offset == 0)
				break;
				
			snprintf(buffer, ERR_LENGTH, "%s\n[0x%08p]", buffer, StackFrame.AddrPC.Offset);
		}
	}
	
	// ------------------------------------
	
    // Core exception handler!
    // Returns true to continue execution, false to quit
    
    ExceptionHandlerResult HandleException_Core(_EXCEPTION_POINTERS *ExceptionInfo)
    {    
        // No longer interested in handling errors.
        SetUnhandledExceptionFilter(0);
        
        if (!ExceptionInfo)
        {
            Log::TypedLog(CHN_DLL, "ExceptionInfo missing.\n");
            return EHR_CONTINUE;
        }
        
        PEXCEPTION_RECORD record = ExceptionInfo -> ExceptionRecord;
		
		uint32_t exceptionCode = record -> ExceptionCode;
        
        // Special exception for breakpoint
        // (We would like to TERMINATE the application!)
        //
        // (This seems to only be encountered when the program exits)
		
		// ------------------
		
        ExceptionHandlerResult exceptionResult = EHR_CONTINUE;
		const char* exceptionString = "UNKNOWN_EXCEPTION";
		bool foundError = false;
        
		for (int i=0; i<NUM_ERRORCODES; i++)
		{
			if (error_codes[i].code == exceptionCode)
			{
                foundError = true;
				exceptionString = error_codes[i].codeName;
                exceptionResult = error_codes[i].codeResult;
				break;
			}
		}
        
        if (!foundError)
        {
            if (!b_IgnoreVectoredExceptions)
                Log::TypedLog(CHN_DEBUG, "Unknown exception 0x%08x occurred at 0x%08x. Continuing.\n", exceptionCode, ExceptionInfo -> ContextRecord -> Eip);
                
            return EHR_CONTINUE;
        }
        
        // It's a non-fatal error, we can probably ignore it. PROBABLY.
        // In that case, let's just print some info about it to the console.
        
        if (exceptionResult == EHR_CONTINUE)
        {
            if (!b_IgnoreVectoredExceptions)
                Log::TypedLog(CHN_DEBUG, "Non-fatal exception %s occurred at 0x%08x. Continuing.\n", exceptionString, ExceptionInfo -> ContextRecord -> Eip);
                
            return EHR_CONTINUE;
        }
        
        // Prevent duplicate errors. Hopefully.
        if (b_HasErrored)
            return exceptionResult;
            
        b_HasErrored = true;
		
		// ------------------
		
		char errorBuffer[ERR_LENGTH];
		
		snprintf(errorBuffer, ERR_LENGTH, "!! -- FATAL ERROR AT 0x%08x -- !!\n\n%s", ExceptionInfo -> ContextRecord -> Eip, exceptionString);
		
		// ------------------
		
		// Exception access violation
		if (exceptionCode == 0xC0000005)
		{
			const char* rwMode = (record -> ExceptionInformation[0]) ? "Write" : "Read";
			snprintf(errorBuffer, ERR_LENGTH, "%s (%s of 0x%08p)", errorBuffer, rwMode, record -> ExceptionInformation[1]);
		}
		
		// ------------------
		
		FormatExceptionRecords(ExceptionInfo -> ContextRecord, errorBuffer);
		
		// ------------------

        strcat_s(errorBuffer, ERR_LENGTH, "\n\nPlease show this message to a developer for support, with a copy of your debug.txt file. This is important!");
		 
        Log::TypedLog(CHN_DLL, "  Wanting to show error...\n");
         
		// Show the error
		Log::Error(errorBuffer);

		return exceptionResult;
    }
    
	LONG HandleException(_EXCEPTION_POINTERS *ExceptionInfo)
	{
        Log::TypedLog(CHN_DLL, "EXCEPTION\n");
        
		ExceptionHandlerResult result = HandleException_Core(ExceptionInfo);
        
        switch (result)
        {
            // Terminate: Will terminate the program
            case EHR_TERMINATE:
                return EXCEPTION_EXECUTE_HANDLER;
                break;
                
            // Continue: Will continue and not crash
            case EHR_CONTINUE:
                return EXCEPTION_CONTINUE_EXECUTION;
                break;
                
            // Default: Will execute handler, AKA terminate
            default:
                return EXCEPTION_EXECUTE_HANDLER;
                break;
        }

        return EXCEPTION_EXECUTE_HANDLER;
	}
    
    LONG HandleVectoredException(_EXCEPTION_POINTERS *ExceptionInfo)
	{    
        PEXCEPTION_RECORD record = ExceptionInfo -> ExceptionRecord;
		uint32_t exceptionCode = record -> ExceptionCode;
        
        if (!b_IgnoreVectoredExceptions)
            Log::TypedLog(CHN_DLL, "VECTOR EXCEPTION: code %08x at 0x%08x\n", exceptionCode, ExceptionInfo -> ContextRecord -> Eip);
        
        ExceptionHandlerResult result = HandleException_Core(ExceptionInfo);
        
        switch (result)
        {
            // Terminate: Will terminate the program
            case EHR_TERMINATE:
                return EXCEPTION_EXECUTE_HANDLER;
                break;
                
            // Continue: Will continue and not crash
            case EHR_CONTINUE:
                return EXCEPTION_CONTINUE_EXECUTION;
                break;
                
            // Default: Will execute handler, AKA terminate
            default:
                return EXCEPTION_EXECUTE_HANDLER;
                break;
        }

        return EXCEPTION_EXECUTE_HANDLER;
    }
	
	void AssignHandler()
	{
		if (b_HandlerAssigned || !b_Enabled)
			return;
			
		b_HandlerAssigned = true;
        
        Log::TypedLog(CHN_DLL, "!! Assigned error handler !!\n");
        
        // We set both exception handlers!
        //
        // (For some people, unhandled exception doesn't
        // seem to be enough... so we'll use a vectored
        // exception handler to pick up what it doesn't.)
        
		SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER) HandleException);
        
        curVectorHandler = AddVectoredExceptionHandler(1, (PVECTORED_EXCEPTION_HANDLER) HandleVectoredException);
	}
    
    void UnassignHandler()
    {
        if (!b_HandlerAssigned || !b_Enabled)
			return;
            
        if (curVectorHandler)
        {
            RemoveVectoredExceptionHandler(curVectorHandler);
            curVectorHandler = nullptr;
        }
            
        b_HandlerAssigned = false;
        Log::TypedLog(CHN_DLL, "Unassigning ExceptionHandler...\n");
    }
    
    void Initialize()
    {
        Log::TypedLog(CHN_DLL, "Enabling ExceptionHandler.\n");
        b_Enabled = true;
        AssignHandler();
    }
    
    void IgnoreVectoredExceptions(bool ignored) { b_IgnoreVectoredExceptions = ignored; }
}

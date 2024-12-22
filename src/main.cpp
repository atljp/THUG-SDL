#include "d3d9.h"
#include "config.h"
#include "script.h"
#include "displayconfig.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_syswm.h"
#include "Resources/MyResources.h"

#pragma data_seg (".d3d9_shared")
HINSTANCE gl_hOriginalDll;
HINSTANCE gl_hThisInstance;
#pragma data_seg ()

void InitInstance(HANDLE hModule);
void LoadOriginalDll(void);
void ExitInstance(void);

BOOL dpi_result = SetProcessDPIAware();

BOOL WINAPI DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    LPVOID lpDummy = lpReserved;
    lpDummy = NULL;
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
		InitInstance(hModule);
		InitPatch();
		InitMod();
		patchScripts();
		patchWindow();
		patchresbuffer();
		patchInput();
		//addCustomCFuncs();
		break;
		case DLL_PROCESS_DETACH: ExitInstance(); break;
		case DLL_THREAD_ATTACH: break;
		case DLL_THREAD_DETACH: break;
    }
    return TRUE;
}

// Exported function (faking d3d9.dll's one-and-only export)
IDirect3D9* WINAPI Direct3DCreate9(UINT SDKVersion) {
	// Hooking IDirect3D Object from Original Library
	typedef IDirect3D9 *(WINAPI* D3D9_Type)(UINT SDKVersion);

	D3D9_Type D3DCreate9_fn = NULL;
	if (!gl_hOriginalDll) LoadOriginalDll(); // looking for the "right d3d9.dll"
	if (gl_hOriginalDll) D3DCreate9_fn = (D3D9_Type) GetProcAddress(gl_hOriginalDll, "Direct3DCreate9");
    
	// Debug
	if (!D3DCreate9_fn) 
	{
		OutputDebugString("PROXYDLL: Pointer to original D3DCreate9 function not received ERROR ****\r\n");
		ExitProcess(0); // exit the hard way
	}
	
	// Request pointer from Original Dll. 
	IDirect3D9 *pIDirect3D9_orig = D3DCreate9_fn(SDKVersion);
	
	// Return pointer to real object
	return (pIDirect3D9_orig);
}

void InitInstance(HANDLE hModule) {
	OutputDebugString("PROXYDLL: InitInstance called.\r\n");
	
	// Initialisation
	gl_hOriginalDll = NULL;
	gl_hThisInstance = NULL;	
	
	// Storing Instance handle into global var
	gl_hThisInstance = (HINSTANCE)  hModule;
}

void LoadOriginalDll(void) {
    char buffer[MAX_PATH];
    
    // Getting path to system dir and to d3d9.dll
	GetSystemDirectory(buffer,MAX_PATH);

	// Append dll name
	strcat_s(buffer, sizeof buffer, "\\d3d9.dll");
	
	// try to load the system's d3d9.dll, if pointer empty
	if (!gl_hOriginalDll) gl_hOriginalDll = LoadLibrary(buffer);

	// Debug
	if (!gl_hOriginalDll)
	{
		OutputDebugString("PROXYDLL: Original d3d9.dll not loaded ERROR ****\r\n");
		ExitProcess(0); // exit the hard way
	}
}

void ExitInstance() {    
    OutputDebugString("PROXYDLL: ExitInstance called.\r\n");
	
	// Release the system's d3d9.dll
	if (gl_hOriginalDll)
	{
		FreeLibrary(gl_hOriginalDll);
	    gl_hOriginalDll = NULL;  
	}
}

bool GetResourcePointer(HINSTANCE Instance, int ResID, LPVOID* ppRes, DWORD* pdwResSize) {
	// Check the pointers to which we want to write
	if (ppRes && pdwResSize)
	{
		HRSRC hRsrc;
		// Find the resource ResName of type ResType in the DLL described by Instance
		if (hRsrc = FindResource((HMODULE)Instance, MAKEINTRESOURCE(ResID), RT_RCDATA))
		{
			HGLOBAL hGlob;
			// Make sure it's in memory ...
			if (hGlob = LoadResource(Instance, hRsrc))
			{
				// Now lock it to get a pointer
				*ppRes = LockResource(hGlob);
				// Also retrieve the size of the resource
				*pdwResSize = SizeofResource(Instance, hRsrc);
				// Return TRUE only if both succeeded
				return (*ppRes && *pdwResSize);
			}
		}
	}
	// Failure means don't use the values in *ppRes and *pdwResSize
	return false;
}

LPVOID getResource(int resID) {
	LPVOID pResource;
	DWORD pResourceSize;
	if (GetResourcePointer(gl_hThisInstance, resID, &pResource, &pResourceSize))
		return pResource;
	else
		return nullptr;
}
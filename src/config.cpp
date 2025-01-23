#include "config.h"
#include <d3d9types.h>
#include <QB/QB.h>

char* executableDirectory[MAX_PATH];
char configFile[MAX_PATH];
char window_title[MAX_PATH] = "THUG SDL";

float* screenAspectRatio = (float*)0x00707860;
uint8_t* antialiasing = (uint8_t*)(0x007D0E00);
uint8_t* hq_shadows = (uint8_t*)0x007D0E01;
uint8_t* distance_clipping = (uint8_t*)(0x007D0E06);
uint8_t* fog = (uint8_t*)(0x007D0E02);
uint8_t* clipping_distance = (uint8_t*)(0x007D0E0C);		//0x01 - 0x64 val
uint16_t* clipping_distance2 = (uint16_t*)(0x007D0E0C+4);	//0x64 - 0x253 val*5 + 95 maybe
HWND* hwnd = (HWND*)0x007CEC88;								// Referenced at 0x005C3386
SDL_Window* window;
float default_clipping_distance = 96000.0f;
uint32_t* resolution_setting = (uint32_t*)0x007D0E08;
uint8_t* isFocused = (uint8_t*)0x00768FA9;

bool isWindowed;
bool isBorderless;
uint8_t console;
uint8_t language;
uint8_t buttonfont;
uint8_t intromovies;
uint8_t dropdowncontrol;
uint8_t laddergrabcontrol;
uint8_t cavemancontrol;
uint8_t boardscuffs;
uint8_t Ps2Controls;
uint8_t quickgetup;
uint8_t invertRXplayer1;
uint8_t invertRYplayer1;
uint8_t disableRXplayer1;
uint8_t disableRYplayer1;
uint8_t chatsize;
uint8_t consolewaittime;
uint8_t noadditionalscriptmods;
uint8_t savewindowposition;
uint8_t menubuttons;
bool exceptionhandler;
bool disablefsgamma;
bool disableblur;
bool writefile;
bool appendlog;
int windowposx;
int windowposy;
int usemod;

int resX;
int resY;
int defWidth;
int defHeight;
graphicsSettings graphics_settings;

typedef uint32_t dehexifyDigit_NativeCall(char* button);
dehexifyDigit_NativeCall* dehexifyDigit_Native = (dehexifyDigit_NativeCall*)(0x00401E40);

typedef uint32_t __cdecl GlobalGetArrayAsInt_NativeCall(uint32_t nameChecksum);
GlobalGetArrayAsInt_NativeCall* GlobalGetArrayAsInt_Native = (GlobalGetArrayAsInt_NativeCall*)(0x00413650);

/* -=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=- */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- Init =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/* -=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=- */

void InitPatch() {
	patchStaticValues();
	getConfigFilePath(configFile);

	/* Copy CFuncs to our own area which can be searched and expanded */
	CFuncs::CopyStockFunctions();

	/* Make game refer to our function list (List at 0x00686AC0) */
	patchDWord((void*)(0x0052CDE8+1), CFuncs::Pointer_Functions());

	/* Refer to new function for grabbing amount of CFuncs */
	patchCall((void*)0x0052CDE2, (void*)CFuncs::Pointer_FunctionCount());

	console = GetPrivateProfileInt(LOG_SECTION, "Console", 0, configFile);
	writefile = getIniBool(LOG_SECTION, "WriteFile", 0, configFile);
	appendlog = getIniBool(LOG_SECTION, "AppendLog", 0, configFile);
	exceptionhandler = getIniBool(LOG_SECTION, "ExceptionHandler", 0, configFile);
	language = GetPrivateProfileInt(MISC_SECTION, "Language", 1, configFile);
	buttonfont = GetPrivateProfileInt(MISC_SECTION, "ButtonFont", 2, configFile);
	intromovies = getIniBool(MISC_SECTION, "IntroMovies", 0, configFile);
	boardscuffs = getIniBool(MISC_SECTION, "Boardscuffs", 1, configFile);
	disablefsgamma = getIniBool(GRAPHICS_SECTION, "DisableFullscreenGamma", 0, configFile);
	disableblur = getIniBool(GRAPHICS_SECTION, "DisableBlur", 1, configFile);
	graphics_settings.antialiasing = getIniBool(GRAPHICS_SECTION, "AntiAliasing", 0, configFile);
	graphics_settings.hqshadows = GetPrivateProfileInt(GRAPHICS_SECTION, "HQShadows", 0, configFile);
	graphics_settings.distanceclipping = getIniBool(GRAPHICS_SECTION, "DistanceClipping", 0, configFile);
	graphics_settings.clippingdistance = GetPrivateProfileInt("Graphics", "ClippingDistance", 100, configFile);
	graphics_settings.fog = getIniBool(GRAPHICS_SECTION, "Fog", 0, configFile);
	resX = GetPrivateProfileInt(GRAPHICS_SECTION, "ResolutionX", 0, configFile);
	resY = GetPrivateProfileInt(GRAPHICS_SECTION, "ResolutionY", 0, configFile);
	isWindowed = getIniBool(GRAPHICS_SECTION, "Windowed", 1, configFile);
	isBorderless = getIniBool(GRAPHICS_SECTION, "Borderless", 0, configFile);
	Ps2Controls = getIniBool(CONTROLS_SECTION, "Ps2Controls", 1, configFile);
	dropdowncontrol = GetPrivateProfileInt(CONTROLS_SECTION, "DropDownControl", 1, configFile);
	laddergrabcontrol = GetPrivateProfileInt(CONTROLS_SECTION, "LadderGrabKey", 1, configFile);
	cavemancontrol = GetPrivateProfileInt(CONTROLS_SECTION, "CavemanKey", 1, configFile);
	invertRXplayer1 = getIniBool(CONTROLS_SECTION, "InvertRXPlayer1", 0, configFile);
	invertRYplayer1 = getIniBool(CONTROLS_SECTION, "InvertRYPlayer1", 0, configFile);
	disableRXplayer1 = getIniBool(CONTROLS_SECTION, "DisableRXPlayer1", 0, configFile);
	disableRYplayer1 = getIniBool(CONTROLS_SECTION, "DisableRYPlayer1", 0, configFile);
	noadditionalscriptmods = getIniBool(MISC_SECTION, "NoAdditionalScriptMods", 0, configFile);
	savewindowposition = getIniBool(GRAPHICS_SECTION, "SaveWindowPosition", 0, configFile);
	windowposx = GetPrivateProfileInt(GRAPHICS_SECTION, "WindowPosX", SDL_WINDOWPOS_CENTERED, configFile);
	windowposy = GetPrivateProfileInt(GRAPHICS_SECTION, "WindowPosY", SDL_WINDOWPOS_CENTERED, configFile);
	menubuttons = GetPrivateProfileInt(CONTROLS_SECTION, "MenuButtons", 1, configFile);
	chatsize = GetPrivateProfileInt(CHAT_SECTION, "ChatSize", 3, configFile);
	consolewaittime = GetPrivateProfileInt(CHAT_SECTION, "ChatWaitTime", 30, configFile);
	usemod = getIniBool(MOD_SECTION, "UseMod", 0, configFile);

	/* Allocate console */
	if (console) {
		Log::Initialize();
		patchDWord((void*)0x00686AF4, (uint32_t)&Log::CFunc_PrintF);
		if (console == 2) { patchJump((void*)0x004019D0, &Log::PrintLog); }
	}

	/* Register error handler */
	if (exceptionhandler) {
		CFuncs::RedirectFunction("ScriptAssert", (void*)Log::CFunc_ScriptAssert);
		ErrorManager::Initialize();
		ErrorManager::IgnoreVectoredExceptions(true);
	}

	Log::TypedLog(CHN_DLL, "THUG SDL %d.%d\n", VERSION_NUMBER_MAJOR, VERSION_NUMBER_MINOR);
	Log::TypedLog(CHN_DLL, "DIRECTORY: %s\n", (LPSTR)executableDirectory);
	Log::TypedLog(CHN_DLL, "Patch initialized\n");
	Log::TypedLog(CHN_DLL, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<Initializing INI settings\n");
	Log::TypedLog(CHN_DLL, "Intro movies\t\t\t\t\t%s\n", intromovies ? "Enabled" : "Disabled");
	Log::TypedLog(CHN_DLL, "Button font\t\t\t\t\t%s\n", (buttonfont == 2) ? "Ps2" : ((buttonfont == 3) ? "Xbox" : ((buttonfont == 4) ? "NGC" : "PC")));
	Log::TypedLog(CHN_DLL, "Ps2Controls\t\t\t\t\t%s\n", Ps2Controls ? "Enabled" : "Disabled");
	Log::TypedLog(CHN_DLL, "Language setting\t\t\t\t\t%s\n", (language == 1) ? "English" : ((language == 2) ? "French" : ((language == 3) ? "German" : "English")));
	Log::TypedLog(CHN_DLL, "Shadow Quality\t\t\t\t\t%s\n", graphics_settings.hqshadows ? (graphics_settings.hqshadows == 2) ? "Very High" : "High" : "Default");
	Log::TypedLog(CHN_DLL, "Fullscreen Anti-Aliasing\t\t\t\t%s\n", graphics_settings.antialiasing ? "Enabled" : "Disabled");
	Log::TypedLog(CHN_DLL, "Distance Clipping\t\t\t\t\t%s\n", graphics_settings.distanceclipping ? "Enabled" : "Disabled");

	if (graphics_settings.distanceclipping) {
		Log::TypedLog(CHN_DLL, "Clipping Distance\t\t\t\t\t%d\n", graphics_settings.clippingdistance);
		Log::TypedLog(CHN_DLL, "Fog\t\t\t\t\t\t%s\n", graphics_settings.fog ? "Enabled" : "Disabled");
	}
	Log::TypedLog(CHN_DLL, "Resolution from INI\t\t\t\t%d x %d\n", resX, resY);
	Log::TypedLog(CHN_DLL, "Window mode\t\t\t\t\t%s \n", (isWindowed && !isBorderless) ? "Enabled (default)" : ((isWindowed && isBorderless) ? "Enabled (borderless)" : "Disabled"));
	Log::TypedLog(CHN_DLL, "Additional Script Mods\t\t\t\t%s\n", noadditionalscriptmods ? "Disabled" : "Enabled");

	switch (dropdowncontrol) {
		case 1: Log::TypedLog(CHN_DLL, "Drop Down button\t\t\t\t\tPC default: L1+R1, Ps2 default: R2\n"); break;
		case 2: Log::TypedLog(CHN_DLL, "Drop Down button\t\t\t\t\tL1\n"); break;
		case 3: Log::TypedLog(CHN_DLL, "Drop Down button\t\t\t\t\tR1\n"); break;
		case 4: Log::TypedLog(CHN_DLL, "Drop Down button\t\t\t\t\tL2\n"); break;
		case 5: Log::TypedLog(CHN_DLL, "Drop Down button\t\t\t\t\tR2\n"); break;
	}
	switch (laddergrabcontrol) {
		case 1: Log::TypedLog(CHN_DLL, "Ladder grab button\t\t\t\tDefault: R1\n"); break;
		case 2: Log::TypedLog(CHN_DLL, "Ladder grab button\t\t\t\tL1\n"); break;
	}
	switch (cavemancontrol) {
		case 1: Log::TypedLog(CHN_DLL, "Caveman button\t\t\t\t\tDefault: PC default: black or white / Ps2 default: L1+R1\n"); break;
		case 2: Log::TypedLog(CHN_DLL, "Caveman button\t\t\t\t\tL1\n"); break;
		case 3: Log::TypedLog(CHN_DLL, "Caveman button\t\t\t\t\tR1\n"); break;
		case 4: Log::TypedLog(CHN_DLL, "Caveman button\t\t\t\t\tL2\n"); break;
		case 5: Log::TypedLog(CHN_DLL, "Caveman button\t\t\t\t\tR2\n"); break;
		case 6: Log::TypedLog(CHN_DLL, "Caveman button\t\t\t\t\tL1+R1\n"); break;
		case 7: Log::TypedLog(CHN_DLL, "Caveman button\t\t\t\t\tL2+R2\n"); break;
	}
	Log::TypedLog(CHN_DLL, "Invert player 1 camera X axis (right stick)\t%s\n", invertRXplayer1 ? "Enabled" : "Disabled");
	Log::TypedLog(CHN_DLL, "Invert player 1 camera Y axis (right stick)\t%s\n", invertRYplayer1 ? "Enabled" : "Disabled");
	Log::TypedLog(CHN_DLL, "Player 1 camera X axis (right stick)\t\t%s\n", disableRXplayer1 ? "Disabled" : "Enabled");
	Log::TypedLog(CHN_DLL, "Player 1 camera Y axis (right stick)\t\t%s\n", disableRYplayer1 ? "Disabled" : "Enabled");
	Log::TypedLog(CHN_DLL, "Chat size\t\t\t\t\t\t%s\n", (chatsize == 2) ? "Small" : ((chatsize == 3) ? "Default" : ((chatsize == 4) ? "Big" : "Extra small")));
	if (consolewaittime < 1 || consolewaittime > 120) consolewaittime = 30;
	Log::TypedLog(CHN_DLL, "Chat message time\t\t\t\t\t%d seconds\n", consolewaittime);
	Log::TypedLog(CHN_DLL, "MenuButtons\t\t\t\t\t%s\n", (menubuttons == 2) ? "Ps2" : "PC (default)");
	if (savewindowposition)
		Log::TypedLog(CHN_DLL, "Found saved window position\t\t\t%d x %d\n", windowposx, windowposy);
	else
		Log::TypedLog(CHN_DLL, "Window position\t\t\t\t\tCentered\n");
	Log::TypedLog(CHN_DLL, "Mod loader\t\t\t\t\t%s\n", usemod ? "Enabled" : "Disabled");
	Log::TypedLog(CHN_DLL, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<Finished initializing INI settings\n");

	/*Language setting*/
	patchNop((void*)0x0057CCAC, 5);	//Don't get the value from registry

	if (language == 1)
		patchByte((void*)0x007CA180, 2); //English
	else if (language == 2)
		patchByte((void*)0x007CA180, 3); //French
	else if (language == 3)
		patchByte((void*)0x007CA180, 5); //German
	else
		patchByte((void*)0x007CA180, 1);

	patchByte((void*)(0x007CA180 + 0x8), 0x06);	//Load and save savegames across different language settings
	patchByte((void*)(0x007CA180 + 0xC), 0x01);

	/*Intro movies*/
	if (!intromovies)
		patchBytesM((void*)0x004222C0, (BYTE*)"\x83\xF8\x01\x75\x01\xC3\xE9\x85\x05\x00\x00", 11);

	/*Button font*/
	patch_button_font(buttonfont);

	/*Patch button lookup for Ps2 menu prompts (triangle = back). This goes along with redirecting CFunc::SetButtonEventMappings*/
	if (menubuttons == 2) {
		patchByte((void*)0x005AD537, 0x03);
		patchByte((void*)0x005AD539, 0x01);
	}

	/*Ledge warp fix*/
	patchCall((void*)0x00569081, (void*)&fix_floating_precision);

	/*Disable gamma*/
	if (disablefsgamma)
		patchByte((void*)0x004AF860, 0xC3);

	if (disableblur)
		patchBytesM((void*)0x00477AF0, (BYTE*)"\xB0\x01\xC3\x90\x90", 5);

	/*Connection fix*/
	patchCall((void*)0x004E056A, (void*)&runProfileConnectScript);
	patchByte((void*)0x004CF330, 0xEB);
	patchByte((void*)(0x004C2DD9 + 1), 0x00);
	patchByte((void*)0x005C86FC, 0x6F);
	patchByte((void*)0x005C8703, 0x70);
	patchByte((void*)0x005C870A, 0x65);
	patchByte((void*)0x005C8711, 0x6E);
	patchBytesM((void*)0x007D1520, (BYTE*)"\x6F\x70\x65\x6E\x73\x70\x79\x00", 8);
}

void patchStaticValues() {

	patchByte((void*)0x004C2A84, 0xEB);
	patchByte((void*)0x004C2B88, 0x75);
	patchDWord((void*)(0x005CB714 + 1), 0x00001388);
	patchNop((void*)0x005C5AF3, 6);
	patchNop((void*)0x005C5B35, 6);
	patchNop((void*)0x005C5B7A, 6);
	patchNop((void*)0x005C5BBF, 6);
	patchBytesM((void*)(0x005C5ADC + 1), (BYTE*)"\xC7\x90\x90\x90\x90", 5);
	patchBytesM((void*)(0x005C5B21 + 1), (BYTE*)"\xD7\x90\x90\x90\x90", 5);
	patchBytesM((void*)(0x005C5B66 + 1), (BYTE*)"\xCF\x90\x90\x90\x90", 5);
	patchBytesM((void*)(0x005C5BAB + 1), (BYTE*)"\xC7\x90\x90\x90\x90", 5);
	patchBytesM((void*)(0x004E042F + 1), (BYTE*)"\xF4\x01", 2);
	patchBytesM((void*)0x005C3AFF, (BYTE*)"\x83\xF8\x13", 3);
	patchByte((void*)(0x004E0446 + 6), 0x06);

	/* Increase startup speed */
	patchDWord((void*)(0x005C8517 + 2), 0x000001F4);
	patchBytesM((void*)(0x004B78D0 + 1), (BYTE*)"\x1C\x02", 2);
	patchBytesM((void*)(0x004B78FA + 4), (BYTE*)"\xC0\x03", 2);
	patchBytesM((void*)(0x004F8060 + 1), (BYTE*)"\xFF\x01", 2);

	srand(time(0));
	patchCall((void*)0x00424BEB, &Rnd_fixed);
	patchCall((void*)0x00424BF9, &Rnd_fixed);
	patchCall((void*)0x00424C3F, &Rnd_fixed);

	/*No CAS integrity check*/
	patchNop((void*)0x0052F76A, 6);

	/*Increase main memory region*/
	patchBytesM((void*)(0x0057C45F + 3), (BYTE*)"\x00\x10", 2);
	patchBytesM((void*)(0x0057C47A + 3), (BYTE*)"\x00\x10", 2);

	/* Expand default clipping distance to avoid issues in large level backgrounds (I.E. hawaii), thanks PARTYMANX */
	patchDWord((void*)(0x004B9D8B + 2), (uint32_t)&default_clipping_distance);
}

void enforceMaxResolution() {

	DEVMODE devMode; 
	devMode.dmSize = sizeof(DEVMODE); 
	if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode)) {
		defWidth = devMode.dmPelsWidth;
		defHeight = devMode.dmPelsHeight;
	}
	
	uint8_t isValidX = 0;
	uint8_t isValidY = 0;
	DEVMODE deviceMode = {};
	int i = 0;

	if (!isWindowed) {
		while (EnumDisplaySettings(NULL, i, &deviceMode)) {
			if (deviceMode.dmPelsWidth == resX) {
				isValidX = 1;
			}
			if (deviceMode.dmPelsHeight == resY) {
				isValidY = 1;
			}
			i++;
		}
		if (!isValidX || !isValidY) {
			resX = 0;
			resY = 0;
		}
	}
	else if (resX > defWidth || resY > defHeight) {
		resX = 0;
		resY = 0;
	}
}

void handleWindowEvent(SDL_Event* e) {

	switch (e->type) {
	case SDL_WINDOWEVENT:
		if (e->window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
			*(uint8_t*)0x0072DE00 = 0; // recreate_device

			//Stop music playback
			patchByte((void*)0x00422B78, 0x74);
			patchByte((void*)0x00422B90, 0x74);
			patchByte((void*)0x00425807, 0x75);
		}
		else if (e->window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
			*(uint8_t*)0x0072DE00 = 1;
			*isFocused = 1;

			//Resume music playback
			patchByte((void*)0x00422B78, 0x75);
			patchByte((void*)0x00422B90, 0x75);
			patchByte((void*)0x00425807, 0x74);
		}
		return;
	default:
		return;
	}
}

void createSDLWindow() {

	registerEventHandler(handleWindowEvent);

	SDL_Init(SDL_INIT_VIDEO);

	uint32_t flags = isWindowed ? (SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE) : SDL_WINDOW_FULLSCREEN;

	if (isBorderless && isWindowed) {
		flags |= SDL_WINDOW_BORDERLESS;
	}

	if (isWindowed) {
		// switch a couple of variables being set for better register selection
		patchDWord((void*)(0x004B78BA + 2), 0x0072DDD8);
		patchDWord((void*)(0x004B795C + 2), 0x0072DDE8);
		patchByte((void*)(0x004B795C + 1), 0x1D);	// set fullscreen to 0
	}

	enforceMaxResolution();

	*resolution_setting = 0;

	if (resX == 0 || resY == 0) {
		resX = defWidth;
		resY = defHeight;
		Log::TypedLog(CHN_DLL, "Found invalid resolution. Falling back to: %d x %d\n", defWidth, defHeight);
	}
	else if (resX < 640 || resY < 480) {
		resX = 640;
		resY = 480;
		Log::TypedLog(CHN_DLL, "Found invalid resolution. Falling back to: %d x %d\n", resX, resY);
	}
	else {
		Log::TypedLog(CHN_DLL, "Setting resolution: %d x %d\n", resX, resY);
	}

	Log::TypedLog(CHN_DLL, "Aspect ratio: %f\n", getaspectratio());

	bool windows_created = false;
	if (!isWindowed) isBorderless = 0;

	if (usemod)
		sprintf_s(window_title, "%s%s%s%s", window_title, " (", getWindowTitle(), ")");

	if (isWindowed || isBorderless) {
		BOOL dpi_result = SetProcessDPIAware();
		if (savewindowposition) {
			window = SDL_CreateWindow(window_title, windowposx ? windowposx : SDL_WINDOWPOS_CENTERED, windowposy ? windowposy : SDL_WINDOWPOS_CENTERED, resX, resY, flags);
			windows_created = true;
		}
		SDL_SetWindowResizable(window, SDL_TRUE);
	}

	if (!windows_created)
		window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, resX, resY, flags);

	// make borderless window draggable
	if (isBorderless) {
		SDL_Renderer* Renderer = SDL_CreateRenderer(window, -1, 0);
		SDL_SetWindowHitTest(window, HitTestCallback, 0);
	}

	if (!window) {
		Log::TypedLog(CHN_SDL, "Failed to create window! Error: %s\n", SDL_GetError());
	}

	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(window, &wmInfo);
	*hwnd = wmInfo.info.win.window;

	*isFocused = 1;

	// patch resolution setting
	patchDWord((void*)(0x004B790A + 4), resX);
	patchDWord((void*)(0x004B78E9 + 4), resY);

	// set aspect ratio and FOV
	patchJump((void*)0x00485050, setAspectRatio);
	patchJump((void*)0x00485090, getScreenAngleFactor);

	if (isWindowed)
		SDL_ShowCursor(1);
	else
		SDL_ShowCursor(0);
}

void getWindowDimensions(uint32_t* width, uint32_t* height) {
	if (width) {
		*width = resX;
	}

	if (height) {
		*height = resY;
	}
}

void writeConfigValues() {
	*antialiasing = graphics_settings.antialiasing;
	if (graphics_settings.hqshadows) {
		*hq_shadows = graphics_settings.hqshadows;

		if (graphics_settings.hqshadows == 2) {
			patchByte((void*)(0x004860C0 + 2), 0x04);
			patchByte((void*)(0x004860C5 + 2), 0x04);
		}
	}
	
	*distance_clipping = graphics_settings.distanceclipping;
	*fog = graphics_settings.fog;

	uint32_t distance = graphics_settings.clippingdistance;
	uint32_t distance2 = 1;

	if (distance > 100)
		distance = 100;
	else if (distance < 2) {
		distance = 1;
		distance2 = 0;
	}
	*clipping_distance = distance;
	*clipping_distance2 = distance * 5.0 + 95.0;

	Log::TypedLog(CHN_DLL, "Setting launcher settings\n");
}

void patchWindow() {

	/* replace the window with an SDL2 window. this kind of straddles the line between input and config */
	patchJump((void*)0x005C3240, &createSDLWindow);

	/* don't move window to corner */
	patchNop((void*)0x004B78CE, 2);
	patchNop((void*)0x004B78F5, 1);
	patchNop((void*)0x004B7916, 5);
	patchNop((void*)0x004B7926, 6);

	/* don't load launcher settings from registry, use our own INI values instead */
	patchCall((void*)0x005C569F, writeConfigValues);
}

uint32_t dehexifyDigit_Wrapper(char* p_button) {
	uint8_t value = *p_button;
	uint8_t original_value = *p_button;
	uint8_t base = *(p_button - 0x30);
	uint8_t* map = (uint8_t*)0x005AD516;

	if ((value > 0x4C) && (value < 0x75)) {
		value = *(uint8_t*)(map + value - 0x4D);

		switch (value) {
		case 0:
			value = 0x33;
			break;
		case 1:
			value = 0x32;
			break;
		case 2:
			value = 0x31;
			break;
		case 3:
			value = 0x30;
			break;
		case 4:
			value = 0x65;
			break;
		case 5:
			value = 0x66;
			break;
		case 6:
			value = 0x67;
			break;
		case 7:
			value = 0x68;
			break;
		default:
			value = original_value;
			break;
		}
	}
	if (value < 0x3A) {
		return (value - 0x30);
	}
	else if (value < 0x57) {
		return (value - 0x37);
	}
	else if (value < 0x77) {
		return (value - 0x57);
	}
	else { return 0; }

	/* return dehexifyDigit_Native(button); */
	/* Original implementation */
	/*
	uint8_t value = *p_button;

	if ((value > 0x30) && (value < 0x39)) {
		return (value - 0x30);
	}
	else if ((value > 0x41) && (value < 0x56)) {
		return (value - 0x37);
	}
	else if ((value > 0x61) && (value < 0x76)) {
		return (value - 0x57);
	}
	else {
		return 0;
	}
	*/

	/*
	0x00 = Triangle
	0x01 = Square
	0x02 = Circle
	0x03 = X
	0x04 = Arrow Down
	0x05 = Arrow Right
	0x06 = Arrow Left
	0x07 = Arrow Up
	0x08 = Select
	0x09 = Start
	0x0A = Arrow Down Right
	0x0B = Arrow Up Left
	0x0C = Arrow Down Left
	0x0D = Arrow Up Right
	0x0E = [L1]
	0x0F = [R1]
	0x10 = [L2]
	0x11 = [R2]
	0x12 = +
	0x13 = Y/C
	0x14 = S/X
	0x15 = <empty>
	0x16 = ENT
	0x17 = ESC
	0x18 = E
	0x19 = R
	0x1A = ß
	0x1B = `
	0x1C = 1
	0x1D = 2
	Rest is empty
	*/
}

uint32_t patchMetaButtonMap() {
	if (buttonfont == 2)
		return GlobalGetArrayAsInt_Native(0x6030A16D); /* meta_button_map_ps2 */
	else if (buttonfont == 3)
		return GlobalGetArrayAsInt_Native(0xBAF816FB); /* meta_button_map_xbox */
	else if (buttonfont == 4)
		return GlobalGetArrayAsInt_Native(0xEE6CDAC5); /* meta_button_map_gamecube */
}

void patch_button_font(uint8_t sel) {
	/* 1 = PC (default), 3 = Xbox. Both have the same text: Xbox.buttons_font */

	if (sel == 2)
		patchDWord((void*)0x0063100B, 0x00327350); // Ps2..buttons_font
	else if (sel == 4)
		patchDWord((void*)0x0063100B, 0x0063674E); // Ngc..buttons_font

	if (1 < sel && sel < 5) {
		patchCall((void*)0x00478B8D, patchMetaButtonMap);
		patchCall((void*)0x004AF394, dehexifyDigit_Wrapper);

		patchNop((void*)0x004AF3C6, 3);
		patchBytesM((void*)0x004AF3C9, (BYTE*)"\x80\xF9\x12", 3);

		/* code cave that holds at least 40 bytes */
		patchBytesM((void*)0x005AD516, (BYTE*)"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x08\x08\x08\x08\x08\x08\x08\x08\x08\x08\x08\x08\x08\x08\x08\x08\x08\x08\x08\x08\x08\x08\x08\x00\x01\x02\x03\x04\x05\x06\x07", 40);	
	}
}

void loadSettings(struct modsettings* settingsOut) {
	if (settingsOut) {
		settingsOut->isPs2Controls = Ps2Controls;
		settingsOut->invertRXplayer1 = invertRXplayer1;
		settingsOut->invertRYplayer1 = invertRYplayer1;
		settingsOut->disableRXplayer1 = disableRXplayer1;
		settingsOut->disableRYplayer1 = disableRYplayer1;
		settingsOut->buttonfont = buttonfont;
		settingsOut->chatsize = chatsize;
		settingsOut->boardscuffs = boardscuffs;
		settingsOut->dropdowncontrol = dropdowncontrol;
		settingsOut->quickgetup = quickgetup;
		settingsOut->dropdowncontrol = dropdowncontrol;
		settingsOut->cavemancontrol = cavemancontrol;
		settingsOut->laddergrabcontrol = laddergrabcontrol;
		settingsOut->noadditionalscriptmods = noadditionalscriptmods;
		settingsOut->savewindowposition = savewindowposition;
		settingsOut->windowposx = windowposx;
		settingsOut->windowposy = windowposy;
		settingsOut->menubuttons = menubuttons;
		settingsOut->consolewaittime = consolewaittime;
	}
}

void loadLogSettings(struct logsettings* settingsOut) {
	settingsOut->writefile = writefile;
	settingsOut->appendlog = appendlog;
}

/* called from initMod */
void loadModSettings(struct extmodsettings* extmodsettingsOut) {
	extmodsettingsOut->usemod = usemod;
	extmodsettingsOut->configfile = configFile;
	extmodsettingsOut->workingdir = (char*)executableDirectory;

}

void loadKeyBinds(struct keybinds* bindsOut) {
	char configFile[MAX_PATH];
	getConfigFilePath(configFile);

	if (bindsOut) {
		bindsOut->cameraToggle = (SDL_Scancode)GetPrivateProfileInt(KEYBIND_SECTION, "ViewToggle", SDL_SCANCODE_TAB, configFile);
		bindsOut->cameraSwivelLock = (SDL_Scancode)GetPrivateProfileInt(KEYBIND_SECTION, "SwivelLock", SDL_SCANCODE_GRAVE, configFile);

		bindsOut->grind = (SDL_Scancode)GetPrivateProfileInt(KEYBIND_SECTION, "Grind", SDL_SCANCODE_KP_8, configFile);
		bindsOut->grab = (SDL_Scancode)GetPrivateProfileInt(KEYBIND_SECTION, "Grab", SDL_SCANCODE_KP_6, configFile);
		bindsOut->ollie = (SDL_Scancode)GetPrivateProfileInt(KEYBIND_SECTION, "Ollie", SDL_SCANCODE_KP_2, configFile);
		bindsOut->kick = (SDL_Scancode)GetPrivateProfileInt(KEYBIND_SECTION, "Flip", SDL_SCANCODE_KP_4, configFile);

		bindsOut->leftSpin = (SDL_Scancode)GetPrivateProfileInt(KEYBIND_SECTION, "SpinLeft", SDL_SCANCODE_KP_7, configFile);
		bindsOut->rightSpin = (SDL_Scancode)GetPrivateProfileInt(KEYBIND_SECTION, "SpinRight", SDL_SCANCODE_KP_9, configFile);
		bindsOut->nollie = (SDL_Scancode)GetPrivateProfileInt(KEYBIND_SECTION, "Nollie", SDL_SCANCODE_KP_1, configFile);
		bindsOut->switchRevert = (SDL_Scancode)GetPrivateProfileInt(KEYBIND_SECTION, "Switch", SDL_SCANCODE_KP_3, configFile);
		bindsOut->caveman = (SDL_Scancode)GetPrivateProfileInt(KEYBIND_SECTION, "Caveman", SDL_SCANCODE_KP_1, configFile);
		bindsOut->caveman2 = (SDL_Scancode)GetPrivateProfileInt(KEYBIND_SECTION, "Caveman2", SDL_SCANCODE_KP_3, configFile);

		bindsOut->right = (SDL_Scancode)GetPrivateProfileInt(KEYBIND_SECTION, "Right", SDL_SCANCODE_D, configFile);
		bindsOut->left = (SDL_Scancode)GetPrivateProfileInt(KEYBIND_SECTION, "Left", SDL_SCANCODE_A, configFile);
		bindsOut->up = (SDL_Scancode)GetPrivateProfileInt(KEYBIND_SECTION, "Forward", SDL_SCANCODE_W, configFile);
		bindsOut->down = (SDL_Scancode)GetPrivateProfileInt(KEYBIND_SECTION, "Backward", SDL_SCANCODE_S, configFile);

		bindsOut->cameraRight = (SDL_Scancode)GetPrivateProfileInt(KEYBIND_SECTION, "CameraRight", SDL_SCANCODE_V, configFile);
		bindsOut->cameraLeft = (SDL_Scancode)GetPrivateProfileInt(KEYBIND_SECTION, "CameraLeft", SDL_SCANCODE_C, configFile);
		bindsOut->cameraUp = (SDL_Scancode)GetPrivateProfileInt(KEYBIND_SECTION, "CameraUp", SDL_SCANCODE_Z, configFile);
		bindsOut->cameraDown = (SDL_Scancode)GetPrivateProfileInt(KEYBIND_SECTION, "CameraDown", SDL_SCANCODE_X, configFile);

		bindsOut->item_up = (SDL_Scancode)GetPrivateProfileInt(KEYBIND_SECTION, "ItemUp", SDL_SCANCODE_HOME, configFile);
		bindsOut->item_down = (SDL_Scancode)GetPrivateProfileInt(KEYBIND_SECTION, "ItemDown", SDL_SCANCODE_END, configFile);
		bindsOut->item_left = (SDL_Scancode)GetPrivateProfileInt(KEYBIND_SECTION, "ItemLeft", SDL_SCANCODE_DELETE, configFile);
		bindsOut->item_right = (SDL_Scancode)GetPrivateProfileInt(KEYBIND_SECTION, "ItemRight", SDL_SCANCODE_PAGEDOWN, configFile);
	}
}

void loadControllerBinds(struct controllerbinds* bindsOut) {
	char configFile[MAX_PATH];
	getConfigFilePath(configFile);

	if (bindsOut) {
		bindsOut->menu = (controllerButton)GetPrivateProfileInt(CONTROLLER_SECTION, "Pause", CONTROLLER_BUTTON_START, configFile);
		bindsOut->cameraToggle = (controllerButton)GetPrivateProfileInt(CONTROLLER_SECTION, "ViewToggle", CONTROLLER_BUTTON_BACK, configFile);
		bindsOut->cameraSwivelLock = (controllerButton)GetPrivateProfileInt(CONTROLLER_SECTION, "SwivelLock", CONTROLLER_BUTTON_RIGHTSTICK, configFile);
		
		bindsOut->grind = (controllerButton)GetPrivateProfileInt(CONTROLLER_SECTION, "Grind", CONTROLLER_BUTTON_Y, configFile);
		bindsOut->grab = (controllerButton)GetPrivateProfileInt(CONTROLLER_SECTION, "Grab", CONTROLLER_BUTTON_B, configFile);
		bindsOut->ollie = (controllerButton)GetPrivateProfileInt(CONTROLLER_SECTION, "Ollie", CONTROLLER_BUTTON_A, configFile);
		bindsOut->kick = (controllerButton)GetPrivateProfileInt(CONTROLLER_SECTION, "Flip", CONTROLLER_BUTTON_X, configFile);

		bindsOut->leftSpin = (controllerButton)GetPrivateProfileInt(CONTROLLER_SECTION, "SpinLeft", CONTROLLER_BUTTON_LEFTSHOULDER, configFile);
		bindsOut->rightSpin = (controllerButton)GetPrivateProfileInt(CONTROLLER_SECTION, "SpinRight", CONTROLLER_BUTTON_RIGHTSHOULDER, configFile);
		bindsOut->nollie = (controllerButton)GetPrivateProfileInt(CONTROLLER_SECTION, "Nollie", CONTROLLER_BUTTON_LEFTTRIGGER, configFile);
		bindsOut->switchRevert = (controllerButton)GetPrivateProfileInt(CONTROLLER_SECTION, "Switch", CONTROLLER_BUTTON_RIGHTTRIGGER, configFile);
		bindsOut->caveman = (controllerButton)GetPrivateProfileInt(CONTROLLER_SECTION, "Caveman", CONTROLLER_BUTTON_LEFTTRIGGER, configFile); //white
		bindsOut->caveman2 = (controllerButton)GetPrivateProfileInt(CONTROLLER_SECTION, "Caveman2", CONTROLLER_BUTTON_RIGHTTRIGGER, configFile); //black

		bindsOut->right = (controllerButton)GetPrivateProfileInt(CONTROLLER_SECTION, "Right", CONTROLLER_BUTTON_DPAD_RIGHT, configFile);
		bindsOut->left = (controllerButton)GetPrivateProfileInt(CONTROLLER_SECTION, "Left", CONTROLLER_BUTTON_DPAD_LEFT, configFile);
		bindsOut->up = (controllerButton)GetPrivateProfileInt(CONTROLLER_SECTION, "Forward", CONTROLLER_BUTTON_DPAD_UP, configFile);
		bindsOut->down = (controllerButton)GetPrivateProfileInt(CONTROLLER_SECTION, "Backward", CONTROLLER_BUTTON_DPAD_DOWN, configFile);

		bindsOut->movement = (controllerStick)GetPrivateProfileInt(CONTROLLER_SECTION, "MovementStick", CONTROLLER_STICK_LEFT, configFile);
		bindsOut->camera = (controllerStick)GetPrivateProfileInt(CONTROLLER_SECTION, "CameraStick", CONTROLLER_STICK_RIGHT, configFile);
	}
}


/* -=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=- */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= Helpers -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
/* -=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=- */

void getConfigFilePath(char mConfigFile[MAX_PATH]) {
	GetModuleFileName(NULL, (LPSTR)&executableDirectory, MAX_PATH);

	// find last slash
	char* exe = strrchr((LPSTR)executableDirectory, '\\');
	if (exe) {
		*(exe + 1) = '\0';
	}
	sprintf(mConfigFile, "%s%s", executableDirectory, CONFIG_FILE_NAME);
}

int getIniBool(const char* section, const char* key, int def, char* file) {
	int result = GetPrivateProfileInt(section, key, def, file);
	if (result) {
		return 1;
	}
	else {
		return 0;
	}
}

float getaspectratio() {
	return ((float)resX / (float)resY);
}

SDL_Window* getWindowHandle() {
	return window;
}

SDL_HitTestResult HitTestCallback(SDL_Window* Window, const SDL_Point* Area, void* Data) {
	return SDL_HITTEST_DRAGGABLE;
}

void dumpWindowPosition() {
	SDL_GetWindowPosition(getWindowHandle(), &windowposx, &windowposy);
	char str_x[10]; sprintf(str_x, "%d", windowposx);
	char str_y[10]; sprintf(str_y, "%d", windowposy);
	WritePrivateProfileString(GRAPHICS_SECTION, "WindowPosX", str_x, configFile);
	WritePrivateProfileString(GRAPHICS_SECTION, "WindowPosY", str_y, configFile);
}

float fix_floating_precision(float val)
{
	val = (val > 1.0f) ? 1.0f : (val < -1.0f ? -1.0f : val);
	double result = std::cos(static_cast<double>(val));
	return static_cast<float>(result);
}

void runProfileConnectScript(void* arg1, Script::LazyStruct* pParams) {
	RunScript(0x588ED87C, pParams, nullptr, nullptr);
}

void __cdecl setAspectRatio(float aspect) {
	*screenAspectRatio = (float)resX / (float)resY;
}

float __cdecl getScreenAngleFactor() {
	return ((float)resX / (float)resY) / (4.0f / 3.0f);
}

int Rnd_fixed(int n) {
	return (rand() % n);
}
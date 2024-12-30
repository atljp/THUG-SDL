#include <Logger/Logger.h>

bool l_ExitOnAssert = true;
bool l_UseConsole, l_DebugOutput;
FILE* CON, * f_tracer, * f_logger;
HANDLE consoleHandle;
struct logsettings mLogSettings;

namespace Log {

	bool ConsoleAllowed() { return l_UseConsole; }
	bool OutputAllowed() { return mLogSettings.writefile; }
	bool AppendAllowed() { return mLogSettings.appendlog; }

	//------------------------
	// Prepare the logger
	//------------------------

	void Initialize() {
		char logpath[MAX_PATH];
		char tracepath[MAX_PATH];
		loadLogSettings(&mLogSettings);

		//if (GameConfig::GetValue("Logger", "Console", 0))
			l_UseConsole = true;
		//if (GameConfig::GetValue("Logger", "WriteFile", 1))
			l_DebugOutput = false;

		//l_ExitOnAssert = GameConfig::GetValue("Logger", "ExitOnAssert", 1);

		// Create console?
		if (l_UseConsole)
		{
			AllocConsole();
			SetConsoleTitle("THUG CONSOLE WINDOW");
			freopen_s(&CON, "CONIN$", "r", stdin);
			freopen_s(&CON, "CONOUT$", "w", stdout);
			freopen_s(&CON, "CONOUT$", "w", stderr);
			std::cout.clear();
			std::cerr.clear();

			consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		}

		// Write to debug file?
		if (OutputAllowed())
		{
			//Log("Logger file output initialized.\n");
			if (AppendAllowed())
				fopen_s(&f_logger, "debug.txt", "a");
			else
				fopen_s(&f_logger, "debug.txt", "w");
			//fopen_s(&f_tracer, "trace.txt", "w");

			if (!f_logger)
				PrintLog("Failed to create debug.txt file.\n");
			//if (!f_tracer)
			//	PrintLog("Failed to create trace.txt file.\n");
		}
	}

	//------------------------
	// Prints message to console and log!
	//------------------------

	void CoreLog(const char* to_log, const char* category = CHN_LOG) {
		// Output to console
		if (ConsoleAllowed())
			printf("[%s] %s", category, to_log);

		// Also write to .txt file!
		if (OutputAllowed() && f_logger)
		{
			fputs(to_log, f_logger);
			fflush(f_logger);
		}
	}

	//------------------------
	// Prints log message, with arguments!
	//------------------------

	void Log(const char* Format, ...)
	{
		char final_buffer[2000];

		va_list args;
		va_start(args, Format);
		vsnprintf(final_buffer, 2000, Format, args);
		va_end(args);

		CoreLog(final_buffer, CHN_LOG);
	}

	//------------------------
	// Prints log message, with arguments!
	//------------------------

	void PrintLog(const char* Format, ...) {
		char final_buffer[2000];

		va_list args;
		va_start(args, Format);
		vsnprintf(final_buffer, 2000, Format, args);
		va_end(args);

		CoreLog(final_buffer, CHN_LOG);
	}

	//------------------------
	// Typed log
	//------------------------

	void TypedLog(const char* category, const char* Format, ...) {
		char final_buffer[3000];

		va_list args;
		va_start(args, Format);
		vsnprintf(final_buffer, 3000, Format, args);
		va_end(args);

		CoreLog(final_buffer, category);
	}

	//------------------------
	// Print a nasty error!
	//------------------------

	void Error(const char* Format, ...)
	{
		//THAWPlus::DebugScriptCallstack();

		char final_buffer[2000];

		va_list args;
		va_start(args, Format);
		vsnprintf(final_buffer, 2000, Format, args);
		va_end(args);

		char finalString[2048];
		sprintf(finalString, "CRITICAL: %s", final_buffer);
		Log("%s\n\n", finalString);

		MessageBox(NULL, final_buffer, "Critical Error", MB_ICONERROR);

		ExitProcess(0);
	}

	//------------------------
	// Format string from pParams (built-in function)
	//------------------------

	typedef bool StringFromParamCall(char* print_dest, Script::LazyStruct* pParams);
	StringFromParamCall* s_from_params = (StringFromParamCall*)(0x00525700); //0x0044C840 = THUG2 , wrong? 0x00496D10

	void StringFromParams(char* print_dest, Script::LazyStruct* pParams)
	{
		s_from_params(print_dest, pParams);
	}

	//------------------------
	// Function to replace plain PrintF!
	//------------------------

	bool CFunc_PrintF(Script::LazyStruct* pParams, void* pScript)
	{
		char buf[3000];
		StringFromParams(buf, pParams);

		TypedLog(CHN_LOG, "%s", buf);

		return 1;
	}

	//------------------------
	// Function to replace plain ScriptAssert!
	//
	// REMEMBER: This should be FATAL and only used
	// when the game should show an error message.
	//------------------------

	bool CFunc_ScriptAssert(Script::LazyStruct* pParams)
	{
		char buf[2048];
		StringFromParams(buf, pParams);

		TypedLog(CHN_LOG, "%s", buf);

		if (l_ExitOnAssert)
		{
			Error(buf);
			return true;
		}

		return 1;
	}

}
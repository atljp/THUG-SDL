#include "modloader.h"

struct extmodsettings mExtModsettings;
char modname[MAX_PATH];
char modfolder[MAX_PATH];
char modfolder_fullpath[MAX_PATH];
char modini_fullpath[MAX_PATH];
char to_be_injected[MAX_PATH];

std::map<std::string, std::string> preFilesMap;
std::map<std::string, std::string> qbFilesMap;
std::map<std::string, std::string> levelPreFilesMap;
std::map<std::string, std::string> levelPreFilesMap_post;

/* -=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=- */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- Init -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
/* -=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=- */

void patchPreWrappers() {
	patchCall((void*)0x0052CC7D, PIPLoadPre_Wrapper2);
	patchCall((void*)0x0057E6EA, PIPLoadPre_Wrapper2);
	patchCall((void*)0x00526D86, PIPLoadPre_Wrapper);
	patchCall((void*)0x0052CD74, PIPLoadPre_Wrapper);
}
void InitModloader() {
	// Get info to determine if the mod loader is active
	// Get handles to thugsdl.ini, game dir and window title
	loadModSettings(&mExtModsettings);

	// Only load mods if it's activated in the ini
	if (mExtModsettings.usemod) {

		if (!(mExtModsettings.noadditionalscriptmods))
			Log::TypedLog(CHN_MOD, "WARNING! SCRIPT MODS FROM THUG-SDL ARE ENABLED. COMPATIBILITY IS NOT GUARANTEED!\n");

		// Check if modfolder and mod.ini are valid
		// This will return a handle to the specified mod.ini and the mod folder.
		// The mod name will also be passed to the window title bar
		if (getModIni()) {

			// Get all defined pre files and store them in a map of form qb.pre=mycustomfile.pre
			if (getAllPreFiles()) {
				Log::TypedLog(CHN_MOD, "Patching PIP::LoadPre and PreMgr::LoadPre\n");
				patchPreWrappers();
			}
			else {
				Log::TypedLog(CHN_MOD, "Failed to load Pre files\n");
			}
			if (getAllQbFiles()) {
				Log::TypedLog(CHN_MOD, "Patching PIP::Load\n");
				patchCall((void*)0x0040A049, pipLoadWrapper);
			}
			else {
				Log::TypedLog(CHN_MOD, "Failed to load Qb files\n");
			}
		}
	}
	else if (!(mExtModsettings.noadditionalscriptmods)) {
		// Load custom qb.pre by default. 
		// This contains most of the data that was previously loaded in script.cpp as well as the observe menu, in game menu, physics etc.
		if (getModDefaultPreFile()) {
			Log::TypedLog(CHN_DLL, "Patching PIP::LoadPre and PreMgr::LoadPre for default mod scripts\n");
			patchPreWrappers();
		}
		else {
			Log::TypedLog(CHN_MOD, "Failed to load default mod Pre file\n");
		}
	}
}


/* -=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=- */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=- Function definitions -=-=-=-=-=-=-=-=-=-=-=-=-=-= */
/* -=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=- */

typedef void __cdecl PIPLoadPre_NativeCall(uint8_t* p_data);
PIPLoadPre_NativeCall* PIPLoadPre_Native = (PIPLoadPre_NativeCall*)(0x0057DD90);

typedef void __cdecl PIPLoadPre_NativeCall2(uint8_t* p_data);
PIPLoadPre_NativeCall2* PIPLoadPre_Native2 = (PIPLoadPre_NativeCall2*)(0x0057E200);

typedef void(__fastcall* PreMgrLoadPre_NativeCall)(int ecx, int edx, uint8_t* p_data, void* arg1, void* arg2, bool arg3);
PreMgrLoadPre_NativeCall PreMgrLoadPre = (PreMgrLoadPre_NativeCall)(0x0057F7F0);

typedef void(__thiscall* PreMgrLoadPre_NativeCall2)(void* arg1, uint8_t* arg2, char* arg3);
PreMgrLoadPre_NativeCall2 PreMgrLoadPre2 = (PreMgrLoadPre_NativeCall2)(0x0057FDD0);

typedef uint8_t* __cdecl pipLoad_NativeCall(const char* p_fileName);
pipLoad_NativeCall* pipLoad_Native = (pipLoad_NativeCall*)(0x0057E840);

typedef uint8_t* __cdecl loadQb_maybe_NativeCall(const char* p_fileName, uint8_t* p_data);
loadQb_maybe_NativeCall* loadQb_maybe_Native = (loadQb_maybe_NativeCall*)(0x0040D410);


/* -=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=- */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=- Mod loader functions -=-=-=-=-=-=-=-=-=-=-=-=-=-= */
/* -=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=- */

std::map<std::string, std::string> extractSection(const std::string& section, bool isPreFile) {

	std::ifstream iniFile(modini_fullpath);
	std::string line;
	bool inSection = false;
	char preFile_fullpath[MAX_PATH];
	std::map<std::string, std::string> keyValues;

	if (!iniFile.is_open()) {
		Log::TypedLog(CHN_MOD, "Error: Could not open the .ini file!\n"); 
		return keyValues;
	}
	else {
		Log::TypedLog(CHN_MOD, "Successfully loaded %s\n", modini_fullpath);
	}

	// Go through mod.ini and extract all "key, value" pairs for a given section. Trim whitespaces and append file endings if needed
	while (std::getline(iniFile, line)) {

		// Ignore lines that are comments or empty
		if (line.empty() || line[0] == ';' || line[0] == '#') {
			continue;
		}

		// Trim whitespace from the beginning and end of the line
		line.erase(0, line.find_first_not_of(" \t"));
		line.erase(line.find_last_not_of(" \t") + 1);

		// Check if the line is a section header
		if (line.front() == '[' && line.back() == ']') {
			std::string currentSection = line.substr(1, line.size() - 2);
			inSection = (currentSection == section);
		}
		else if (inSection) {
			// Check if the line is a key-value pair
			std::size_t pos = line.find('=');

			if (pos != std::string::npos) {
				std::string key = line.substr(0, pos);
				std::string value = line.substr(pos + 1);

				// Trim whitespace from the key and value
				key.erase(0, key.find_first_not_of(" \t"));
				key.erase(key.find_last_not_of(" \t") + 1);
				value.erase(0, value.find_first_not_of(" \t"));
				value.erase(value.find_last_not_of(" \t") + 1);

				// Check for file endings
				if (isPreFile) {
					if (key.find(".pre") == std::string::npos) { key += ".pre"; }
					if (value.find(".pre") == std::string::npos) { value += ".pre"; }
				}
				else {
					if (key.find(".qb") == std::string::npos) { key += ".qb"; }
					if (value.find(".qb") == std::string::npos) { value += ".qb"; }
				}

				// Fill the map which will be iterated in the hooked functions
				// Don't add double entries
				if (!isKeyInMap(keyValues, key)) {
					
					// Generate injection string, this will be passed to LoadPre
					sprintf_s(to_be_injected, "%s%s%s", strrchr(modfolder, '\\') + 1, "\\", value.c_str());

					// Check if file exists on hard disk
					sprintf_s(preFile_fullpath, "%s%s%s", mExtModsettings.workingdir, "data\\pre\\", to_be_injected);

					if (checkFileExists(preFile_fullpath)) {
						keyValues[key] = to_be_injected;
					}
					else {
						Log::TypedLog(CHN_MOD, "File not found: %s\n", to_be_injected);
					}
				}
			}
		}
	}
	return keyValues;
}

bool getModIni() {

	// Check if modfolder was specified in partymod.ini. Folders have to be relative to the game directory (data\pre\mymod)
	GetPrivateProfileString(MOD_SECTION, "Folder", "", modfolder, sizeof(modfolder), mExtModsettings.configfile);

	if (strlen(modfolder)){

		// Maybe replace forward slashes with backslashes
		for (int i = 0; i < strlen(modfolder); ++i) {
			if (modfolder[i] == '/') {
				modfolder[i] = '\\';
			}
		}

		// Get full path to modfolder
		sprintf_s(modfolder_fullpath, "%s%s", mExtModsettings.workingdir, modfolder);
		Log::TypedLog(CHN_MOD, "Trying to load files from specified mod folder: %s\n", modfolder);

		if (checkFolderExists(modfolder_fullpath)) {
			Log::TypedLog(CHN_MOD, "Found mod folder: %s\n", modfolder_fullpath);
		}
		else {
			Log::TypedLog(CHN_MOD, "ERROR: Mod folder doesn\'t exist!\n", modfolder_fullpath); return FALSE;
		}

		// Get full path of mod.ini file
		sprintf(modini_fullpath, "%s%s", modfolder_fullpath, "\\mod.ini");

		// Check if mod.ini file exists on hard drive
		if (checkFileExists(modini_fullpath)) {
			Log::TypedLog(CHN_MOD, "Found mod.ini\n");
		}
		else {
			Log::TypedLog(CHN_MOD, "ERROR: Mod folder doesn\'t contain a mod.ini\n");
			return false;
		}

		// If ini was found: Get mod name
		GetPrivateProfileString("MODINFO", "Name", "UNDEFINED", modname, sizeof(modname), modini_fullpath);
		Log::TypedLog(CHN_MOD, "Attempting to load mod: %s\n", modname);
		return true;
	}
	else {
		Log::TypedLog(CHN_MOD, "ERROR: No mod folder specified!\n"); return false;
	}
}

bool getModDefaultPreFile() {
	
	char preFile_fullpath[MAX_PATH];
	sprintf_s(preFile_fullpath, "%s%s", mExtModsettings.workingdir, "thugsdl.pre");

	if (checkFileExists(preFile_fullpath)) {
		preFilesMap["qb.pre"] = "..\\..\\thugsdl.pre";
		printf("Found file %s\n", preFile_fullpath);
		return true;
	}
	printf("thugsdl.pre not found in game directory. Falling back to data\\pre\\qb.pre\n");
	return false;
}

bool getAllPreFiles() {

	preFilesMap = extractSection("PRE", true);

	if (preFilesMap.empty())
		return false;

	for (const auto& kv : preFilesMap) {
		Log::TypedLog(CHN_MOD, "REGISTERING PRE FILE: %s\n", kv.second.c_str());
	}
	return true;
}

bool getAllQbFiles() {

	qbFilesMap = extractSection("QB", false);

	if (qbFilesMap.empty())
		return false;

	for (const auto& kv : qbFilesMap) {
		Log::TypedLog(CHN_MOD, "REGISTERING QB FILE: %s=%s\n", kv.first.c_str(), kv.second.c_str());
	}
	return true;
}

uint8_t* getQbData(const std::string& fileName) {

	char qbfile_fullpath[MAX_PATH];

	sprintf_s(qbfile_fullpath, "%s%s%s", mExtModsettings.workingdir, "data\\pre\\", fileName.c_str());
	std::ifstream infile(qbfile_fullpath, std::ios::binary | std::ios::ate);

	if (!infile) {
		Log::TypedLog(CHN_MOD, "Could not open file: %s\n", qbfile_fullpath);
		return nullptr;
	}

	size_t fileSize = infile.tellg();
	infile.seekg(0, std::ios::beg);

	std::vector<uint8_t> buffer(fileSize);
	if (!infile.read(reinterpret_cast<char*>(buffer.data()), buffer.size())) {
		Log::TypedLog(CHN_MOD, "Error reading file: %s\n", qbfile_fullpath);
		return nullptr;
	}

	// Allocate memory for the data pointer and copy the contents
	uint8_t* dataPtr = new uint8_t[fileSize];
	std::memcpy(dataPtr, buffer.data(), fileSize);

	return (uint8_t*)dataPtr;
}

int strncasecmp(const char* s1, const char* s2, size_t n) {
	for (size_t i = 0; i < n; ++i) {
		char c1 = std::tolower(static_cast<unsigned char>(s1[i]));
		char c2 = std::tolower(static_cast<unsigned char>(s2[i]));

		if (c1 != c2) {
			return c1 - c2;
		}
		if (c1 == '\0') {
			return 0;
		}
	}
	return 0;
}

bool checkFolderExists(char* folder) {
	// Check if specified mod folder exists on hard drive
	struct stat info;
	stat(folder, &info);
	if (info.st_mode & S_IFDIR)
		return true;
	else {
		return false;
	}
}

bool checkFileExists(char* file) {
	// Check if specified file exists on hard drive
	std::ifstream infile(file);

	if (infile.good()) {
		return true;
	}
	else {
		return false;
	}
}

bool isKeyInMap(const std::map<std::string, std::string>& keyValues, const std::string& key) {
	for (const auto& kv : keyValues) {
		if (kv.first == key) {
			return true;
		}
	}
	return false;
}

char* getWindowTitle() {
	return modname;
}


/* -=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=- */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=- Hooked functions =--=-=-=-=-=-=-=-=-=-=-=-=-=-= */
/* -=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=- */

void PIPLoadPre_Wrapper(uint8_t* p_data) {

	//Log::TypedLog(CHN_MOD, "PIPLoadPre_Wrapper: %s\n", (const char*)p_data);

	for (const auto& pair : preFilesMap) {
		//Log::TypedLog(CHN_MOD, "COMPARING: %s with %s\n", (const char*)p_data, pair.first.c_str());
		if (strncasecmp((const char*)p_data, pair.first.c_str(), strlen((const char*)p_data)-5) == 0) // Compare the original data pointer with our files (without the ending and without the first letter to account for capital first letters)
		{
			Log::TypedLog(CHN_MOD, "Successfully replaced %s with %s\n", (const char*)p_data, pair.second.c_str());
			p_data = (uint8_t*)pair.second.c_str();
		}
	}
	PIPLoadPre_Native(p_data);
}

void PIPLoadPre_Wrapper2(uint8_t* p_data) {

	//Log::TypedLog(CHN_MOD, "PIPLoadPre_Wrapper2: %s\n", (const char*)p_data);
	
	for (const auto& pair : preFilesMap) {
		//Log::TypedLog(CHN_MOD, "PIPLoadPre_Wrapper2 COMPARING: %s with %s\n", (const char*)p_data, pair.first.c_str());
		if (strncasecmp((const char*)p_data, pair.first.c_str(), strlen((const char*)p_data) - 5) == 0) // Compare the original data pointer with our files (without the ending and without the first letter to account for capital first letters)
		{
			Log::TypedLog(CHN_MOD, "Successfully replaced %s with %s\n", (const char*)p_data, pair.second.c_str());
			p_data = (uint8_t*)pair.second.c_str();
		}
	}
	PIPLoadPre_Native2(p_data);
}

void __fastcall PreMgrLoadPre_Wrapper(int ecx, int edx, uint8_t* p_data, void* arg1, void* arg2, bool arg3) {

	//Log::TypedLog(CHN_MOD, "PreMgrLoadPre_Wrapper: %s\n", (const char*)p_data);

	for (const auto& pair : preFilesMap)
	{
		//Log::TypedLog(CHN_MOD, "PreMgrLoadPre_Wrapper COMPARING: %s with %s\n", (const char*)p_data, pair.first.c_str());
		if (strncasecmp((const char*)p_data, pair.first.c_str(), strlen((const char*)p_data) - 5) == 0) // Compare the original data pointer with our files (without the ending and without the first letter to account for capital first letters)
		{
			Log::TypedLog(CHN_MOD, "Successfully replaced %s with %s\n", (const char*)p_data, pair.second.c_str());
			p_data = (uint8_t*)pair.second.c_str();
		}
	}
	PreMgrLoadPre(ecx, edx, p_data, arg1, arg2, arg3);
}

void __fastcall PreMgrLoadPre_Wrapper2(void* arg1, void* unused, uint8_t* p_data, char* arg3) {
	
	//Log::TypedLog(CHN_MOD, "PreMgrLoadPre_Wrapper2: %s\n", (const char*)p_data);

	for (const auto& pair : preFilesMap)
	{
		//Log::TypedLog(CHN_MOD, "PreMgrLoadPre_Wrapper COMPARING: %s with %s\n", (const char*)p_data, pair.first.c_str());
		if (strncasecmp((const char*)p_data, pair.first.c_str(), strlen((const char*)p_data) - 5) == 0) // Compare the original data pointer with our files (without the ending and without the first letter to account for capital first letters)
		{
			Log::TypedLog(CHN_MOD, "Successfully replaced %s with %s\n", (const char*)p_data, pair.second.c_str());
			p_data = (uint8_t*)pair.second.c_str();
		}
	}
	PreMgrLoadPre2(arg1, p_data, arg3);
}

uint8_t* __cdecl pipLoadWrapper(char* p_fileName, int unk1, int unk2, int unk3, int unk4) {

	for (const auto& kv : qbFilesMap) {

		if (!strcmp(p_fileName, kv.first.c_str())) {
			Log::TypedLog(CHN_MOD, "Loading %s -> %s\n", kv.first.c_str(), kv.second.c_str());

			// Get a data pointer from the file on disk
			return getQbData(kv.second);
		}
	}
	return pipLoad_Native(p_fileName);
}
#include "modloader.h"
#include <map>


struct extmodsettings mExtModsettings;
struct stat info;
char modname[MAX_PATH];
char modfolder[MAX_PATH];
char full_modfolder[MAX_PATH];
char ini_file[MAX_PATH];
bool filemissing = FALSE;
char to_be_injected[60];

LPVOID pResource_template;

typedef void __cdecl PIPLoadPre_NativeCall(uint8_t* p_data);
PIPLoadPre_NativeCall* PIPLoadPre_Native = (PIPLoadPre_NativeCall*)(0x0057DD90);//0x0057E200);

typedef void __cdecl PIPLoadPre_NativeCall2(uint8_t* p_data);
PIPLoadPre_NativeCall2* PIPLoadPre_Native2 = (PIPLoadPre_NativeCall2*)(0x0057E200);

typedef void(__thiscall* PreMgrLoadPre_NativeCall)(void* arg1, uint8_t* arg2, char* arg3, char* arg4, char arg5);
PreMgrLoadPre_NativeCall PreMgrLoadPre = (PreMgrLoadPre_NativeCall)(0x0057F7F0);

typedef void(__thiscall* PreMgrLoadPre_NativeCall2)(void* arg1, uint8_t* arg2, char* arg3);
PreMgrLoadPre_NativeCall2 PreMgrLoadPre2 = (PreMgrLoadPre_NativeCall2)(0x0057FDD0);

typedef uint8_t* __cdecl pipLoad_NativeCall(char* p_fileName);
pipLoad_NativeCall* pipLoad_Native = (pipLoad_NativeCall*)(0x0057E840);

//typedef void ParseQB_NativeCall(const char* p_fileName, uint8_t* p_qb, int ecx, int assertIfDuplicateSymbols, bool allocateChecksumNameLookupTable);
//ParseQB_NativeCall* ParseQB_Native = (ParseQB_NativeCall*)(0x00472420); //thug2

//##############################
std::unordered_set<std::string> all_pre_files() {
	return {
		"anims",
		"bits",
		"casfiles",
		"cas_bedroom",
		"cas_bedroomcol",
		"cas_bedroomcol_net",
		"cas_bedroomped",
		"cas_bedroomscn",
		"cas_bedroomscn_net",
		"chapters",
		"dj",
		"djcol",
		"djcol_net",
		"djped",
		"djscn",
		"djscn_net",
		"fl",
		"flcol",
		"flcol_net",
		"flped",
		"flscn",
		"flscn_net",
		"fonts",
		"hi",
		"hicol",
		"hicol_net",
		"hiped",
		"hiscn",
		"hiscn_net",
		"hn",
		"hncol",
		"hncol_net",
		"hnped",
		"hnscn",
		"hnscn_net",
		"levelselect",
		"mainmenu",
		"mainmenucol",
		"mainmenucol_net",
		"mainmenuped",
		"mainmenuscn",
		"mainmenuscn_net",
		"netanims",
		"nj",
		"njcol",
		"njcol_net",
		"njped",
		"njscn",
		"njscn_net",
		"nj_skateshop",
		"nj_skateshopcol",
		"nj_skateshopcol_net",
		"nj_skateshopped",
		"nj_skateshopscn",
		"nj_skateshopscn_net",
		"ny",
		"nycol",
		"nycol_net",
		"nyped",
		"nyscn",
		"nyscn_net",
		"panelsprites",
		"parked_sounds",
		"permtex",
		"ph",
		"phcol",
		"phcol_net",
		"phped",
		"phscn",
		"phscn_net",
		"qb",
		"ru",
		"rucol",
		"rucol_net",
		"ruped",
		"ruscn",
		"ruscn_net",
		"sc",
		"sc2",
		"sc2col",
		"sc2col_net",
		"sc2ped",
		"sc2scn",
		"sc2scn_net",
		"sccol",
		"sccol_net",
		"scped",
		"scscn",
		"scscn_net",
		"sd",
		"sdcol",
		"sdcol_net",
		"sdped",
		"sdscn",
		"sdscn_net",
		"se",
		"secol",
		"secol_net",
		"seped",
		"sescn",
		"sescn_net",
		"sj",
		"sjcol",
		"sjcol_net",
		"sjped",
		"sjscn",
		"sjscn_net",
		"sk5ed",
		"sk5ed2scn",
		"sk5ed2_shell",
		"sk5ed2_shellcol",
		"sk5ed2_shellcol_net",
		"sk5ed2_shellped",
		"sk5ed2_shellscn",
		"sk5ed2_shellscn_net",
		"sk5ed3scn",
		"sk5ed3_shell",
		"sk5ed3_shellcol",
		"sk5ed3_shellcol_net",
		"sk5ed3_shellped",
		"sk5ed3_shellscn",
		"sk5ed3_shellscn_net",
		"sk5ed4scn",
		"sk5ed4_shell",
		"sk5ed4_shellcol",
		"sk5ed4_shellcol_net",
		"sk5ed4_shellped",
		"sk5ed4_shellscn",
		"sk5ed4_shellscn_net",
		"sk5ed5scn",
		"sk5ed5_shell",
		"sk5ed5_shellcol",
		"sk5ed5_shellcol_net",
		"sk5ed5_shellped",
		"sk5ed5_shellscn",
		"sk5ed5_shellscn_net",
		"sk5edcol",
		"sk5edcol_net",
		"sk5edped",
		"sk5edscn",
		"sk5edscn_net",
		"sk5ed_shell",
		"sk5ed_shellcol",
		"sk5ed_shellcol_net",
		"sk5ed_shellped",
		"sk5ed_shellscn",
		"sk5ed_shellscn_net",
		"skaterparts",
		"skater_sounds",
		"skateshop",
		"skateshopcol",
		"skateshopcol_net",
		"skateshopped",
		"skateshopscn",
		"skateshopscn_net",
		"skeletons",
		"sp",
		"spcol",
		"spcol_net",
		"spped",
		"spscn",
		"spscn_net",
		"testlevel",
		"testlevelcol",
		"testlevelcol_net",
		"testlevelped",
		"testlevelscn",
		"testlevelscn_net",
		"themes",
		"unloadableanims",
		"vc",
		"vccol",
		"vccol_net",
		"vcped",
		"vcscn",
		"vcscn_net",
		"vn",
		"vncol",
		"vncol_net",
		"vnped",
		"vnscn",
		"vnscn_net",
	};
}

std::vector<std::string> mmoddedFileNames_original;
std::vector<std::string> mmoddedFileNames_modded;
std::map<std::string, std::string> fileNameMapping;

int strncasecmp(const char* s1, const char* s2, size_t n) { for (size_t i = 0; i < n; ++i) { char c1 = std::tolower(static_cast<unsigned char>(s1[i])); char c2 = std::tolower(static_cast<unsigned char>(s2[i])); if (c1 != c2) { return c1 - c2; } if (c1 == '\0') { return 0; } } return 0; }

void generateModdedFileVector(std::vector<std::string>& mmoddedFileNames_original)
{
	std::unordered_set<std::string> validFileNames = all_pre_files();
	std::ifstream iniFile(ini_file);

	if (!iniFile.is_open()) {
		Log::TypedLog(CHN_MOD, "Error: Could not open the .ini file!\n"); return;
	}
	else { Log::TypedLog(CHN_MOD, "Successfully loaded mod.ini\n"); }

	std::string line;
	while (std::getline(iniFile, line)) {

		// Ignore lines that are comments or empty
		if (line.empty() || line[0] == ';' || line[0] == '#') {
			continue;
		}

		// Find the position of the '=' which separates key and value
		size_t pos = line.find('=');
		if (pos == std::string::npos) {
			continue; // Malformed line, skip it
		}

		// Extract the key
		std::string key = line.substr(0, pos);
		key.erase(0, key.find_first_not_of(" \t")); // Trim leading spaces
		key.erase(key.find_last_not_of(" \t") + 1); // Trim trailing spaces

		// Maybe remove file ending
		size_t dotPosition = key.find('.');
		if (dotPosition != std::string::npos) {
			// Remove everything from the dot to the end of the string
			key.erase(dotPosition);
		}

		// Check if the key is in the valid file names set
		if (validFileNames.find(key) != validFileNames.end()) {
			Log::TypedLog(CHN_MOD, "Valid key found: %s\n", key.c_str());
			mmoddedFileNames_original.push_back(key);
		}
		else {
			if (key != "Name") {
				Log::TypedLog(CHN_MOD, "Invalid key found: %s\n", key.c_str());
				filemissing = TRUE;
			}
		}
	}
	iniFile.close();
}

bool getModIni()
{
	// Check if modfolder was specified in partymod.ini. Folders have to be relative to the game directory (data\pre\mymod)
	GetPrivateProfileString(MOD_SECTION, "Folder", "", modfolder, sizeof(modfolder), mExtModsettings.configfile);
	if (strlen(modfolder))
	{
		// Maybe replace forward slashes with backslashes
		for (int i = 0; i < strlen(modfolder); ++i) {
			if (modfolder[i] == '/') {
				modfolder[i] = '\\';
			}
		}

		// Get full path to modfolder
		sprintf_s(full_modfolder, "%s%s", mExtModsettings.workingdir, modfolder);
		Log::TypedLog(CHN_MOD, "Trying to load files from specified mod folder: %s\n", modfolder);

		// Check if specified mod folder exists on hard drive
		stat(full_modfolder, &info);
		if (info.st_mode & S_IFDIR)
			Log::TypedLog(CHN_MOD, "Found mod folder: %s\n", full_modfolder);
		else {
			Log::TypedLog(CHN_MOD, "ERROR: Mod folder doesn\'t exist!\n", full_modfolder); return FALSE;
		}

		// Get full path of mod.ini file
		sprintf(ini_file, "%s%s", full_modfolder, "\\mod.ini");

		// Check if mod.ini file exists on hard drive
		std::ifstream infile_ini(ini_file);
		if (infile_ini.good())
			Log::TypedLog(CHN_MOD, "Found mod.ini!\n");
		else {
			Log::TypedLog(CHN_MOD, "ERROR: Mod folder doesn\'t contain a mod.ini\n"); return FALSE;
		}

		GetPrivateProfileString("MODINFO", "Name", "UNDEFINED", modname, sizeof(modname), ini_file);
		Log::TypedLog(CHN_MOD, "Attempting to load mod: %s\n", modname);
		return TRUE;
	}
	else {
		Log::TypedLog(CHN_MOD, "ERROR: No mod folder specified!\n"); return FALSE;
	}
}

void getModdedFileName(char* filename)
{
	char a[30];
	char b[MAX_PATH];
	char filename_pre[30];

	sprintf_s(filename_pre, "%s%s", filename, ".pre");

	// get info from mod.ini: get the new qb_scripts.pre filename
	GetPrivateProfileString("MODINFO", filename_pre, "UNDEFINED", a, sizeof(a), ini_file); 


	if (strcmp((const char*)a, "UNDEFINED") && strlen(a)) {
		//printf("FILENAME: %s -- AAA: %s\n", filename_pre, a);
		sprintf_s(b, "%s%s%s", full_modfolder, "\\", a); // check if file exists on hard drive
		//printf("CHECK: %s\n", b);
		std::ifstream infile(b);
		if (infile.good()) {
			Log::TypedLog(CHN_MOD, "Found modded file: %s\n", a);
			sprintf_s(to_be_injected, "%s%s%s", strrchr(modfolder, '\\') + 1, "/", a); // generate injection string, this will be passed to LoadPre
			//printf("TO BE INJECTED: %s\n", to_be_injected);
			mmoddedFileNames_modded.push_back(to_be_injected);
		}
		else {
			Log::TypedLog(CHN_MOD, "ERROR: Couldn\'t find modded file on hard drive: %s\n", a);
			filemissing = TRUE;
		}

	}
	else {
		// If files can't be found or were specified incorrectly, no PRE loading functions will be patched
		Log::TypedLog(CHN_MOD, "ERROR: Invalid file specified for: %s\n", filename_pre);
		filemissing = TRUE;
	}
}

//char* myString = "thugturbo\\thugturbo_nj.pre";
//uint8_t* p_data2 = reinterpret_cast<uint8_t*>(myString);
void PIPLoadPre_Wrapper(uint8_t* p_data) {

	for (const auto& pair : fileNameMapping) {
		//Log::TypedLog(CHN_MOD, "COMPARING: %s with %s\n", (const char*)p_data, pair.first.c_str());
		if (strncasecmp((const char*)p_data, pair.first.c_str(), strlen((const char*)p_data)-5) == 0) // Compare the original data pointer with our files (without the ending and without the first letter to account for capital first letters)
		{
			Log::TypedLog(CHN_MOD, "PIPLoadPre_Wrapper - Successfully replaced %s with %s\n", (const char*)p_data, pair.second.c_str());
			p_data = (uint8_t*)pair.second.c_str();
		}
	}
	PIPLoadPre_Native(p_data);

}

void PIPLoadPre_Wrapper2(uint8_t* p_data) {

	for (const auto& pair : fileNameMapping) {
		//Log::TypedLog(CHN_MOD, "COMPARING: %s with %s\n", (const char*)p_data, pair.first.c_str());
		if (strncasecmp((const char*)p_data, pair.first.c_str(), strlen((const char*)p_data) - 5) == 0) // Compare the original data pointer with our files (without the ending and without the first letter to account for capital first letters)
		{
			Log::TypedLog(CHN_MOD, "PIPLoadPre_Wrapper 2 - Successfully replaced %s with %s\n", (const char*)p_data, pair.second.c_str());
			p_data = (uint8_t*)pair.second.c_str();
		}
	}
	PIPLoadPre_Native2(p_data);

}




void __fastcall PreMgrLoadPre_Wrapper(void* arg1, void* unused, uint8_t* p_data, char* arg3, char* arg4, char arg5) {

	//Log::TypedLog(CHN_MOD, "PreMgrLoadPre_Wrapper: %s\n", (const char*)p_data);
	for (const auto& pair : fileNameMapping)
	{
		//Log::TypedLog(CHN_MOD, "PreMgrLoadPre_Wrapper COMPARING: %s with %s\n", (const char*)p_data, pair.first.c_str());
		if (strncasecmp((const char*)p_data, pair.first.c_str(), strlen((const char*)p_data) - 5) == 0) // Compare the original data pointer with our files (without the ending and without the first letter to account for capital first letters)
		{
			Log::TypedLog(CHN_MOD, "PreMgrLoadPre_Wrapper - Successfully replaced %s with %s\n", (const char*)p_data, pair.second.c_str());
			p_data = (uint8_t*)pair.second.c_str();
		}
	}
	PreMgrLoadPre(arg1, p_data, arg3, arg4, arg5);
	/*
	Log::TypedLog(CHN_MOD, "PreMgrLoadPre_Wrapper1: %s\n", (const char*)p_data);
	if (strncasecmp((const char*)p_data, "nj.pre", strlen((const char*)p_data)) == 0) {
		printf("wrapper 1, FOUND NJ\n");
		PreMgrLoadPre(arg1, p_data2, arg3, arg4, arg5);
	}
	else {
		PreMgrLoadPre(arg1, p_data, arg3, arg4, arg5);
	}
	*/
}

void __fastcall PreMgrLoadPre_Wrapper2(void* arg1, void* unused, uint8_t* p_data, char* arg3) {
	
	
	for (const auto& pair : fileNameMapping)
	{
		//Log::TypedLog(CHN_MOD, "PreMgrLoadPre_Wrapper COMPARING: %s with %s\n", (const char*)p_data, pair.first.c_str());
		if (strncasecmp((const char*)p_data, pair.first.c_str(), strlen((const char*)p_data) - 5) == 0) // Compare the original data pointer with our files (without the ending and without the first letter to account for capital first letters)
		{
			Log::TypedLog(CHN_MOD, "PreMgrLoadPre_Wrapper2 - Successfully replaced %s with %s\n", (const char*)p_data, pair.second.c_str());
			p_data = (uint8_t*)pair.second.c_str();
		}
	}
	PreMgrLoadPre2(arg1, p_data, arg3);
	/*
	Log::TypedLog(CHN_MOD, "PreMgrLoadPre_Wrapper2: %s\n", (const char*)p_data);
	if (strncasecmp((const char*)p_data, "nj.pre", strlen((const char*)p_data)) == 0) {
		printf("FOUND NJ\n");
		PreMgrLoadPre2(arg1, p_data2, arg3);
	}
	else {
		PreMgrLoadPre2(arg1, p_data, arg3);
	}
	*/
}

void getWindowTitle(struct extmodsettings* extmodsettingsOut)
{
	extmodsettingsOut->windowtitle = modname;
}

void InitMod()
{
	// Get info to determine if the mod loader is active. Also get handles to partymod.ini, the game directory and the window title
	// The info is stored in the mModsettings struct
	loadModSettings(&mExtModsettings);

	// Only load mods if it's activated in the ini
	if (mExtModsettings.usemod) {

		// Check if modfolder and mod.ini are valid
		// When successful, we have a handle to the specified mod.ini and the mod folder. The mod name will also be passed to the window title bar
		if (getModIni()) {
			
			// Check the contents of mod.ini and verify valid file names (checked against all_pre_files vector)
			generateModdedFileVector(mmoddedFileNames_original);

			// Go through mod.ini and get all modded file names and put it into mmoddedFileNames_modded
			for (const auto& entry : mmoddedFileNames_original)
				getModdedFileName((char*)(entry.c_str()));
			
			if (!filemissing)
			{
				// Ensure the vectors have the same length
				if (mmoddedFileNames_original.size() != mmoddedFileNames_modded.size()) {
					Log::TypedLog(CHN_MOD, "Vectors are not of the same length!\n"); return;
				}

				// Create the mapping using std::map
				for (size_t i = 0; i < mmoddedFileNames_original.size(); ++i) {
					fileNameMapping[mmoddedFileNames_original[i]] = mmoddedFileNames_modded[i];
				}

				// Patch PRE loading functions when everything was initialized correctly
				//anims
				//unloadableanims
				//mainmenucol
				patchCall((void*)0x0052CC7D, PIPLoadPre_Wrapper2);
				patchCall((void*)0x0057E6EA, PIPLoadPre_Wrapper2);

				patchCall((void*)0x00526D86, PIPLoadPre_Wrapper);
				patchCall((void*)0x0052CD74, PIPLoadPre_Wrapper);
				patchCall((void*)0x0057E087, PIPLoadPre_Wrapper);

				patchCall((void*)0x0057FD37, PreMgrLoadPre_Wrapper);
				patchCall((void*)0x00526DA2, PreMgrLoadPre_Wrapper2);
				patchCall((void*)0x0057FEA4, PreMgrLoadPre_Wrapper2);
				Log::TypedLog(CHN_MOD, "Patching PIP::LoadPre and PreMgr::LoadPre\n");

				// For qb files. Get resource handle in Resources.rc and Resources.h
				//if (pResource_template = getResource(IDR_MANUALTRICKS))
				//	patchCall((void*)0x0040A049, pipLoadWrapper);
			}
		}	
	}

	/*
	for (auto& pair : fileNameMapping) {
		printf("std::map contents: %s -> %s\n", pair.first.c_str(), pair.second.c_str());
	}
	printf("LENN: %d\n", fileNameMapping.size());

	for (const auto& entry : mmoddedFileNames_original) {
		std::cout << "ORIGINAL: " << entry << std::endl;
	}
	for (const auto& entry : mmoddedFileNames_modded) {
		std::cout << "HERE: " << entry << std::endl;
	}
	for (const auto& entry : mmoddedFileNames_original) {
		std::cout << "ORIGINAL: " << entry << std::endl;
	}
	for (const auto& entry : mmoddedFileNames_modded) {
		std::cout << "MODDED: " << entry << std::endl;
	}
	*/
}

uint8_t* __cdecl pipLoadWrapper(char* p_fileName, int unk1, int unk2, int unk3, int unk4) {

	if (!strcmp(p_fileName, "scripts\\game\\skater\\manualtricks.qb"))
		return (uint8_t*)pResource_template;

	return pipLoad_Native(p_fileName);;
}

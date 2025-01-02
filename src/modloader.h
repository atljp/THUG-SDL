#pragma once
#include "config.h"
#include <unordered_set>
#include <string>
#include <map>

void InitMod();
std::map<std::string, std::string> extractSection(const std::string& section, bool isPreFile);
bool getModIni();
bool getAllPreFiles();
bool getAllQbFiles();
uint8_t* getQbData(const std::string& fileName);
int strncasecmp(const char* s1, const char* s2, size_t n);
bool checkFolderExists(char* folder);
bool checkFileExists(char* file);
bool isKeyInMap(const std::map<std::string, std::string>& keyValues, const std::string& key);
char* getWindowTitle();
void PIPLoadPre_Wrapper(uint8_t* p_data);
void PIPLoadPre_Wrapper2(uint8_t* p_data);
void __fastcall PreMgrLoadPre_Wrapper(int ecx, int edx, uint8_t* p_data, void* arg1, void* arg2, bool arg3);
void __fastcall PreMgrLoadPre_Wrapper2(void* arg1, void* unused, uint8_t* p_data, char* arg3);
uint8_t* __cdecl pipLoadWrapper(char* p_fileName, int unk1, int unk2, int unk3, int unk4);

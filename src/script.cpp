#include "script.h"

/*
ADDING SCRIPTS FROM FILES

1) Compile the .q to .qb and remove the first bytes that define the script name. The trailing 0x01 is not always there. Usually the script name bytes look like this 0x23 0x16 0xEF 0xBE 0xAD 0xDE 0x01
2) If the compiler generated a checksum table, this can be removed as it only increases the file size. 0x2B marks the start of the checksum entries, remove from there (including 0x2B)
3) Place the .qb file inside the src\Resources directory
4) In resources.rc, add: 
	IDR_YOURSCRIPTNAME RCDATA "yourscript.qb"
5) In resources.h, add with an increasing number: 
	#define		IDR_YOURSCRIPTNAME	101
6) Load the file like this with an arbitrary path name:

	LPVOID pResource_yourscript;

	if (pResource_yourscript = getResource(IDR_YOURSCRIPTNAME)) {
		removeScript(0xC0FEBABE); // Checksum of the original script you're replacing
		contentsChecksum = CalculateScriptContentsChecksum_Native((uint8_t*)pResource_yourscript);
		sCreateScriptSymbol_Wrapper(5896, 0xF0425254, contentsChecksum, (uint8_t*)pResource_yourscript, "path\\to\\file.qb");
	}
*/

struct modsettings mSettings;
bool walkspinpatched = false;
bool boardscuffpatched = false;
uint32_t addr_sCreateScriptSymbol = 0x0040AF40;
uint32_t addr_sCreateSymbolOfTheFormNameEqualsValue = 0x0040D1D0;
LPVOID pResource_oslogo;
LPVOID pResource_keyboard_restored;

struct CGoalManagerInstance
{
	char unk[76];
	uint32_t GetLevelStructureName;
};

struct SkateInstance
{
	/* singleton of Skate::Instance() */
	char unk[920];
	uint32_t m_cur_level;
	uint32_t m_prev_level;
	bool m_new_record;
	char unk2[11];
	CGoalManagerInstance GetGoalManager;
};

uint8_t console_wait_time[] = { //0x074DBDBB
	/*console_wait_time = 30*/
	0x16, 0xBB, 0xBD, 0x4D, 0x07, 0x07, 0x17, 0x1E, 0x00, 0x00, 0x00, 0x01
};


/* -=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=- */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- Init -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
/* -=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=- */

void patchScripts() {
	loadSettings(&mSettings); /*get config from INI. struct defined in config.h*/
	patchCall((void*)0x0052CDDD, &patchCFuncs);
	patchCall((void*)0x004BFD5C, &initScriptPatch);
}

void patchCFuncs() {
	CFuncs::RedirectFunction("CreateScreenElement", CreateScreenElement_Patched);
	CFuncs::RedirectFunction("IsPS2", IsPS2_Patched);
	CFuncs::RedirectFunction("GetMemCardSpaceAvailable", GetMemCardSpaceAvailable_Patched);
	if (!mSettings.noadditionalscriptmods) {
		CFuncs::RedirectFunction("SetScreenElementProps", SetScreenElementProps_Patched);
		if (pResource_oslogo = getResource(IDR_OSLOGO)) CFuncs::RedirectFunction("IsXBOX", IsXBOX_Patched);
	}
	if (mSettings.menubuttons == 2)
		/*Actually change the button actions when ps2 style menu navigation is selected*/
		CFuncs::RedirectFunction("SetButtonEventMappings", SetButtonEventMappings_Patched);

	Log::TypedLog(CHN_DLL, "Initializing CFuncs\n");
}

void initScriptPatch() {
	if (!mSettings.noadditionalscriptmods) {
		setButtonPrompts();
		Log::TypedLog(CHN_DLL, "Adjusting button prompts\n");
		Log::TypedLog(CHN_DLL, "Patching scripts in memory\n");
	}
	editScriptsInMemory(); /*loads single functions of scripts and overwrites existing ones*/
	setDropDownKeys();
	setCavemanKeys();
	setLadderGrabKeys();
}


/* -=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=- */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=- Function definitions -=-=-=-=-=-=-=-=-=-=-=-=-=-= */
/* -=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=- */

typedef Script::LazyArray* __cdecl GlobalGetArray_NativeCall(uint32_t nameChecksum);
GlobalGetArray_NativeCall* GlobalGetArray_Native = (GlobalGetArray_NativeCall*)(0x00413650);

typedef Script::LazyStruct* __cdecl GlobalGetStructure_NativeCall(uint32_t nameChecksum);
GlobalGetStructure_NativeCall* GlobalGetStructure_Native = (GlobalGetStructure_NativeCall*)(0x00413600);

typedef uint8_t* __cdecl sCreateSymbolOfTheFormNameEqualsValue_NativeCall(uint8_t* p_token, const char* p_fileName);
sCreateSymbolOfTheFormNameEqualsValue_NativeCall* sCreateSymbolOfTheFormNameEqualsValue_Native = (sCreateSymbolOfTheFormNameEqualsValue_NativeCall*)(0x0040D1D0);

typedef bool __cdecl GetMemCardSpaceAvailable_NativeCall(void* pParams, DummyScript* pScript);
GetMemCardSpaceAvailable_NativeCall* GetMemCardSpaceAvailable_Native = (GetMemCardSpaceAvailable_NativeCall*)(0x0052DC70);

typedef bool __cdecl CreateScreenElement_NativeCall(Script::LazyStruct* pParams, DummyScript* pScript);
CreateScreenElement_NativeCall* CreateScreenElement_Native = (CreateScreenElement_NativeCall*)(0x00490B50);

typedef bool __cdecl SetScreenElementProps_NativeCall(Script::LazyStruct* pParams, DummyScript* pScript);
SetScreenElementProps_NativeCall* SetScreenElementProps_Native = (SetScreenElementProps_NativeCall*)(0x004920E0);

typedef bool __cdecl SetButtonEventMappings_NativeCall(Script::LazyStruct* pParams, DummyScript* pScript);
SetButtonEventMappings_NativeCall* SetButtonEventMappings_Native = (SetButtonEventMappings_NativeCall*)(0x005035E0);

typedef bool __cdecl ProfileLoggedIn_NativeCall(Script::LazyStruct* pParams, DummyScript* pScript);
ProfileLoggedIn_NativeCall* ProfileLoggedIn_Native = (ProfileLoggedIn_NativeCall*)(0x004D73C0);

typedef uint32_t CalculateScriptContentsChecksum_NativeCall(uint8_t* p_token);
CalculateScriptContentsChecksum_NativeCall* CalculateScriptContentsChecksum_Native = (CalculateScriptContentsChecksum_NativeCall*)(0x0040AAC0);

typedef uint32_t __cdecl ScriptGetArray_NativeCall(uint32_t partChecksum);
ScriptGetArray_NativeCall* ScriptGetArray_Native = (ScriptGetArray_NativeCall*)(0x00413330);

typedef uint32_t __cdecl ScriptCleanUpAndRemoveSymbol_NativeCall(uint32_t p_symbolName);
ScriptCleanUpAndRemoveSymbol_NativeCall* ScriptCleanUpAndRemoveSymbol_Native = (ScriptCleanUpAndRemoveSymbol_NativeCall*)(0x0040BAE0);

typedef void(__thiscall* LoadTextureFromBuffer_NativeCall)(int sp_sprite_tex_dict, uint8_t* p_buffer, uint32_t buffer_size, uint32_t texture_checksum, bool sprite, bool alloc_vram, bool perm);
LoadTextureFromBuffer_NativeCall LoadTextureFromBuffer_Native = (LoadTextureFromBuffer_NativeCall)(0x00484CF0);


/* -=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=- */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-= Patched CFuncs =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
/* -=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=- */

bool IsPS2_Patched(void* pParams, DummyScript* pScript) {

	if (pScript) {
		if (pScript->mScriptNameChecksum == 0x255ED86F /*grind*/ || 
			pScript->mScriptNameChecksum == 0x6AEC78DA /*check_for_neversoft_skaters*/ ||
			(mSettings.isPs2Controls && pScript->mScriptNameChecksum == 0x9DB065AD /*parked_set_helper_text_mode*/))
			return true;
	}
	return false;
}

bool IsXBOX_Patched(void* pParams, DummyScript* pScript) {

	if (pScript->mScriptNameChecksum == 0xEF384924 /*load_textures_to_main_memory*/) {
			LoadTextureFromBuffer_Native(*(int*)0x00707840, (uint8_t*)pResource_oslogo, sizeof(pResource_oslogo), 0x2074BEAE /*gslogo*/, true, true, true);
			return false;
	}
	return true;
}

bool GetMemCardSpaceAvailable_Patched(void* pParams, /*ebp + 0x8*/
										DummyScript* pScript, /*ebp+0xC*/
										uint32_t a, /*ebp+0x10*/
										ULARGE_INTEGER b, /*ebp+0x14*/
										ULARGE_INTEGER c, /*ebp+0x1C*/
										ULARGE_INTEGER d, /*ebp+0x24*/
										ULARGE_INTEGER e, /*ebp+0x2C*/
										ULARGE_INTEGER f, /*epc+0x34*/
										uint8_t p_card) { /*ebp+0x3C*/

	ULARGE_INTEGER mlpFreeBytesAvailableToCaller;
	ULARGE_INTEGER mlpTotalNumberOfBytes;
	uint32_t space_available = (INT_MAX - (UINT16_MAX * 2)); /*stdint.h, 0x7FFE0001*/
	uint32_t space_available_result = 0;
	uint32_t GetNumFreeClusters_Result = 0;

	pScript->GetParams->AddInteger(0x855B2FC, 1000000); /*FilesLeft*/

	if (p_card && GetDiskFreeSpaceExA(NULL, &mlpFreeBytesAvailableToCaller, &mlpTotalNumberOfBytes, NULL)) {
		space_available_result = ((mlpFreeBytesAvailableToCaller.HighPart << 0x16) + (mlpFreeBytesAvailableToCaller.LowPart >> 0xA));
		if (space_available_result <= space_available)
			space_available = space_available_result;
	}
	pScript->GetParams->AddInteger(0xC37C363, space_available); /*SpaceAvailable*/
	return true;
}

bool CreateScreenElement_Patched(Script::LazyStruct* pParams, DummyScript* pScript) {
	
	SkateInstance* Skate = (SkateInstance*)*(uint32_t*)(0x0076A788);
	uint32_t p_checksum = 0;

	if (Skate->m_cur_level == 0x9F2BAFB7 /*load_skateshop*/ && getaspectratio() > 1.34f) {

		if (pScript->mScriptNameChecksum == 0x7C92D11A) {  /*make_mainmenu_3d_plane*/
			pParams->GetChecksum(0x40C698AF, &p_checksum, false);  /*id*/
			if (p_checksum == 0xBC4B9584) /*bg_plane*/
				pParams->AddInteger(0xED7C6031, -281); /*cameraz*/
		}
	}

	if (!mSettings.noadditionalscriptmods) {
		if (pScript->mScriptNameChecksum == 0xCCB19938) { /*create_console_message*/

			if (mSettings.chatsize && mSettings.chatsize < 5) {

				//4 big: scale 1.0, padding_scale 0.85
				//3 default: scale 0.8, padding_scale 0.65
				//2 small: scale 0.5, padding_scale 0.45
				//1 extra small scale 0.3, padding_scale 0.25
				switch (mSettings.chatsize) {
				case 1:
					pParams->AddFloat(0x13B9DA7B, 0.3); /*scale*/
					break;
				case 2:
					pParams->AddFloat(0x13B9DA7B, 0.5); /*scale*/
					break;
				case 3:
					pParams->AddFloat(0x13B9DA7B, 0.8); /*scale*/
					break;
				case 4:
					pParams->AddFloat(0x13B9DA7B, 1.0); /*scale*/
					break;
				}
			}
		}
		else if (pScript->mScriptNameChecksum == 0xEBC4A74E) { /*create_console*/

			if (mSettings.chatsize && mSettings.chatsize < 5) {
				switch (mSettings.chatsize) {
				case 1:
					pParams->AddFloat(0x6D853B88, 0.25); /*padding_scale*/
					break;
				case 2:
					pParams->AddFloat(0x6D853B88, 0.45); /*padding_scale*/
					break;
				case 3:
					pParams->AddFloat(0x6D853B88, 0.65); /*padding_scale*/
					break;
				case 4:
					pParams->AddFloat(0x6D853B88, 0.85); /*padding_scale*/
					break;
				}
			}
		}
		else if (pScript->mScriptNameChecksum == 0x361F992E) { /*create_timeline_trick_info*/

			const char* text_content_cat_info;

			pParams->GetText(0xC4745838/*text*/, &text_content_cat_info, 0);
			if (mSettings.buttonfont != 1) {
				if (strstr(text_content_cat_info, "(\\bq)")) {
					if (!(mSettings.isPs2Controls)) {
						pParams->AddString(0xC4745838/*text*/, "(\\bh)"); /*R1*/
					}
					else {
						pParams->AddString(0xC4745838/*text*/, "(\\br)"); /*R1*/
					}
				}
			}
		}
		else if (pScript->mScriptNameChecksum == 0x85E146D5) { /*create_snazzy_dialog_box*/

			pParams->GetChecksum(0x7321A8D6, &p_checksum, false);  /*type*/

			if (p_checksum == 0x5200DFB6 || p_checksum == 0x40D92263) { /*textelement, textBlockElement*/

				const char* text_content_gs;

				pParams->GetText(0xC4745838/*text*/, &text_content_gs, 0);
				if (strstr(text_content_gs, "GameSpy")) {
					char* text_content_os = setText(text_content_gs, "GameSpy", "OpenSpy");
					pParams->AddString(0xC4745838/*text*/, text_content_os);
				}
				else if (strstr(text_content_gs, "GAMESPY")) {
					char* text_content_os = setText(text_content_gs, "GAMESPY", "OPENSPY");
					pParams->AddString(0xC4745838/*text*/, text_content_os);
				}
			}
		}
	}
	return CreateScreenElement_Native(pParams, pScript);
}

bool SetScreenElementProps_Patched(Script::LazyStruct* pParams, DummyScript* pScript) {
	
	SkateInstance* Skate = (SkateInstance*)*(uint32_t*)(0x0076A788);
	uint32_t p_checksum = 0;
	bool return_val = 0;

	if (pScript->mScriptNameChecksum == 0x8B149090) { /*scalingmenu_add_options_to_menu*/
		pScript->GetParams->GetChecksum(0xB6F08F39 /*part*/, &p_checksum, 0);
		if (p_checksum == 0xAC66013C /*board_bone_group*/) {
			RunScript(0x3DBD296D, pParams, nullptr, nullptr); /*showboard_myan*/
		}
	}
	else if (pScript->mScriptNameChecksum == 0xB9ED9B74) { /*create_internet_options*/
		pParams->GetChecksum(0x40C698AF /*id*/, &p_checksum, 0);
		if (p_checksum == 0x455A37D3 /*menu_create_profile*/) {
			if (ProfileLoggedIn_Native(pParams, pScript)) {
				return_val = SetScreenElementProps_Native(pParams, pScript);
				Script::LazyStruct* new_item = Script::LazyStruct::s_create();
				Script::LazyStruct* menu_create_profile_struct = Script::LazyStruct::s_create();

				//new_item = { id = { menu_create_profile child = 0} text = "Change OpenSpy Login" }
				if (new_item) {
					menu_create_profile_struct->AddChecksum(0, 0x455A37D3 /*menu_create_profile*/);
					menu_create_profile_struct->AddInteger(0xDD4CABD6 /*child*/, 0);
					new_item->AddStructure(0x40C698AF /*id*/, menu_create_profile_struct);
					new_item->AddString(0xC4745838 /*text*/, "Change OpenSpy Login");
					SetScreenElementProps_Native(new_item, pScript);

					if (menu_create_profile_struct) {
						menu_create_profile_struct->Clear();
						FreeQBStruct(menu_create_profile_struct);
						new_item->Clear();
						FreeQBStruct(new_item);
					}
				}
				return return_val;
			}
		}
		else if (p_checksum == 0x62D6356F /*menu_save*/) {
			/*
				 theme_menu_add_item {
					text = 'Save Settings'
					id = menu_save
					pad_choose_script = launch_save_internet_settings
					centered
					focus_script = internet_options_focus
					focus_params = { texture = io_settings }
					unfocus_script = internet_options_unfocus
				}
				SetScreenElementProps {
					id = menu_save
					event_handlers = [
						{focus PlayHelperDesc params = { save_settings_helper_desc }}
						{ unfocus StopHelperDesc }
					]
				}
			*/
			Script::LazyStruct* new_item = Script::LazyStruct::s_create();

			if (new_item) {
				new_item->AddString(0xC4745838 /*text*/, "Load Settings");
				new_item->AddChecksum(0x40C698AF /*id*/, 0x12B6EBAC /*menu_load*/);
				new_item->AddChecksum(0x2B0CFCA1 /*pad_choose_script*/, 0x07440DFA /*LoadInternetOptions_PadChooseScript_myan*/);
				new_item->AddChecksum(0, 0x2A434D05 /*centered*/);
				new_item->AddChecksum(0x5A8C82DB /*focus_script*/, 0x33B80F0B /*internet_options_focus*/);
				Script::LazyStruct* focus_params = Script::LazyStruct::s_create();

				if (focus_params) {
					focus_params->AddChecksum(0x7D99F28D /*texture*/, 0xB2814858 /*io_settings*/);
					new_item->AddStructure(0xC9C30B12 /*focus_params*/, focus_params);
					new_item->AddChecksum(0x6A3B36B5 /*unfocus_script*/, 0x9E0794DA /*internet_options_unfocus*/);
					RunScript(0xC16BF7BD /*theme_menu_add_item*/, new_item, nullptr, nullptr);
					if (focus_params) {
						new_item->Free();
						new_item->AddChecksum(0x40C698AF /*id*/, 0x12B6EBAC /*menu_load*/);
						Script::LazyStruct* event_handlers_struct = Script::LazyStruct::s_create();
						Script::LazyStruct* save_settings_helper_desc = Script::LazyStruct::s_create();
						save_settings_helper_desc->AddChecksum(0, 0x926D3E69 /*LoadInternetOptions_HelperDesc_myan*/);
						event_handlers_struct->AddChecksum(0, 0x9D3FB516 /*focus*/);
						event_handlers_struct->AddChecksum(0, 0x28C8A177 /*PlayHelperDesc*/);
						event_handlers_struct->AddStructure(0x7031F10C /*params*/, save_settings_helper_desc);
						focus_params->AddChecksum(0, 0x4ADF0CD3 /*unfocus*/);
						focus_params->AddChecksum(0, 0x28D11C80  /*StopHelperDesc*/);

						Script::LazyArray* event_handlers = Script::LazyArray::s_create();
						if (event_handlers_struct) {
							event_handlers->SetSizeAndType(2, ESYMBOLTYPE_STRUCTURE);
							event_handlers->SetStructure(0, event_handlers_struct);
							event_handlers->SetStructure(1, focus_params);
							new_item->AddArray(0x475BF03C /*event_handlers*/, event_handlers);
							SetScreenElementProps_Native(new_item, pScript);

							save_settings_helper_desc->Clear();
							FreeQBStruct(save_settings_helper_desc);
							event_handlers_struct->Clear();
							FreeQBStruct(event_handlers_struct);
							if (focus_params) {
								focus_params->Clear();
								FreeQBStruct(focus_params);
							}
							event_handlers->Clear();
							FreeQBArray(event_handlers);
							new_item->Clear();
							FreeQBStruct(new_item);
						}
					}
				}
			}
		}
	}
	return SetScreenElementProps_Native(pParams, pScript);
}

bool SetButtonEventMappings_Patched(Script::LazyStruct* pParams, DummyScript* pScript) {

	if (pScript->mScriptNameChecksum == 0xE2602BAC /*setup_main_button_event_mappings*/) {

		Script::LazyArray* ButtonEventMap_xbox = nullptr;
		Script::LazyArray* ButtonMap = nullptr;

		pParams->GetArray(0x87D839B8 /*xbox*/, &ButtonEventMap_xbox);

		ButtonMap = ButtonEventMap_xbox->GetArray(6); //edit first entry inside array: [ b pad_back ] => [ y pad_back ]
		ButtonMap->SetChecksum(0, 0x0424D9EA /*y*/);
		//This makes speech bubbles react to triangle
		//ButtonMap = ButtonEventMap_xbox->GetArray(7); //[ b pad_circle ] => [ y pad_circle ]
		//ButtonMap->SetChecksum(0, 0x0424D9EA /*y*/);
		ButtonMap = ButtonEventMap_xbox->GetArray(12); //[ y pad_triangle2 ] => [ b pad_triangle2 ]
		ButtonMap->SetChecksum(0, 0x8E411006 /*b*/);
		ButtonMap = ButtonEventMap_xbox->GetArray(14); //[ y pad_space ] => [ b pad_space ]
		ButtonMap->SetChecksum(0, 0x8E411006 /*b*/);
		ButtonMap = ButtonEventMap_xbox->GetArray(17); //[ y pad_expand ] => [ b pad_expand ]
		ButtonMap->SetChecksum(0, 0x8E411006 /*b*/);
	}
	return SetButtonEventMappings_Native(pParams, pScript);
}


/* -=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=- */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- Scripty Stuff =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
/* -=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=- */

void editScriptsInMemory() {

	uint32_t contentsChecksum = 0;

	if (!mSettings.noadditionalscriptmods) {

		//Longer text input in online chat
		removeScript(0x3B4548B8); /*enter_kb_chat*/
		contentsChecksum = CalculateScriptContentsChecksum_Native((uint8_t*)enter_kb_chat_new);
		sCreateScriptSymbol_Wrapper(sizeof(enter_kb_chat_new), 0x3B4548B8, contentsChecksum, (uint8_t*)enter_kb_chat_new, "scripts\\game\\game.qb");

		//Increased color and saturation sliders
		removeScript(0xB26B0D6F); /*colormenu_min_value*/
		sCreateSymbolOfTheFormNameEqualsValue_Native((uint8_t*)colormenu_min_value_new, "scripts\\engine\\menu\\colormenu.qb");
		removeScript(0x6580BF2E); /*colormenu_max_value*/
		sCreateSymbolOfTheFormNameEqualsValue_Native((uint8_t*)colormenu_max_value_new, "scripts\\engine\\menu\\colormenu.qb");
		removeScript(0x63BBA1ED); /*colormenu_min_saturation*/
		sCreateSymbolOfTheFormNameEqualsValue_Native((uint8_t*)colormenu_min_saturation_new, "scripts\\engine\\menu\\colormenu.qb");
		removeScript(0xDA9D3A9C); /*colormenu_min_saturation*/
		sCreateSymbolOfTheFormNameEqualsValue_Native((uint8_t*)colormenu_max_saturation_new, "scripts\\engine\\menu\\colormenu.qb");
		
		//Show board during board scaling in C-A-S. New script: showboard_myan 0x3DBD296D
		contentsChecksum = CalculateScriptContentsChecksum_Native((uint8_t*)showboard_myan);
		sCreateScriptSymbol_Wrapper(sizeof(showboard_myan), 0x3DBD296D, contentsChecksum, (uint8_t*)showboard_myan, "scripts\\myan.qb");

		//Unlimited scale in C-A-S (no arrows)
		removeScript(0x9F95228A); /*scalingmenu_get_limits*/
		contentsChecksum = CalculateScriptContentsChecksum_Native((uint8_t*)scalingmenu_get_limits_new);
		sCreateScriptSymbol_Wrapper(sizeof(scalingmenu_get_limits_new), 0x9F95228A, contentsChecksum, (uint8_t*)scalingmenu_get_limits_new, "scripts\\engine\\menu\\scalingmenu.qb");

		//Padding between axis sliders
		removeScript(0xFAC8CE58); /*scalingmenu_spacing_between*/
		sCreateSymbolOfTheFormNameEqualsValue_Native((uint8_t*)scalingmenu_spacing_between_new, "scripts\\engine\\menu\\scalingmenu.qb");

		//Three axis scaling for all body parts and board
		removeScript(0xD2BE4CAF); /*skateshop_scaling_options*/
		sCreateSymbolOfTheFormNameEqualsValue_Native((uint8_t*)skateshop_scaling_options_new, "scripts\\engine\\menu\\scalingmenu.qb");

		//Remove board when leaving board scale menu
		removeScript(0xFA23D9E3); /*edit_skater_create_options_menu*/
		contentsChecksum = CalculateScriptContentsChecksum_Native((uint8_t*)edit_skater_create_options_menu_new);
		sCreateScriptSymbol_Wrapper(sizeof(edit_skater_create_options_menu_new), 0xFA23D9E3, contentsChecksum, (uint8_t*)edit_skater_create_options_menu_new, "scripts\\myan.qb");

		//Helper: LoadInternetOptions_AbortAndDoneScript_myan
		removeScript(0x33317668);
		contentsChecksum = CalculateScriptContentsChecksum_Native((uint8_t*)LoadInternetOptions_AbortAndDoneScript_myan);
		sCreateScriptSymbol_Wrapper(sizeof(LoadInternetOptions_AbortAndDoneScript_myan), 0x33317668, contentsChecksum, (uint8_t*)LoadInternetOptions_AbortAndDoneScript_myan, "scripts\\myan.qb");

		//Helper: LoadInternetOptions_RetryScript_myan
		removeScript(0x1C253B2E);
		contentsChecksum = CalculateScriptContentsChecksum_Native((uint8_t*)LoadInternetOptions_RetryScript_myan);
		sCreateScriptSymbol_Wrapper(sizeof(LoadInternetOptions_RetryScript_myan), 0x1C253B2E, contentsChecksum, (uint8_t*)LoadInternetOptions_RetryScript_myan, "scripts\\myan.qb");

		//Helper: LoadInternetOptions_PadChooseScript_myan
		removeScript(0x07440DFA);
		contentsChecksum = CalculateScriptContentsChecksum_Native((uint8_t*)LoadInternetOptions_PadChooseScript_myan);
		sCreateScriptSymbol_Wrapper(sizeof(LoadInternetOptions_PadChooseScript_myan), 0x07440DFA, contentsChecksum, (uint8_t*)LoadInternetOptions_PadChooseScript_myan, "scripts\\myan.qb");

		//Helper: LoadInternetOptions_HelperDesc_myan
		removeScript(0x926D3E69);
		sCreateSymbolOfTheFormNameEqualsValue_Native((uint8_t*)LoadInternetOptions_HelperDesc_myan, "scripts\\myan.qb");

		//Select Internet play on top
		removeScript(0x40BB7409); /*select_network_play_multi*/
		contentsChecksum = CalculateScriptContentsChecksum_Native((uint8_t*)select_network_play_multi_new);
		sCreateScriptSymbol_Wrapper(sizeof(select_network_play_multi_new), 0x40BB7409, 0x62EE909F, (uint8_t*)select_network_play_multi_new, "scripts\\myan.qb");

		// No chat reset after games start/end in net games
		if (mSettings.consolewaittime != 30) {
			if (mSettings.consolewaittime && mSettings.consolewaittime <= 120) console_wait_time[7] = mSettings.consolewaittime;
		}
		removeScript(0x074DBDBB); /*console_wait_time*/
		sCreateSymbolOfTheFormNameEqualsValue_Native((uint8_t*)console_wait_time, "engine\\menu\\consolemessage.qb");

		removeScript(0x0C42890C); /*kill_net_panel_messages*/
		contentsChecksum = CalculateScriptContentsChecksum_Native((uint8_t*)kill_net_panel_messages_new);
		sCreateScriptSymbol_Wrapper(sizeof(kill_net_panel_messages_new), 0x0C42890C, contentsChecksum, (uint8_t*)kill_net_panel_messages_new, "scripts\\game\\net\\net.qb");

		removeScript(0xCCB19938); /*create_console_message*/
		contentsChecksum = CalculateScriptContentsChecksum_Native((uint8_t*)create_console_message_new);
		sCreateScriptSymbol_Wrapper(sizeof(create_console_message_new), 0xCCB19938, contentsChecksum, (uint8_t*)create_console_message_new, "engine\\menu\\consolemessage.qb");

		removeScript(0xF2F8DF40); /*console_message_wait_and_die*/
		contentsChecksum = CalculateScriptContentsChecksum_Native((uint8_t*)console_message_wait_and_die_new);
		sCreateScriptSymbol_Wrapper(sizeof(console_message_wait_and_die_new), 0xF2F8DF40, contentsChecksum, (uint8_t*)console_message_wait_and_die_new, "engine\\menu\\consolemessage.qb");

		removeScript(0x8c34fe0a); /*gameflow_startrun*/
		contentsChecksum = CalculateScriptContentsChecksum_Native((uint8_t*)gameflow_startrun_new);
		sCreateScriptSymbol_Wrapper(sizeof(gameflow_startrun_new), 0x8C34FE0A, contentsChecksum, (uint8_t*)gameflow_startrun_new, "game\\gameflow.qb");

		//No freeze when changing levels in net games
		removeScript(0x39C58EA1); /*change_level*/
		contentsChecksum = CalculateScriptContentsChecksum_Native((uint8_t*)change_level_new);
		sCreateScriptSymbol_Wrapper(sizeof(change_level_new), 0x39C58EA1, contentsChecksum, (uint8_t*)change_level_new, "game\\skutils.qb");

		//Fix crash when connecting online right after leaving the lobby
		removeScript(0xBD54C02B); /*connected_to_internet*/
		contentsChecksum = CalculateScriptContentsChecksum_Native((uint8_t*)connected_to_internet_new);
		sCreateScriptSymbol_Wrapper(sizeof(connected_to_internet_new), 0xBD54C02B, contentsChecksum, (uint8_t*)connected_to_internet_new, "game\\net\\net.qb");
		

		if (!mSettings.boardscuffs) removeScript(0x9CE4DA4F); /*DoBoardScuff*/
	}

	//Restore onscreen keyboard
	if (pResource_keyboard_restored = getResource(IDR_KEYBOARD_RESTORED)) {
		removeScript(0xF0425254); /*create_onscreen_keyboard*/
		contentsChecksum = CalculateScriptContentsChecksum_Native((uint8_t*)pResource_keyboard_restored);
		sCreateScriptSymbol_Wrapper(5896, 0xF0425254, contentsChecksum, (uint8_t*)pResource_keyboard_restored, "engine\\menu\\keyboard.qb");
	}
}

void __fastcall sCreateScriptSymbol_Wrapper(uint32_t size, uint32_t nameChecksum, uint32_t contentsChecksum, const uint8_t* p_data, const char* p_fileName) {
	__asm {
		push dword ptr ss : [ebp + 0x10] /* *p_fileName */
		push dword ptr ss : [ebp + 0xC] /* *p_data */
		push dword ptr ss : [ebp + 0x8] /* contentsChecksum */
		push edx /* nameChecksum */
		mov eax, ecx  /* size */
		call dword ptr ds : addr_sCreateScriptSymbol
		mov esp, ebp
		pop ebp
		ret 12
	}
}

uint32_t __fastcall removeScript(uint32_t partChecksum) {
	uint32_t p_script = 0;
	p_script = ScriptGetArray_Native(partChecksum);
	if (p_script)
		p_script = ScriptCleanUpAndRemoveSymbol_Native(p_script);
	return p_script;
}

void setDropDownKeys() {
	/*
	if (pref_dropdown = L1)
		change GrindRelease = [ {
			trigger = { Press l1 100 }
			scr = SkateInOrBail
			params = { GrindRelease grindbail = Airborne moveright = -5 movey = 5 }
		} ]
	endif

	PC controls: white = L2 / black = R2 / L1 = L1 / R1 = R1
	*/
	if (mSettings.dropdowncontrol && mSettings.dropdowncontrol < 6)
	{
		Script::LazyStruct* grindrelease = Script::LazyStruct::s_create();
		Script::LazyStruct* trigger = Script::LazyStruct::s_create();
		Script::LazyStruct* params = Script::LazyStruct::s_create();

		if (mSettings.isPs2Controls)
		{
			if (mSettings.dropdowncontrol == 1 || mSettings.dropdowncontrol == 5)
			{
				/*Set R2*/
				trigger->AddChecksum(0, 0x823B8342); /*press*/
				trigger->AddChecksum(0, 0x6BF8A7F4); /*R2*/
			}
			if (mSettings.dropdowncontrol == 2)
			{
				/*Set L1*/
				trigger->AddChecksum(0, 0x823B8342); /*press*/
				trigger->AddChecksum(0, 0x26B0C991); /*L1*/
			}
			else if (mSettings.dropdowncontrol == 3)
			{
				/*Set R1*/
				trigger->AddChecksum(0, 0x823B8342); /*press*/
				trigger->AddChecksum(0, 0xF2F1F64E); /*R1*/
			}
			else if (mSettings.dropdowncontrol == 4)
			{
				/*Set L2*/
				trigger->AddChecksum(0, 0x823B8342); /*press*/
				trigger->AddChecksum(0, 0xBFB9982B); /*L2*/
			}
		}
		else {
			if (mSettings.dropdowncontrol == 1) {
				// No dropdowns on THUG PC originally. Set default to THUG2 PC dropdown L2+R2

				/*Set L2+R2*/
				trigger->AddChecksum(0, 0x3F369070); /*PressTwoAnyOrder*/
				trigger->AddChecksum(0, 0xBFB9982B); /*L2*/
				trigger->AddChecksum(0, 0x6bF8A7F4); /*R2*/
			}
			if (mSettings.dropdowncontrol == 2)
			{
				/*Set L1*/
				trigger->AddChecksum(0, 0x823B8342); /*press*/
				trigger->AddChecksum(0, 0xBD30325B); /*white*/
			}
			else if (mSettings.dropdowncontrol == 3)
			{
				/*Set R1*/
				trigger->AddChecksum(0, 0x823B8342); /*press*/
				trigger->AddChecksum(0, 0x767A45D7); /*black*/
			}
			else if (mSettings.dropdowncontrol == 4)
			{
				/*Set L2*/
				trigger->AddChecksum(0, 0x823B8342); /*press*/
				trigger->AddChecksum(0, 0xbFB9982B); /*L2*/
			}
			else if (mSettings.dropdowncontrol == 5)
			{
				/*Set R2*/
				trigger->AddChecksum(0, 0x823B8342); /*press*/
				trigger->AddChecksum(0, 0x6BF8A7F4); /*R2*/
			}
		}

		trigger->AddInteger(0, 100);

		params->AddChecksum(0, 0x9077508B); /*GrindRelease*/
		params->AddChecksum(0xA7E24442, 0xCF3C89F7); /*GrindBail, Airborne*/
		params->AddInteger(0x1878C644, -5); /*MoveRight*/
		params->AddInteger(0x5A151ED3, 5); /*MoveY*/

		grindrelease->AddStructure(0xE594F0A2, trigger); /*Trigger*/
		grindrelease->AddChecksum(0xA6D2D890, 0x90E528CA); /*Scr, SkateOrInBail*/
		grindrelease->AddStructure(0x7031F10C, params); /*params*/

		Script::LazyArray* pref_dropdown = GlobalGetArray_Native(0x9077508B); // , 0, grindrelease); /*GrindRelease*/
		pref_dropdown->SetStructure(0, grindrelease);
	}
}

void setCavemanKeys() {
	//CavemanKey=1 ; 1 = default, 2 = L1, 3 = R1, 4 = L2, 5 = R2, 6 = L1+R1, 7 = L2+R2 ; PC default: black or white which is R2 or L2
	//In PC controls, white = L2 / black = R2
	if (mSettings.cavemancontrol > 1)
	{
		Script::LazyStruct* trigger = Script::LazyStruct::s_create();

		if (mSettings.isPs2Controls)
		{
			if (mSettings.cavemancontrol == 2)
			{
				/*Set L1*/
				trigger->AddChecksum(0, 0x823B8342); /*press*/
				trigger->AddChecksum(0, 0x26B0C991); /*L1*/
				trigger->AddInteger(0, 10);
			}
			else if (mSettings.cavemancontrol == 3)
			{
				/*Set R1*/
				trigger->AddChecksum(0, 0x823B8342); /*press*/
				trigger->AddChecksum(0, 0xF2F1F64E); /*R1*/
				trigger->AddInteger(0, 10);
			}
			else if (mSettings.cavemancontrol == 4)
			{
				/*Set L2*/
				trigger->AddChecksum(0, 0x823B8342); /*press*/
				trigger->AddChecksum(0, 0xBFB9982B); /*L2*/
				trigger->AddInteger(0, 10);
			}
			else if (mSettings.cavemancontrol == 5)
			{
				/*Set R2*/
				trigger->AddChecksum(0, 0x823B8342); /*press*/
				trigger->AddChecksum(0, 0x6BF8A7F4); /*R2*/
				trigger->AddInteger(0, 10);
			}
			else if (mSettings.cavemancontrol == 6)
			{
				/*Set L1+R1*/
				trigger->AddChecksum(0, 0x3F369070); /*PressTwoAnyOrder*/
				trigger->AddChecksum(0, 0x26B0C991); /*L1*/
				trigger->AddChecksum(0, 0xF2F1F64E); /*R1*/
				trigger->AddInteger(0, 400);
			}
			else if (mSettings.cavemancontrol == 7)
			{
				/*Set L2+R2*/
				trigger->AddChecksum(0, 0x3F369070); /*PressTwoAnyOrder*/
				trigger->AddChecksum(0, 0xBFB9982B); /*L2*/
				trigger->AddChecksum(0, 0x6BF8A7F4); /*R2*/
				trigger->AddInteger(0, 400);
			}
		}
		else {
			if (mSettings.cavemancontrol == 2)
			{
				/*Set L1*/
				trigger->AddChecksum(0, 0x823B8342); /*press*/
				trigger->AddChecksum(0, 0xBD30325B); /*white*/
				trigger->AddInteger(0, 10);
			}
			else if (mSettings.cavemancontrol == 3)
			{
				/*Set R1*/
				trigger->AddChecksum(0, 0x823B8342); /*press*/
				trigger->AddChecksum(0, 0x767A45D7); /*black*/
				trigger->AddInteger(0, 10);

			}
			else if (mSettings.cavemancontrol == 4)
			{
				/*Set L2*/
				trigger->AddChecksum(0, 0x823B8342); /*press*/
				trigger->AddChecksum(0, 0x26B0C991); /*L1*/
				trigger->AddInteger(0, 10);
			}
			else if (mSettings.cavemancontrol == 5)
			{
				/*Set R2*/
				trigger->AddChecksum(0, 0x823B8342); /*press*/
				trigger->AddChecksum(0, 0xF2F1F64E); /*R1*/
				trigger->AddInteger(0, 10);

			}
			else if (mSettings.cavemancontrol == 6)
			{
				/*Set L1+R1*/
				trigger->AddChecksum(0, 0x3F369070); /*PressTwoAnyOrder*/
				trigger->AddChecksum(0, 0x767A45D7); /*black*/
				trigger->AddChecksum(0, 0xBD30325B); /*white*/
				trigger->AddInteger(0, 400);
			}
			else if (mSettings.cavemancontrol == 7)
			{
				/*Set L2+R2*/
				trigger->AddChecksum(0, 0x3F369070); /*PressTwoAnyOrder*/
				trigger->AddChecksum(0, 0x26B0C991); /*L1*/
				trigger->AddChecksum(0, 0xF2F1F64E); /*R1*/
				
				trigger->AddInteger(0, 400);
			}

		}
		Script::LazyStruct* pref_caveman = GlobalGetStructure_Native(0xB32BEA02); //SwitchControl_Trigger
		pref_caveman->Free();
		pref_caveman->AddStructurePointer(0xE594F0A2, trigger); //trigger
	}
}

void setLadderGrabKeys() {

	if (!mSettings.isPs2Controls) {
		if (mSettings.laddergrabcontrol == 2) {
			patchBytesM((BYTE*)(0x004690E0 + 2), (BYTE*)"\xC0\x00", 2); /* L2 */
			patchBytesM((BYTE*)(0x00469ED9 + 2), (BYTE*)"\xC0\x00", 2); /* L2 */
			patchBytesM((BYTE*)(0x00469A89 + 2), (BYTE*)"\xA0\x00", 2); /* L2 */
		}
	}
	else {
		if (mSettings.laddergrabcontrol == 2) {
			patchBytesM((BYTE*)(0x004690E0 + 2), (BYTE*)"\xA0\x01", 2); /* L1 */
			patchBytesM((BYTE*)(0x00469ED9 + 2), (BYTE*)"\xA0\x01", 2); /* L1 */
			patchBytesM((BYTE*)(0x00469A89 + 2), (BYTE*)"\x80\x00", 2); /* L1 */
		}
	}
}

void setButtonPrompts() {

	if (mSettings.buttonfont > 1 && mSettings.buttonfont < 5) {

		if (!mSettings.isPs2Controls) {

			/* XBOX controls */
			if (mSettings.buttonfont != 3) {
				
				/* PS2 font (and ngc but we don't care about that) */

				patchByte((void*)0x005AD52F, 0x07); // R1 -> R2
				patchByte((void*)0x005AD532, 0x05); //L1+R1 => R1
				patchByte((void*)0x005AD53A, 0x05); // L1 -> R1
				patchByte((void*)0x005AD53B, 0x04); // R1 -> L1
				patchByte((void*)0x005AD531, 0x05); // R2 -> R1

				setHelperText(0x4125FAE0/*park_editor_helper_text_xbox*/, 3, "\\bg/\\bf = Lower/Raise"); //Set L2 in helper text to be displayed as L1
				setHelperText(0x4125FAE0/*park_editor_helper_text_xbox*/, 4, "\\be/\\bh =Zoom"); //Set L1 in helper text to be displayed as L2
				setHelperText(0x84EB1BCB/*rail_editor_free_roam_helper_text_xbox*/, 4, "\\bg/\\bf =Lower/Raise");
				setHelperText(0x84EB1BCB/*rail_editor_free_roam_helper_text_xbox*/, 5, "\\be/\\bh =Zoom");
				setHelperText(0x8ebd23fd/*rail_editor_layout_helper_text_xbox*/, 3, "\\bg/\\bf =Lower/Raise");
				setHelperText(0x8ebd23fd/*rail_editor_layout_helper_text_xbox*/, 4, "\\be/\\bh =Zoom");
				setHelperText(0xFBC77044/*rail_editor_grab_helper_text_xbox*/, 3, "\\bg/\\bf =Lower/Raise");
				setHelperText(0xFBC77044/*rail_editor_grab_helper_text_xbox*/, 4, "\\be/\\bh =Zoom");

				// \\bg = L2 ; \\bf = R2
				// \\be = L1 ; \\ bh = R1
				//patchByte((void*)0x005AD53C, 0x04);
				//patchByte((void*)0x005AD53D, 0x05);
				//patchByte((void*)0x005AD530, 0x04); //patch bg -> be (L2 => L1)
				//patchByte((void*)0x005AD52E, 0x06); //patch be -> bg (L1 => L2)
			}
			else {

				/* XBOX font */

				setHelperText(0x093BCE23/*generic_helper_text_cas*/, 3, "\\bq/\\br = Rotate");
				setHelperText(0x4F864854/*generic_helper_text_cas_z*/, 3, "\\bq/\\br = Rotate");
				setHelperText(0x4F864854/*generic_helper_text_cas_z*/, 4, "\\bt = Zoom");
				setHelperText(0xCE014F9E/*generic_helper_text_color_menu*/, 4, "\\bq/\\br=Rotate");
				setHelperText(0x8452EC2D/*generic_helper_text_color_menu_z*/, 4, "\\bq/\\br=Rotate");
				setHelperText(0x8452EC2D/*generic_helper_text_color_menu_z*/, 5, "\\bt = Zoom");
				setHelperText(0xB52596F6/*generic_helper_text_color_menu_reset*/, 1, "\\bq/\\br=Rotate");
				setHelperText(0xB52596F6/*generic_helper_text_color_menu_reset*/, 2, "\\bs=Reset");
				setHelperText(0xB52596F6/*generic_helper_text_color_menu_reset*/, 3, "\\bt=Zoom");
				setHelperText(0xE6AD8FF7/*generic_helper_text_color_menu_reset_cad*/, 1, "\\bs=Reset");
				setHelperText(0xA65EC4BA/*cag_helper_text_xbox*/, 3, "\\bq/\\br=Lower/Raise");
				setHelperText(0xA65EC4BA/*cag_helper_text_xbox*/, 4, "\\bs/\\bt=Zoom");
				setHelperText(0xE827750F/*cag_helper_text_no_back_xbox*/, 2, "\\bq/\\br = Lower/Raise");
				setHelperText(0xE827750F/*cag_helper_text_no_back_xbox*/, 3, "\\bs/\\bt = Zoom");
			}
		}
		else {

			/* XBOX controls */
			if (mSettings.buttonfont != 3) {

				/* PS2 controls*/

				//setHelperText(0x4125FAE0/*park_editor_helper_text_xbox*/, 3, "\\be/\\bg =Raise/Lower"); //Not needed since IsPs2 CFunc returns true for this script
				//setHelperText(0x4125FAE0/*park_editor_helper_text_xbox*/, 4, "\\bf/\\bh =Zoom");
				setHelperText(0x84EB1BCB/*rail_editor_free_roam_helper_text*/, 4, "\\be/\\bg =Raise/Lower");
				setHelperText(0x84EB1BCB/*rail_editor_free_roam_helper_text*/, 5, "\\bf/\\bh =Zoom");
				setHelperText(0x8ebd23fd /*rail_editor_layout_helper_text_xbox*/, 3, "\\be/\\bg =Raise/Lower");
				setHelperText(0x8ebd23fd /*rail_editor_layout_helper_text_xbox*/, 4, "\\bf/\\bh =Zoom");

				setHelperText(0xFBC77044/*rail_editor_grab_helper_text_xbox*/, 3, "\\be/\\bg =Raise/Lower");
				setHelperText(0xFBC77044/*rail_editor_grab_helper_text_xbox*/, 4, "\\bf/\\bh =Zoom");

				setHelperText(0xA737F35E/*gap_regular_helper_text*/, 0, "\\b0 = Delete");
				setHelperText(0x093BCE23/*generic_helper_text_cas*/, 3, "\\bq/\\br = Rotate");
				setHelperText(0x4F864854/*generic_helper_text_cas_z*/, 3, "\\bq/\\br = Rotate");
				setHelperText(0x4F864854/*generic_helper_text_cas_z*/, 4, "\\bt = Zoom");
				setHelperText(0xCE014F9E/*generic_helper_text_color_menu*/, 4, "\\bq/\\br=Rotate");
				setHelperText(0x8452EC2D/*generic_helper_text_color_menu_z*/, 4, "\\bq/\\br=Rotate");
				setHelperText(0x8452EC2D/*generic_helper_text_color_menu_z*/, 5, "\\bt = Zoom");
				setHelperText(0xB52596F6/*generic_helper_text_color_menu_reset*/, 1, "\\bq/\\br=Rotate");
				setHelperText(0xB52596F6/*generic_helper_text_color_menu_reset*/, 2, "\\bs=Reset");
				setHelperText(0xB52596F6/*generic_helper_text_color_menu_reset*/, 3, "\\bt=Zoom");
				setHelperText(0xE6AD8FF7/*generic_helper_text_color_menu_reset_cad*/, 1, "\\bs=Reset");
				setHelperText(0xA65EC4BA/*cag_helper_text_xbox*/, 3, "\\be/\\bg=Raise/Lower");
				setHelperText(0xA65EC4BA/*cag_helper_text_xbox*/, 4, "\\bf/\\bh=Zoom");
				setHelperText(0xE827750F/*cag_helper_text_no_back_xbox*/, 2, "\\be/\\bg = Raise/Lower");
				setHelperText(0xE827750F/*cag_helper_text_no_back_xbox*/, 3, "\\bf/\\bh = Zoom");
			}
		}
	}
	setHelperText(0x30B25099/*gap_regular_helper_text_xbox*/, 1, "\\b3 = Accept");
	setHelperText(0xDDCF8C99/*gap_adjust_helper_text_xbox*/, 1, "\\b1/\\b2 = Rotate");

	/* CAT menu*/
	if (mSettings.isPs2Controls) {

		removeScript(0x0CA5D7F8); /*add_timeline_helper_text*/
		uint32_t contentsChecksum = CalculateScriptContentsChecksum_Native((uint8_t*)add_timeline_helper_text_ps2_controls_ps2font);
		sCreateScriptSymbol_Wrapper(sizeof(add_timeline_helper_text_ps2_controls_ps2font), 0x0CA5D7F8, contentsChecksum, (uint8_t*)add_timeline_helper_text_ps2_controls_ps2font, "game\\menu\\catmenu.qb");
	}
	else {
		if (mSettings.buttonfont != 3) {

			/* PS2 and PC font (and ngc but we still don't care about it).
			XBOX font is already correct except for the spin on/off text which is handled in CreateScreenElement_Patched */

			removeScript(0x0CA5D7F8); /*add_timeline_helper_text*/
			uint32_t contentsChecksum = CalculateScriptContentsChecksum_Native((uint8_t*)add_timeline_helper_text_xbox_controls_ps2font);
			sCreateScriptSymbol_Wrapper(sizeof(add_timeline_helper_text_xbox_controls_ps2font), 0x0CA5D7F8, contentsChecksum, (uint8_t*)add_timeline_helper_text_xbox_controls_ps2font, "game\\menu\\catmenu.qb");
		}
	}
}

void setHelperText(uint32_t struct_checksum, int index, char* text) {

	//scripts/engine/menu/helper_text.q
	Script::LazyArray* helper_text_elements = nullptr;

	Script::LazyStruct* helper_text = GlobalGetStructure_Native(struct_checksum);
	if (helper_text) {
		helper_text->GetArray(0xE954A378, &helper_text_elements); /*helper_text_elements*/
		if (helper_text_elements)
			helper_text_elements->GetStructure(index)->AddString(0xC4745838, text); /*text*/
	}
}

char* setText(const char* text_content, const char* old_word, const char* new_word) {
	const char* pos = text_content; 
	int count = 0; 
	int old_word_len = strlen(old_word); 
	int new_word_len = strlen(new_word); 

	// Count occurrences of old_word in text_content 
	while ((pos = strstr(pos, old_word)) != NULL) { 
		++count; pos += old_word_len; 
	} 
	
	// Allocate memory for the new string 
	char* result = new char[strlen(text_content) + count * (new_word_len - old_word_len) + 1]; 
	char* current_pos = result; // Replace old_word with new_word 

	while ((pos = strstr(text_content, old_word)) != NULL) { 
		int len = pos - text_content; 
		strncpy(current_pos, text_content, len); 
		current_pos += len; strcpy(current_pos, new_word); 
		current_pos += new_word_len; text_content = pos + old_word_len; 
	} 
	
	// Copy the remaining part of the text_content 
	strcpy(current_pos, text_content); 
	return result; 
}
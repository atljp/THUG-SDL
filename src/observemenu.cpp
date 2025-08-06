#include "observemenu.h"


struct SkaterProfile;

struct PlayerInfo {
	char padding0[0x4];
	PlayerInfo* localPlayer;
	char reserved[0x4]; // 0x00000000
	SkaterProfile* NextSkater;
	SkaterProfile* PrevSkater;
	void* m_Conn;
	int m_Score;
	int m_BestCombo;
	int m_Profile;
	int m_Rating;
	int m_Something;
	int m_Team;
	char m_Name[vMAX_PLAYER_NAME_LEN+1];
	char padding1[0x108]; //0x14A
	uint8_t playerstate;
	char padding2[1];
	bool IsSurveying;
};

struct SkaterProfile {
	char padding[0x4];
	PlayerInfo* id;
	char padding2[0x4];
	PlayerInfo* localSkater;
	PlayerInfo* localSkater2;
};

struct GameNetMan {
	char unk0[4];
	uint32_t test;
	char unk1[0x32];
	uint32_t idk;
	char unk2[0x82]; // 0xC0
	PlayerInfo* GetCurrentlyObservedPlayer;
	PlayerInfo* GetServerPlayer;
	PlayerInfo* GetLocalPlayer;
	PlayerInfo* GetNextPlayerToObserve;
	PlayerInfo* GetKingOfTheHill;
	PlayerInfo* FirstNewPlayerInfo;
	PlayerInfo* NextNewPlayerInfo;
	PlayerInfo* GetNewPlayerInfoByObjectID;
	char unk3[0x68];
	PlayerInfo* observer;
};

struct Component {
	char padding[0x18];
	uint8_t uberfrigged;
};

struct CGameMode {
	char unk[100];
};

int observerOrPendingPlayerState = (mOBSERVER | mPENDING_PLAYER);
int localPlayerState = (mHAS_PLAYER_INFO | mLOCAL_PLAYER);
bool handle_received = false;
CGameMode* GameMode = {};
char* playername = "---";


/* -=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=- */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=- Function definitions -=-=-=-=-=-=-=-=-=-=-=-=-=-= */
/* -=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=- */

typedef PlayerInfo* (__thiscall* GetCurrentlyObservedPlayer_NativeCall)(GameNetMan* man);
GetCurrentlyObservedPlayer_NativeCall GetCurrentlyObservedPlayer_Native = (GetCurrentlyObservedPlayer_NativeCall)(0x004C64E0);

typedef PlayerInfo* (__thiscall* IsSurveying_NativeCall)(PlayerInfo* player);
IsSurveying_NativeCall IsSurveying_Native = (IsSurveying_NativeCall)(0x004DDD20);

bool(__thiscall* IsTrue)(CGameMode* skate_mod, uint32_t p_checksum) = reinterpret_cast <bool(__thiscall*)(CGameMode * skate_mod, uint32_t p_checksum)>(0x004E6A00);

typedef uint32_t __cdecl GetInt_NativeCall(uint32_t nameChecksum);
GetInt_NativeCall* GetInt_Native = (GetInt_NativeCall*)(0x004136A0);

typedef uint32_t (__thiscall* GetGamemode_NativeCall)(CGameMode* skate_mod);
GetGamemode_NativeCall GetGamemode_Native = (GetGamemode_NativeCall)(0x004E6A20);

typedef PlayerInfo* (__thiscall* GetLocalPlayer_NativeCall)(GameNetMan* man);
GetLocalPlayer_NativeCall GetLocalPlayer_Native = (GetLocalPlayer_NativeCall)(0x004CA9F0);

void* (__thiscall* GetLocalSkater_Native)(void* cGame) = reinterpret_cast <void* (__thiscall*)(void* cGame)>(0x004F94F0);

typedef void(__thiscall* CSkaterResetPhysics_NativeCall)(void* skater, bool walk, bool in_cleanup);
CSkaterResetPhysics_NativeCall CSkaterResetPhysics_Native = (CSkaterResetPhysics_NativeCall)(0x00518840);

typedef void* __cdecl Unkn_Func_NativeCall(void* a);
Unkn_Func_NativeCall* Unkn_Func_Native = (Unkn_Func_NativeCall*)(0x004FB780);

typedef uint32_t(__thiscall* ObservePlayer_NativeCall)(GameNetMan* man, PlayerInfo* pInfo);
ObservePlayer_NativeCall ObservePlayer_Native = (ObservePlayer_NativeCall)(0x004CE150);

typedef void(__thiscall* SetupObsInput_NativeCall)(void* ecx, uint32_t a0, uint32_t a1, void* a2, uint32_t a3);
SetupObsInput_NativeCall SetupObsInput_Native = (SetupObsInput_NativeCall)(0x004C7F20);

typedef Component* (__thiscall* GetComponent_NativeCall)(void* skater, uint32_t nameChecksum);
GetComponent_NativeCall GetComponent_Native = (GetComponent_NativeCall)(0x0041BDC0);


/* -=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=- */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-= Helper functions =-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
/* -=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=- */

void* GetNetManager() {
	return *(void**)0x0076914C;
}

void* get_gamemode_handle_wrapper(uint8_t* a) {
	// No singleton so we get the handle like this, gross
	if (!handle_received) {
		GameMode = GameMode = *(CGameMode**)(*(uint8_t**)(a + 0x1C) + 0x5C);
		handle_received = true;
	}
	return Unkn_Func_Native(a);
}

PlayerInfo* getskaterinfo() {
	uint32_t b_joinedasobserver = GetInt_Native(0x4724568A); //JoinedAsObserver
	uint32_t b_joinedgameinprogress = GetInt_Native(0x55001E9A); // JoinedGameInProgress
	uint32_t crc_gamemode = GetGamemode_Native(GameMode);
	bool firefight = IsTrue(GameMode, 0x3A952920 /*is_firefight*/);
	PlayerInfo* man = {};

	if (b_joinedasobserver || b_joinedgameinprogress) {
		Log::TypedLog(CHN_OBS, "JOINEDASOBSERVER or JOINEDGAMEINPROGRESS\n");
	}
	else {
		Log::TypedLog(CHN_OBS, "Getting local player info\n");
		man = GetLocalPlayer_Native((GameNetMan*)GetNetManager());
	}

	if (!man) {
		// Fallback?
		Log::TypedLog(CHN_OBS, "PlayerInfo was NULL\n");
	}
	return man;
}

void Obs_Helper_SetPlayerName(PlayerInfo* player) {

	Script::LazyStruct* name_struct = Script::LazyStruct::s_create();
	name_struct->AddString(0xC4745838 /*text*/, player->m_Name);
	name_struct->AddChecksum(0, 0x74297717); /*local*/

	RunScript(0x1FA767E4, name_struct, nullptr, nullptr); /*m_observe_menu_set_display_text*/

	name_struct->Clear();
	FreeQBStruct(name_struct);
}

PlayerInfo* GetNextPlayer(PlayerInfo* player) {

	if (!((uint32_t)player->NextSkater->id == -1))
		return player->NextSkater->id;

	if (!((uint32_t)player->NextSkater->localSkater->localPlayer == -1))
		return player->NextSkater->localSkater->localPlayer;

	return player;
}

PlayerInfo* GetPrevPlayer(PlayerInfo* player) {

	if (!((uint32_t)player->PrevSkater->id == -1))
		return player->PrevSkater->id;

	if (!((uint32_t)player->PrevSkater->localSkater2->localPlayer == -1))
		return player->PrevSkater->localSkater2->localPlayer;

	return player;
}

void* GetLocalSkater() {
	return GetLocalSkater_Native(*(void**)0x0076A788);
}

bool CFunc_ResetPhysics(Script::LazyStruct* pParams) {
	void* skater = GetLocalSkater();

	Log::TypedLog(CHN_DLL, "Resetting physics\n");

	if (pParams->ContainsFlag(0x726E85AA)) { /* walk */
		CSkaterResetPhysics_Native(skater, true, false);
	}
	else {
		CSkaterResetPhysics_Native(skater, false, false);
	}
	return true;
}

bool CFunc_IsSurveying(Script::LazyStruct* pParams) {
	uint32_t b_joinedasobserver = GetInt_Native(0x4724568A); //JoinedAsObserver
	uint32_t b_joinedgameinprogress = GetInt_Native(0x55001E9A); // JoinedGameInProgress

	if ((b_joinedasobserver == 0) && (b_joinedgameinprogress == 0)) {

		PlayerInfo* currentPlayer = GetLocalPlayer_Native((GameNetMan*)GetNetManager());
		Log::TypedLog(CHN_OBS, "LocalPlayer: 0x%08x\n", currentPlayer);
		if (currentPlayer) {
			Log::TypedLog(CHN_OBS, "IsSurveying: %d\n", IsSurveying_Native(currentPlayer));
			return IsSurveying_Native(currentPlayer);
		}
	}
	return false;
}

bool CFunc_UberFriggedThisFrame(Script::LazyStruct* pParams, DummyScript* pScript) {

	void* skater = GetLocalSkater();
	Component* mComp = GetComponent_Native(skater, 0x9F9CC949 /*SkaterAdjustPhysics*/);

	return (mComp->uberfrigged) ? true : false;
}

typedef float __cdecl CViewportManager_sSetScreenAngle_NativeCall(float fov);
CViewportManager_sSetScreenAngle_NativeCall* CViewportManager_sSetScreenAngle_Native = (CViewportManager_sSetScreenAngle_NativeCall*)(0x00485480);

bool CFunc_SetFOV(Script::LazyStruct* pParams) {

	float fov = 0.0;
	pParams->GetFloat(0x96715172, &fov, 0);

	if (!fov) {
		return false;
	}
	float fov_real = AdjustHorizontalFOV(fov, *(float*)0x00707860);
	CViewportManager_sSetScreenAngle_Native(fov_real);
	return true;
}


/* -=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=- */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=- Observe functions =-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
/* -=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=- */

bool Obs_ObserveSelf() {

	PlayerInfo* mInfo = getskaterinfo();

	if (!mInfo) {
		Log::TypedLog(CHN_OBS, "No Skater info to observe self\n");
		return 0;
	}

	Obs_Helper_SetPlayerName(mInfo);
	ObservePlayer_Native((GameNetMan*)GetNetManager(), mInfo);
	return false;
}

bool Obs_ObserveNext() {

	//Log::TypedLog(CHN_OBS, "Observing next player\n");
	GameNetMan* man = (GameNetMan*)GetNetManager();
	PlayerInfo* currentPlayer = GetCurrentlyObservedPlayer_Native(man);
	PlayerInfo* targetPlayer = nullptr;

	Log::TypedLog(CHN_OBS, "Trying to get next player: 0x%08x\n", (uint32_t)GetNextPlayer(currentPlayer));
	targetPlayer = GetNextPlayer(currentPlayer);

	if (currentPlayer) {
		for (int i = 0; i < 16; ++i) {

			targetPlayer = GetNextPlayer(currentPlayer);
			if (!targetPlayer) targetPlayer = getskaterinfo(); // Shouldn't happen

			if (targetPlayer->playerstate == observerOrPendingPlayerState) break;
			//Log::TypedLog(CHN_OBS, "Target Player: 0x%08x\n", targetPlayer);
		}
	}
	else {
		currentPlayer = getskaterinfo();
	}

	if (!(targetPlayer)) return 0;
	if (targetPlayer == currentPlayer) return 0;
	if (targetPlayer->playerstate == observerOrPendingPlayerState) return 0;

	Obs_Helper_SetPlayerName(targetPlayer);
	ObservePlayer_Native(man, targetPlayer);
	return true;
}

bool Obs_ObservePrev() {

	//Log::TypedLog(CHN_OBS, "Observing previous player\n");
	GameNetMan* man = (GameNetMan*)GetNetManager();
	PlayerInfo* currentPlayer = GetCurrentlyObservedPlayer_Native(man);
	PlayerInfo* targetPlayer = nullptr;

	Log::TypedLog(CHN_OBS, "Trying to get previous player: 0x%08x\n", (uint32_t)GetPrevPlayer(currentPlayer));
	targetPlayer = GetPrevPlayer(currentPlayer);

	if (currentPlayer) {
		for (int i = 0; i < 16; ++i) {

			targetPlayer = GetPrevPlayer(currentPlayer);
			if (!targetPlayer) targetPlayer = getskaterinfo(); // Shouldn't happen

			if (targetPlayer->playerstate == observerOrPendingPlayerState) break;
			//Log::TypedLog(CHN_OBS, "Target Player: 0x%08x\n", targetPlayer);
		}
	}
	else {
		currentPlayer = getskaterinfo();
	}

	if (!(targetPlayer)) return 0;
	if (targetPlayer == currentPlayer) return 0;
	if (targetPlayer->playerstate == observerOrPendingPlayerState) return 0;

	Obs_Helper_SetPlayerName(targetPlayer);
	ObservePlayer_Native(man, targetPlayer);
	return true;
}

bool Obs_ObserveLeader() {

	Log::TypedLog(CHN_OBS, "Observing leader\n");
	GameNetMan* man = (GameNetMan*)GetNetManager();
	PlayerInfo* currentPlayer = GetCurrentlyObservedPlayer_Native(man);

	if (!(man->observer))
		return false;

	Obs_Helper_SetPlayerName(currentPlayer);
	ObservePlayer_Native(man, currentPlayer);

	return true;
}

bool Obs_RefreshCamera() {

	Log::TypedLog(CHN_OBS, "Refreshing camera\n");
	GameNetMan* man = (GameNetMan*)GetNetManager();
	PlayerInfo* player = GetCurrentlyObservedPlayer_Native(man);

	if (!player) {
		player = getskaterinfo();
		
		if (!player)
			return 0;
	}

	Obs_Helper_SetPlayerName(player);
	ObservePlayer_Native(man, player);
	return false;
}

bool Obs_GetCurrentPlayerName(void* pParams, DummyScript* pScript) {

	GameNetMan* man = (GameNetMan*)*(uint32_t*)(0x0076914C);
	PlayerInfo* currPlayer = GetCurrentlyObservedPlayer_Native(man);

	if (!currPlayer) 
		playername = man->GetLocalPlayer->m_Name;
	else 
		playername = currPlayer->m_Name;

	if (man && currPlayer) {

		playername = currPlayer->m_Name;

		pScript->GetParams->AddString(0x03C6D1CA /*player_name*/, playername);
		return true;
	}

	pScript->GetParams->AddString(0x03C6D1CA /*player_name*/, playername);
	return false;
}

bool CFunc_ObserveFunctionCall(Script::LazyStruct* pParams, DummyScript* pScript) {

	uint32_t func = 0;
	pParams->GetChecksum(0xBD09544C, &func, 0); /*func*/

	if (func) {
		switch (func) {
		case 0x7E3298F3: /* ObserveSelf */
			Obs_ObserveSelf();
			break;
		case 0x04E1BA8E: /* ObservePrevPlayer */
			Obs_ObservePrev();
			break;
		case 0x850611BE: /* ObserveNextPlayer */
			Obs_ObserveNext();
			break;
		case 0xD6A37DC8: /* RefreshCamera */
			Obs_RefreshCamera();
			break;
		case 0x9890A1E7: /* GetCurrentPlayerName */
			Obs_GetCurrentPlayerName(pParams, pScript);
			break;
		case 0x64AAE435: /* ObserveLeader */
			Obs_ObserveLeader();
			break;
		default:
			Log::TypedLog(CHN_OBS, "Default\n");
			break;
		}
	}
	return true;
}

void addObserveCFuncs() {

	Log::TypedLog(CHN_DLL, "Adding observe CFuncs\n");

	CFuncs::AddFunction("M_ResetPhysics", CFunc_ResetPhysics);
	CFuncs::AddFunction("M_ObserveFunctionCall", CFunc_ObserveFunctionCall);
	CFuncs::AddFunction("M_IsSurveying", CFunc_IsSurveying);
	CFuncs::AddFunction("M_UberFriggedThisFrame", CFunc_UberFriggedThisFrame);
	CFuncs::AddFunction("M_SetFOV", CFunc_SetFOV);

	// Hook to get GameMode pointer
	patchDWord((void*)(0x004FB95F + 3), (uint32_t)&get_gamemode_handle_wrapper);

	// Skip selecting next skater on pad_choose
	patchByte((void*)0x004CF8FE, 0xEB);
}
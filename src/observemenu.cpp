#include "observemenu.h"

namespace Lst
{
	struct Search
	{
		int unk[5];
	};

	struct HashItem
	{
		uint32_t m_key;

		union
		{
			void* mp_value_void;
			int mp_value_int;
		};

		HashItem* mp_next;
	};

	struct HashTable
	{
		uint32_t m_numBits;
		HashItem* mp_hash_table;
		int m_size;
		int unkD;
		int unkE;
		bool b_UnkBoolA;
		bool b_UnkBoolB;
		bool b_UnkBoolC;
		int unkG;

		bool PutItem(const uint32_t* key, void* item, int unk);
		HashItem* GetItem(const uint32_t* key);
	};

	bool(__thiscall* HT_PutItem_Native)(HashTable* table, const uint32_t* key, void* item, int unk) = reinterpret_cast <bool(__thiscall*)(HashTable * table, const uint32_t * key, void* item, int unk)>(0x0049D470);
	bool HashTable::PutItem(const uint32_t* key, void* item, int unk)
	{
		return HT_PutItem_Native(this, key, item, unk);
	}

	HashItem* (__thiscall* HT_GetItem_Native)(HashTable* table, const uint32_t* key, int unused) = reinterpret_cast <HashItem * (__thiscall*)(HashTable * table, const uint32_t * key, int unused)>(0x004D0510);
	HashItem* HashTable::GetItem(const uint32_t* key)
	{
		return HT_GetItem_Native(this, key, 0);
	}
}

struct PlayerInfo {

	char unk1[100];

};

struct GameNetMan {
	char unk1[0xC0];
	PlayerInfo* GetLocalPlayer;
	PlayerInfo* GetServerPlayer;
	PlayerInfo* GetNextPlayerToObserve;
	PlayerInfo* GetCurrentlyObservedPlayer;
	PlayerInfo* GetKingOfTheHill;
	PlayerInfo* FirstNewPlayerInfo;
	PlayerInfo* NextNewPlayerInfo;
	PlayerInfo* GetNewPlayerInfoByObjectID;
};


void* GetNetManager()
{
	return *(void**)0x0076914C;
}

struct CGameMode {

	char unk[100];

};
Lst::Search infoSearch;

PlayerInfo* (__thiscall* GetCurrentlyObservedPlayer)(GameNetMan* man) = reinterpret_cast <PlayerInfo * (__thiscall*)(GameNetMan * man)>(0x004C64E0);


typedef uint32_t __cdecl GetInt_NativeCall(uint32_t nameChecksum);
GetInt_NativeCall* GetInt_Native = (GetInt_NativeCall*)(0x004136A0);

uint32_t(__thiscall* GetGamemode)(CGameMode* skate_mod) = reinterpret_cast <uint32_t(__thiscall*)(CGameMode * skate_mod)>(0x004E6A20);
bool(__thiscall* IsTrue)(CGameMode* skate_mod, uint32_t p_checksum) = reinterpret_cast <bool(__thiscall*)(CGameMode * skate_mod, uint32_t p_checksum)>(0x004E6A00);

typedef void* (__thiscall* FirstPlayerInfo_NativeCall)(void* manager, Lst::Search* search, bool include_observers);
FirstPlayerInfo_NativeCall FirstPlayerInfo_Native = (FirstPlayerInfo_NativeCall)(0x004CA900);

uint32_t getskaterinfo() {

	uint32_t b_joinedasobserver = GetInt_Native(0x4724568A); //JoinedAsObserver
	printf("b_joinedasobserver: 0x%08x\n", b_joinedasobserver);
	/*
	uint32_t b_joinedgameinprogress = GetInt(0x55001E9A); //JoinedGameInProgress

	bool flag1;
	bool firefight;

	if (b_joinedgameinprogress) flag1 = 1;

	CGameMode* skate_mod = (CGameMode*)*(uint32_t*)(0x0076A788);
	GameNetMan* man = (GameNetMan*)GetNetManager();

	GetGamemode(skate_mod);
	firefight = IsTrue(skate_mod, 0x3A952920);

	//0x3a952920 is_firefight

	*/
	return 1;
}


bool Obs_ObserveSelf() {

	getskaterinfo();
	Log::TypedLog(CHN_DLL, "Obs self\n");
	return 0;
}



void Obs_ObserveNext() {

	GameNetMan* man = (GameNetMan*)GetNetManager();
	//PlayerInfo* player = GetCurrentlyObservedPlayer(man);
	Log::TypedLog(CHN_DLL, "Man: 0x%08x\n", man);
	Log::TypedLog(CHN_DLL, "LocalPlayer: 0x%08x\n", man->GetLocalPlayer);
	Log::TypedLog(CHN_DLL, "ServerPlayer: 0x%08x\n", man->GetServerPlayer);
	Log::TypedLog(CHN_DLL, "GetCurrentlyObservedPlayer: 0x%08x\n", man->GetCurrentlyObservedPlayer);
	Log::TypedLog(CHN_DLL, "GetKingOfTheHill: 0x%08x\n", man->GetKingOfTheHill);
	//if (player)
	//	ObservePlayer_Native(man, player);
	Log::TypedLog(CHN_DLL, "obs next from DLL\n");


	//return true; Bool maybe

}

bool Obs_RefreshCamera() {
	GameNetMan* man = (GameNetMan*)GetNetManager();
	PlayerInfo* player = GetCurrentlyObservedPlayer(man);

	if (!player) {
		getskaterinfo();
		//something gamenet


	}
	printf("REFRESH CAMERA\n");
	return 0;
}

bool CFunc_ObserveFunctionCall(Script::LazyStruct* pParams) {

	uint32_t func = 0;
	pParams->GetChecksum(0xBD09544C, &func, 0); /* func */

	if (func) {
		switch (func) {
		case 0x7E3298F3: /* ObserveSelf */
			Obs_ObserveSelf();
			break;
		case 0x04E1BA8E: /* ObservePrevPlayer */
			//TODO
			break;
		case 0x850611BE: /* ObserveNextPlayer */
			Obs_ObserveNext();
			break;
		case 0xD6A37DC8: /* RefreshCamera */
			printf("Got RefreshCamera param\n");
			Obs_RefreshCamera();
			break;
		case 0x9890A1E7: /* GetCurrentPlayerName */
			//TODO
			break;
		case 0x64AAE435: /* ObserveLeader */
			//TODO
			break;
		case 0x1F7D3722: /* ObserveKing */
			//TODO
			break;
		case 0x83F64745: /* ObservePlayerByPlayerName */
			//TODO
			break;
		default:
			printf("nulln");
			break;
		}
	}
	return true;
}

void* (__thiscall* GetLocalSkater_Native)(void* cGame) = reinterpret_cast <void* (__thiscall*)(void* cGame)>(0x004F94F0);

typedef void(__thiscall* CSkaterResetPhysics_NativeCall)(void* skater, bool walk, bool in_cleanup);
CSkaterResetPhysics_NativeCall CSkaterResetPhysics_Native = (CSkaterResetPhysics_NativeCall)(0x00518840);

void* GetLocalSkater()
{
	return GetLocalSkater_Native(*(void**)0x0076A788);
}


bool CFunc_ResetPhysics(Script::LazyStruct* pParams) {

	Log::TypedLog(CHN_DLL, "######################## Resetting physics from DLL\n");
	void* skater = GetLocalSkater();
	if (pParams->ContainsFlag(0x726E85AA)) { /* walk */
		CSkaterResetPhysics_Native(skater, true, false);
	}
	else {
		CSkaterResetPhysics_Native(skater, false, false);
	}
	return true;
}

void addObserveCFuncs() {
	Log::TypedLog(CHN_DLL, "Adding CFuncs\n");
	//Add a call to this function in DllMain
	CFuncs::AddFunction("M_ResetPhysics", CFunc_ResetPhysics);
	CFuncs::AddFunction("M_ObserveFunctionCall", CFunc_ObserveFunctionCall);
}
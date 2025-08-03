#include "LazyStruct.h"
#include "malloc.h"
#include "script.h"


namespace Script {

	// --------------------------------------------

	typedef void(__thiscall* StructInitializeCall)(LazyStruct* struc);
	StructInitializeCall StructInitialize = (StructInitializeCall)(0x00430620);

	void LazyStruct::Initialize()
	{
		StructInitialize(this);
	}

	// --------------------------------------------

	typedef void(__thiscall* Clear_NativeCall)(LazyStruct* struc);
	Clear_NativeCall Clear_Native = (Clear_NativeCall)(0x00411820);

	void LazyStruct::Clear()
	{
		Clear_Native(this);
	}

	// --------------------------------------------

	typedef void (__thiscall* Free_NativeCall)(LazyStruct* struc);
	Free_NativeCall Free_Native = (Free_NativeCall)(0x00411790);

	void LazyStruct::Free()
	{
		Free_Native(this);
	}

	//---------------------------------------
	// Create a struct!
	//---------------------------------------

	LazyStruct* LazyStruct::s_create()
	{
		LazyStruct* result = (LazyStruct*)qbMalloc(sizeof(LazyStruct));

		if (!result)
		{
			//Logger::Error("LazyStruct::s_create() failed. Memory error?");
			return nullptr;
		}

		result->Initialize();
		return result;
	}

	//---------------------------------------
	// Free a struct!
	//---------------------------------------

	void LazyStruct::s_free(LazyStruct* to_free)
	{
		to_free->Free();
		FreeQBStruct(to_free);
	}

	// --------------------------------------------

	LazyStructItem* LazyStruct::GetItem(uint32_t qbKey)
	{
		LazyStructItem* item = this->first;
		printf("LazyStructItem first: 0x%08x\n", item);

		while (item != nullptr)
		{
			if (item->key == qbKey)
				return item;

			item = item->next;
		}

		return nullptr;
	}

	LazyStructItem* LazyStruct::LastItem()
	{
		LazyStructItem* last_item = this->first;
		if (!last_item)
			return nullptr;

		LazyStructItem* next_item = last_item->next;

		while (next_item)
		{
			last_item = next_item;
			next_item = (last_item->next);
		}

		return last_item;
	}

	// Adds an item of type
	LazyStructItem* LazyStruct::AddItem(uint32_t qbKey)
	{
		// Item didn't exist in the stack, let's make it
		// (In a moment, we'll decide what to parent it to)

		LazyStructItem* new_item = (LazyStructItem*)qbItemMalloc(sizeof(LazyStructItem), 1);

		if (!new_item)
			return nullptr;

		new_item->flags = 0;
		new_item->itemType = 0;
		//new_item->unkByteA = 0x0D;
		new_item->key = 0;
		new_item->next = 0;
		new_item->value = 0;
		new_item->key = qbKey;

		// Get the last item in our stack, add a new one to it

		LazyStructItem* last_item = LastItem();

		if (last_item)
			last_item->next = new_item;

		// No items, this is our first!

		else
			first = new_item;

		return new_item;
	}

	// Gets an item if it exists, adds one if not
	LazyStructItem* LazyStruct::GetOrAdd(uint32_t qbKey)
	{
		LazyStructItem* found_item = GetItem(qbKey);

		if (found_item)
			return found_item;

		found_item = AddItem(qbKey);

		return found_item;
	}

	typedef bool (__thiscall* Contains_NativeCall)(LazyStruct* struc, uint32_t item_name);
	Contains_NativeCall Contains_Native = (Contains_NativeCall)(0x00411220);

	// See if we contain a valueless checksum
	bool LazyStruct::Contains(uint32_t qbKey)
	{
		return Contains_Native(this, qbKey) || ContainsFlag(qbKey);
	}

	typedef bool (__thiscall* ContainsFlag_NativeCall)(LazyStruct* struc, uint32_t flag);
	ContainsFlag_NativeCall ContainsFlag_Native = (ContainsFlag_NativeCall)(0x004112A0);

	// Contains a flag?
	bool LazyStruct::ContainsFlag(uint32_t qbKey) { return ContainsFlag_Native(this, qbKey); }

	typedef void __fastcall AddStringCall(LazyStruct* struc, int edx, uint32_t qbKey, char* value);
	AddStringCall* AddString_Native = (AddStringCall*)(0x00412080);

	void LazyStruct::AddString(uint32_t qbKey, char* value)
	{
		AddString_Native(this, 0, qbKey, value);
	}

	typedef void (__thiscall* AddStructure_NativeCall)(LazyStruct* struc, uint32_t qbKey, LazyStruct* value);
	AddStructure_NativeCall AddStructure_Native = (AddStructure_NativeCall)(0x00412D90);

	void LazyStruct::AddStructure(uint32_t qbKey, LazyStruct* value)
	{
		AddStructure_Native(this, qbKey, value);
	}

	void LazyStruct::RedefineArrayItem(uint32_t qbKey, void* value)
	{
		LazyStructItem* itm = GetItem(qbKey);

		if (itm)
			itm->value = (uint32_t)value;
	}

	//---------------------------------------
	// Add structure pointer
	//---------------------------------------

	typedef void(__thiscall* AddStructurePointer_NativeCall)(LazyStruct* struc, uint32_t nameChecksum, LazyStruct* p_structure);
	AddStructurePointer_NativeCall AddStructurePointer_Native = (AddStructurePointer_NativeCall)(0x00412890);

	void LazyStruct::AddStructurePointer(uint32_t nameChecksum, LazyStruct* p_structure)
	{
		AddStructurePointer_Native(this, nameChecksum, p_structure);
	}
	

	//---------------------------------------
	// Get integer item
	//---------------------------------------

	int LazyStruct::GetInteger(uint32_t qbKey) // 0x00413550
	{
		LazyStructItem* item = GetItem(qbKey);
		if (!item)
			return 0;

		return item->value;
	}

	//---------------------------------------
	// Get float item
	//---------------------------------------

	typedef bool(__thiscall* GetFloat_NativeCall)(LazyStruct* struc, uint32_t checksum, float* p_checksum, bool assert);
	GetFloat_NativeCall GetFloat_Native = (GetFloat_NativeCall)(0x00410E20);
	
	float LazyStruct::GetFloat(uint32_t checksum, float* p_checksum, bool assert)
	{
		return GetFloat_Native(this, checksum, p_checksum, assert);
	}

	//---------------------------------------
	// Get checksum item
	//---------------------------------------

	typedef bool (__thiscall* GetChecksum_NativeCall)(LazyStruct* struc, uint32_t checksum, uint32_t *p_checksum, bool assert);
	GetChecksum_NativeCall GetChecksum_Native = (GetChecksum_NativeCall)(0x004110B0);

	bool LazyStruct::GetChecksum(uint32_t checksum, uint32_t *p_checksum, bool assert)
	{
		return GetChecksum_Native(this, checksum, p_checksum, assert);
	}

	//---------------------------------------
	// Get text
	//---------------------------------------

	typedef bool(__thiscall* GetText_NativeCall)(LazyStruct* struc, uint32_t checksum, const char** pp_text, bool assert);
	GetText_NativeCall GetText_Native = (GetText_NativeCall)(0x00410CE0);

	bool LazyStruct::GetText(uint32_t checksum, const char** pp_text, bool assert)
	{
		return GetText_Native(this, checksum, pp_text, assert);
	}

	//---------------------------------------
	// Add integer
	//---------------------------------------

	typedef void (__thiscall* AddInteger_NativeCall)(LazyStruct* struc, uint32_t checksum, int value);
	AddInteger_NativeCall AddInteger_Native = (AddInteger_NativeCall)(0x00412230);

	void LazyStruct::AddInteger(uint32_t checksum, int value)
	{
		AddInteger_Native(this, checksum, value);
	}
	
	//---------------------------------------
	// Add float
	//---------------------------------------

	typedef void (__thiscall* AddFloat_NativeCall)(LazyStruct* struc, uint32_t nameChecksum, float float_val);
	AddFloat_NativeCall AddFloat_Native = (AddFloat_NativeCall)(0x00412310);

	void LazyStruct::AddFloat(uint32_t nameChecksum, float float_val)
	{
		AddFloat_Native(this, nameChecksum, float_val);
	}

	//---------------------------------------
	// Add pair
	//---------------------------------------
	
	typedef void (__thiscall* AddPair_NativeCall)(LazyStruct* struc, uint32_t nameChecksum, float x, float y);
	AddPair_NativeCall AddPair_Native = (AddPair_NativeCall)(0x00412780); //maybe

	void LazyStruct::AddPair(uint32_t nameChecksum, float x, float y)
	{
		AddPair_Native(this, nameChecksum, x, y);
	}

	//---------------------------------------
	// Add checksum
	//---------------------------------------

	typedef void (__thiscall* AddChecksum_NativeCall)(LazyStruct* struc, uint32_t nameChecksum, uint32_t checksum);
	AddChecksum_NativeCall AddChecksum_Native = (AddChecksum_NativeCall)(0x004123F0);

	void LazyStruct::AddChecksum(uint32_t nameChecksum, uint32_t checksum)
	{
		AddChecksum_Native(this, nameChecksum, checksum);
	}

	//---------------------------------------
	// Append data from another structure
	//---------------------------------------

	typedef void (__thiscall* AppendStructure_NativeCall)(LazyStruct* struc, const LazyStruct *p_struct);
	AppendStructure_NativeCall AppendStructure_Native = (AppendStructure_NativeCall)(0x00412C50);

	void LazyStruct::AppendStructure(const LazyStruct *p_struct)
	{
		AppendStructure_Native(this, p_struct);
	}

	//---------------------------------------
	// Add pointer to an array
	//---------------------------------------

	typedef void (__thiscall* AddArrayPointer_NativeCall)(LazyStruct* struc, uint32_t nameChecksum, void *p_array);
	AddArrayPointer_NativeCall AddArrayPointer_Native = (AddArrayPointer_NativeCall)(0x004127B0);

	void LazyStruct::AddArrayPointer(uint32_t nameChecksum, void *p_array)
	{
		AddArrayPointer_Native(this, nameChecksum, p_array);
	}

	//---------------------------------------
	// Creates a new array & copies in the contents of the passed array.
	//---------------------------------------

	typedef void(__thiscall* AddArray_NativeCall)(LazyStruct* struc, uint32_t nameChecksum, const Script::LazyArray* p_array);
	AddArray_NativeCall AddArray_Native = (AddArray_NativeCall)(0x004131D0);

	void LazyStruct::AddArray(uint32_t nameChecksum, const Script::LazyArray* p_array)
	{
		AddArray_Native(this, nameChecksum, p_array);
	}
	
	//---------------------------------------
	// Get array
	//---------------------------------------


	bool(__thiscall* GetArray_Native)(LazyStruct* struc, uint32_t qbKey, LazyArray** out_val, int assert) = reinterpret_cast <bool(__thiscall*)(LazyStruct * struc, uint32_t qbKey, LazyArray * *out_val, int assert)>(0x00410FC0);

	bool LazyStruct::GetArray(uint32_t nameChecksum, Script::LazyArray** pp_array)
	{
		return GetArray_Native(this, nameChecksum, pp_array, 0);
	}

	//---------------------------------------
	// Get string item
	//---------------------------------------

	char* LazyStruct::GetString(uint32_t qbKey)
	{
		LazyStructItem* item = GetItem(qbKey);

		if (item && ((item->itemType >> 1) == ESYMBOLTYPE_STRING))
			return (char*)item->value;

		return nullptr;
	}

	//---------------------------------------
	// Get structure item
	//---------------------------------------

	LazyStruct* LazyStruct::GetStruct(uint32_t qbKey)
	{
		LazyStructItem* item = GetItem(qbKey);
		if (!item)
			return nullptr;

		return (LazyStruct*)item->value;
	}
}
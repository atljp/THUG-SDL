#include "CFuncs.h"

namespace CFuncs
{
	char			cf_names[STOCK_CFUNC_COUNT + CUSTOM_CFUNC_COUNT][128];
	CFunc 			cf_functions[STOCK_CFUNC_COUNT + CUSTOM_CFUNC_COUNT];
	uint32_t 		cf_count = STOCK_CFUNC_COUNT;

	// ---------------------------------------
	// Number of functions we have
	// ---------------------------------------

	uint32_t NumberOfFuncs()
	{
		return cf_count;
	}

	// ---------------------------------------
	// Find a function by name
	// ---------------------------------------

	CFunc* FindFunction(const char* func_name)
	{
		for (int i = 0; i < NumberOfFuncs(); i++)
		{
			if (_stricmp(cf_functions[i].id, func_name) == 0)
				return (CFunc*)&cf_functions[i];
		}

		return nullptr;
	}

	// ---------------------------------------
	// Redirect a function
	// ---------------------------------------

	void RedirectFunction(const char* func_name, void* new_offset)
	{
		CFunc* func = FindFunction(func_name);
		if (func)
			func->offset = new_offset;
	}

	// ---------------------------------------
	// Pointer functions for patching
	// ---------------------------------------

	uint32_t Pointer_Functions() { return (uint32_t)&cf_functions; }
	uint32_t Pointer_FunctionCount() { return (uint32_t)&NumberOfFuncs; }

	// ---------------------------------------
	// Copies built-in functions to our list
	// ---------------------------------------

	void CopyStockFunctions()
	{
		// Copy list of game's stock CFuncs
		for (int i = 0; i < STOCK_CFUNC_COUNT; i++)
		{
			uint32_t cfunc_id_off = *(uint32_t*)(0x00686AC0 + (8 * i));
			uint32_t cfunc_sub_off = *(uint32_t*)(0x00686AC0 + (8 * i) + 4);

			char* stock_name = (char*)cfunc_id_off;
			memcpy((void*)cf_names[i], (void*)stock_name, strlen(stock_name) + 1);

			cf_functions[i].id = (char*)cf_names[i];
			cf_functions[i].offset = (void*)cfunc_sub_off;
		}
	}

	// ---------------------------------------
	// Add a CFunc
	// ---------------------------------------

	void AddFunction(const char* func_id, void* func_offset)
	{
		memcpy((void*)cf_names[cf_count], (void*)func_id, strlen(func_id) + 1);

		cf_functions[cf_count].id = (char*)cf_names[cf_count];
		cf_functions[cf_count].offset = func_offset;
		cf_count++;
	}

}
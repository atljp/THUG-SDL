#pragma once

#include <string.h>
#include <cstdint>

#define STOCK_CFUNC_COUNT			1144
#define CUSTOM_CFUNC_COUNT			512

namespace CFuncs
{
	struct CFunc
	{
		public:
			char* id;
			void* offset;
	};

	uint32_t NumberOfFuncs();

	CFunc* FindFunction(const char* func_name);
	void RedirectFunction(const char* func_name, void* new_offset);

	uint32_t Pointer_Functions();
	uint32_t Pointer_FunctionCount();

	void CopyStockFunctions();
	void AddFunction(const char* func_id, void* func_offset);
}


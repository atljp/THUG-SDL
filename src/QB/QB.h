//------------------------------------------------
// Contains generic QB functions.
//------------------------------------------------

#pragma once

#include <cstdint>
#include <QB/LazyStruct.h>
#include <script.h>

bool RunScript(uint32_t name, Script::LazyStruct* params, void* object, void* object2);

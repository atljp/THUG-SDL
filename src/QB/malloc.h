 #pragma once
// Various memory allocation calls used by classes in the game. Since the game's memory management is different than ours, any game class created
// that uses dynamic memory allocation should be using these calls so that the game cleans them up properly.

// mallocVariant1
static void * (__cdecl * const qbItemMalloc)(size_t size, int unused) = reinterpret_cast<void * (__cdecl *)(size_t size, int unused)>(0x00483410); // ??? ghwt or thaw

// Used for qbstructs
static void * (__cdecl * const qbMalloc)(size_t size) = reinterpret_cast<void * (__cdecl *)(size_t size)>(0x00408960); /* returns struct pointer */
    
// Used for arrays
static void* (__cdecl* const qbArrayMalloc)(size_t size, int unused) = reinterpret_cast<void* (__cdecl*)(size_t size, int unused)>(0x0040B140);

// For freeing structures
static void (__cdecl * const FreeQBStruct)(void *memNode) = reinterpret_cast<void (__cdecl *)(void *memNode)>(0x004089A0);
    
// For freeing arrays
static void (__cdecl * const FreeQBArray)(void *memNode) = reinterpret_cast<void (__cdecl *)(void *memNode)>(0x0040B180);


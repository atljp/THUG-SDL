#include "QbKey.h"

namespace Script
{
    typedef uint32_t __cdecl ExtendCRC_NativeCall(uint32_t rc, const char* pName);
    ExtendCRC_NativeCall* ExtendCRC_Native = (ExtendCRC_NativeCall*)(0x00401970); // or 0x00401B90

    uint32_t ExtendCRC(uint32_t rc, char* pName) { return ExtendCRC_Native(rc, pName); }
    uint32_t ExtendCRC(uint32_t rc, const char* pName) { return ExtendCRC_Native(rc, pName); }
}

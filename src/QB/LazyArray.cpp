#include <QB/LazyArray.h>
#include <QB/malloc.h>

namespace Script
{
    // ----------------------------
    // Cleanup array
    // ----------------------------

    typedef void __cdecl CleanupArray_NativeCall(LazyArray* arr);
    CleanupArray_NativeCall* CleanupArray_Native = (CleanupArray_NativeCall*)(0x00411830);

    void CleanupArray(LazyArray* arr)
    {
        CleanupArray_Native(arr);
    }

    // ----------------------------
    // Create new array from pool
    // ----------------------------

    LazyArray* LazyArray::s_create()
    {
        LazyArray* result = (LazyArray*)qbArrayMalloc(sizeof(LazyArray), 1);

        if (!result)
        {
            return nullptr;
        }

        result->Initialize();
        return result;
    }

    // ----------------------------
    // Purge and cleanup an array
    // ----------------------------

    void LazyArray::s_free(LazyArray* arr)
    {
        if (!arr)
            return;

        // Calling this will call Clear in the array.
        Script::CleanupArray(arr);
        FreeQBArray(arr);
    }

    // ----------------------------
    // Initialize the array
    // ----------------------------

    typedef void(__thiscall* ArrayInitializeCall)(LazyArray* arr);
    ArrayInitializeCall ArrayInitialize = (ArrayInitializeCall)(0x00408200);

    void LazyArray::Initialize()
    {
        m_union = 0;
        type = 0;
        length = 0;

        ArrayInitialize(this);
    }

    // ----------------------------
    // Clear the array
    // ----------------------------

    typedef void(__thiscall* Clear_NativeCall)(LazyArray* arr);
    Clear_NativeCall Clear_Native = (Clear_NativeCall)(0x00408220);

    void LazyArray::Clear() {
        Clear_Native(this);
    }

    // ----------------------------
    // Set array size and type
    // ----------------------------

    typedef void(__thiscall* SetSizeAndType_NativeCall)(LazyArray* arr, int size, uint8_t type);
    SetSizeAndType_NativeCall SetSizeAndType_Native = (SetSizeAndType_NativeCall)(0x00408370);

    void LazyArray::SetSizeAndType(int size, uint8_t type)
    {
        SetSizeAndType_Native(this, size, type);
    }

    // ----------------------------
    // Set structure
    // ----------------------------

    typedef void(__thiscall* SetStructure_NativeCall)(LazyArray* arr, uint32_t index, Script::LazyStruct* p_struct);
    SetStructure_NativeCall SetStructure_Native = (SetStructure_NativeCall)(0x00408290);

    void LazyArray::SetStructure(uint32_t index, Script::LazyStruct* p_struct)
    {
        SetStructure_Native(this, index, p_struct);
    }

    // ----------------------------
    // Set checksum
    // ----------------------------

    void LazyArray::SetChecksum(uint32_t index, uint32_t checksum)
    {
        if (length == 1)
            m_checksum = checksum;
        else
            mp_checksums[index] = checksum;
    }

    // ----------------------------
    // Get integer
    // ----------------------------

    typedef uint32_t(__thiscall* GetInteger_NativeCall)(LazyArray* arr, uint32_t index);
    GetInteger_NativeCall GetInteger_Native = (GetInteger_NativeCall)(0x00408350);

    uint32_t LazyArray::GetInteger(uint32_t index)
    {
        return GetInteger_Native(this, index);
    }

    // ----------------------------
    // Set array
    // ----------------------------

    void LazyArray::SetArray(uint32_t index, LazyArray* value)
    {
        if (length == 1)
            mp_array = value;
        else
            mpp_arrays[index] = value;
    }

    // ----------------------------
    // Get float
    // ----------------------------

    float LazyArray::GetFloat(int index) {
        if (length == 1)
            return m_float;
        else
            return mp_floats[index];
    }

    // ----------------------------
    // Get checksum
    // ----------------------------

    uint32_t LazyArray::GetChecksum(int index) {
        if (length == 1)
            return  m_checksum;
        else
            return mp_checksums[index];
    }
         
    // ----------------------------
    // Get array
    // ----------------------------

    Script::LazyArray* LazyArray::GetArray(int index) { 
        if (length == 1)
            return mp_array;
        else
            return mpp_arrays[index];
     }
           
    // ----------------------------
    // Get integer
    // ----------------------------

    int LazyArray::GetInteger(int index) {
        if (length == 1)
            return m_integer;
        else
            return mp_integers[index];
    }

    // ----------------------------
    // Get structure
    // ----------------------------

    Script::LazyStruct* LazyArray::GetStructure(int index) {
        if (length == 1)
            return mp_structure;
        else
            return  mpp_structures[index];
    }
}


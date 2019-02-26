#include <cstddef>
#include <cstdint>
#include <new>

// EASTL requires you to have an overload for the operator new[], here is an example that just forwards to global new[]
// see https://github.com/electronicarts/EASTL/blob/master/doc/CMake/EASTL_Project_Integration.md#setting-up-your-code
void* operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line) {
    return new uint8_t[size];
}

void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file,
                     int line) {
    return new uint8_t[size];
}
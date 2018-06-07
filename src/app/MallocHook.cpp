#include "MallocHook.h"

#include <dlfcn.h>
#include <stdio.h>

#include <algorithm>
#include <atomic>

std::atomic<uint64_t> sAllocatedMem(0);

extern "C" {

void* malloc(size_t size) noexcept {
	static void* (*real_malloc)(size_t) = (void* (*)(size_t))dlsym(RTLD_NEXT, "malloc");

	size_t const estimatedRequiredBytes = (std::max)(MinAlloc, ((size + Alignment - 1) / Alignment) * Alignment) + Overhead;
	sAllocatedMem += estimatedRequiredBytes;

	void* data = real_malloc(Alignment + size);
	// fprintf(stderr, "%zu %zu\n", estimatedRequiredBytes, size);

	*reinterpret_cast<uint64_t*>(data) = estimatedRequiredBytes;
	// safety check
	*(reinterpret_cast<uint64_t*>(data) + 1) = 0xDEADBEEFbadc0ded;
	return reinterpret_cast<char*>(data) + Alignment;
}

void free(void* ptr) noexcept {
	static void (*real_free)(void*) = (void (*)(void*))dlsym(RTLD_NEXT, "free");

	if (!ptr) {
		return;
	}

	void* data = reinterpret_cast<char*>(ptr) - Alignment;
	if (*(reinterpret_cast<uint64_t*>(data) + 1) == 0xDEADBEEFbadc0ded) {
		size_t estimatedRequiredBytes = *reinterpret_cast<size_t*>(data);
		sAllocatedMem -= estimatedRequiredBytes;
	}
	real_free(data);
}
}

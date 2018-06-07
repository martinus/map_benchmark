#pragma once

#include <dlfcn.h>
#include <stdio.h>

#include <algorithm>
#include <atomic>

extern std::atomic<size_t> sAllocatedMem;

static constexpr size_t Alignment = 16;
static constexpr size_t Overhead = 8;
static constexpr size_t MinAlloc = 32;

extern "C" {
void* malloc(size_t size) noexcept;
void free(void* ptr) noexcept;
}

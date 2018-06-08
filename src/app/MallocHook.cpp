/******************************************************************************
 * malloc_count.c
 *
 * malloc() allocation counter based on http://ozlabs.org/~jk/code/ and other
 * code preparing LD_PRELOAD shared objects.
 *
 ******************************************************************************
 * Copyright (C) 2013-2014 Timo Bingmann <tb@panthema.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *****************************************************************************/

#define NO_HOOK

#ifdef NO_HOOK

#include "MallocHook.h"
#include <stdio.h>

extern size_t malloc_count_current(void) {
	return 0;
}
extern size_t malloc_count_peak(void) {
	return 0;
}
extern void malloc_count_reset_peak(void) {}

#else

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <dlfcn.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <atomic>

#include "MallocHook.h"

/* function pointer to the real procedures, loaded using dlsym */
typedef void* (*malloc_type)(size_t);
typedef void (*free_type)(void*);
typedef void* (*realloc_type)(void*, size_t);

static malloc_type real_malloc = NULL;
static free_type real_free = NULL;
static realloc_type real_realloc = NULL;

/* a simple memory heap for allocations prior to dlsym loading */
#define INIT_HEAP_SIZE 1024 * 1024
static char init_heap[INIT_HEAP_SIZE];
static size_t init_heap_use = 0;

std::atomic<size_t> sMemCurrent(0);
std::atomic<size_t> sMemPeak(0);

/* user function to return the currently allocated amount of memory */
extern size_t malloc_count_current(void) {
	return sMemCurrent;
}

/* user function to return the peak allocation */
extern size_t malloc_count_peak(void) {
	return sMemPeak;
}

static constexpr size_t Alignment = 16;
static constexpr size_t Overhead = 8;
static constexpr size_t MinAlloc = 32;

size_t estimateAllocBytes(size_t size) {
	return Overhead + (std::max)(MinAlloc, ((size + Alignment - 1) / Alignment) * Alignment);
}

/* exported malloc symbol that overrides loading from libc */
extern void* malloc(size_t size) {
	void* ret;

	if (size == 0)
		return nullptr;

	if (real_malloc) {
		ret = (*real_malloc)(sizeof(size_t) + size);
	} else {
		if (init_heap_use + sizeof(size_t) + size > INIT_HEAP_SIZE) {
			fprintf(stderr, "init heap full !!!\n");
			exit(1);
		}

		ret = init_heap + init_heap_use;
		init_heap_use += sizeof(size_t) + size;
	}

	// prepend allocation size
	size_t const estimatedBytes = estimateAllocBytes(size);

	*(size_t*)ret = estimatedBytes;
	sMemCurrent += estimatedBytes;

	return (char*)ret + sizeof(size_t);
}

/* exported free symbol that overrides loading from libc */
extern void free(void* ptr) {
	size_t size;

	if (!ptr)
		return; /* free(NULL) is no operation */

	if ((char*)ptr >= init_heap && (char*)ptr <= init_heap + init_heap_use) {
		// using init heap, don't do anything
		return;
	}

	if (!real_free) {
		fprintf(stderr, "free(%p) outside init heap and without real_free !!!\n", ptr);
		return;
	}

	ptr = (char*)ptr - sizeof(size_t);

	sMemCurrent -= *(size_t*)ptr;

	(*real_free)(ptr);
}

/* exported calloc() symbol that overrides loading from libc, implemented using
 * our malloc */
extern void* calloc(size_t nmemb, size_t size) {
	void* ret;
	size *= nmemb;
	if (!size)
		return NULL;
	ret = malloc(size);
	memset(ret, 0, size);
	return ret;
}

/* exported realloc() symbol that overrides loading from libc */
extern void* realloc(void* ptr, size_t size) {
	if ((char*)ptr >= (char*)init_heap && (char*)ptr <= (char*)init_heap + init_heap_use) {
		// realloc on init heap

		ptr = (char*)ptr - sizeof(size_t);
		size_t oldsize = *(size_t*)ptr;

		if (oldsize >= size) {
			/* keep old area, just reduce the size */
			*(size_t*)ptr = size;
			return (char*)ptr + sizeof(size_t);
		} else {
			/* allocate new area and copy data */
			ptr = (char*)ptr + sizeof(size_t);
			void* newptr = malloc(size);
			memcpy(newptr, ptr, oldsize);
			// free on init_heap, does not do anything
			free(ptr);
			return newptr;
		}
	}

	if (size == 0) {
		// special case size == 0 -> free()
		free(ptr);
		return nullptr;
	}

	if (ptr == nullptr) {
		// special case ptr == 0 -> malloc()
		return malloc(size);
	}

	ptr = (char*)ptr - sizeof(size_t);

	size_t const estimatedOldSize = *(size_t*)ptr;
	size_t const estimatedNewSize = estimateAllocBytes(size);
	sMemCurrent += estimatedNewSize - estimatedOldSize;

	void* newptr = (*real_realloc)(ptr, sizeof(size_t) + size);

	*(size_t*)newptr = estimatedNewSize;

	return (char*)newptr + sizeof(size_t);
}

static __attribute__((constructor)) void init(void) {
	char* error;

	dlerror();

	real_malloc = (malloc_type)dlsym(RTLD_NEXT, "malloc");
	if (error = dlerror()) {
		fprintf(stderr, "error %s\n", error);
		exit(1);
	}

	real_realloc = (realloc_type)dlsym(RTLD_NEXT, "realloc");
	if (error = dlerror()) {
		fprintf(stderr, "error %s\n", error);
		exit(1);
	}

	real_free = (free_type)dlsym(RTLD_NEXT, "free");
	if (error = dlerror()) {
		fprintf(stderr, "error %s\n", error);
		exit(1);
	}
}

#endif
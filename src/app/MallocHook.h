#pragma once

#include <stdlib.h>

extern "C" {
extern size_t malloc_count_current(void);
extern size_t malloc_count_peak(void);
extern void malloc_count_reset_peak(void);
}

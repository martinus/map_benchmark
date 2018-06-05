#include "bench.h"

static void InternalGetMem(Bench &bench) {
    /*
    bench.title("InternalGetMem");

    bench.beginMeasure();
    size_t r = 0;
    for (size_t i=0; i<100'000; ++i) {
        r += Profiler::get_mem();
    }
    bench.endMeasure();

    // result map status
    bench.result(r);
    */
}

static BenchRegister reg(InternalGetMem);
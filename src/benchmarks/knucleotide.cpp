// This code is adapted from Jeremy Zerfas [0] submission to the
// "The Computer Language Benchmarks Game" [1], the appropriate license is at
// the bottom of the file.
//
// [0] https://benchmarksgame-team.pages.debian.net/benchmarksgame/program/knucleotide-clang-1.html
// [1] https://salsa.debian.org/benchmarksgame-team/benchmarksgame/


#include <vector>
#include <cstdint>

#include "Map.h"
#include "bench.h"


// Macro to convert a nucleotide character to a code. Note that upper and lower
// case ASCII letters only differ in the fifth bit from the right and we only
// need the three least significant bits to differentiate the letters 'A', 'C',
// 'G', and 'T'. Spaces in this array/string will never be used as long as
// characters other than 'A', 'C', 'G', and 'T' aren't used.
#define CODE_FOR_NUCLEOTIDE(nucleotide) (" \0 \1\3  \2"[nucleotide & 0x7])


static size_t count(const std::vector<char> &poly, const std::string &oligo) {

    using M = Map<uint64_t, uint32_t>;
#ifdef USE_POOL_ALLOCATOR
    Resource<uint64_t, uint32_t> resource;
    M map{0, M::hasher{}, M::key_equal{}, &resource};
#else
    M map;
#endif

    uint64_t key = 0;
    const uint64_t mask = ((uint64_t)1 << 2 * oligo.size()) - 1;

    // For the first several nucleotides we only need to append them to key in
    // preparation for the insertion of complete oligonucleotides to map.
    for (size_t i = 0; i < oligo.size() - 1; ++i)
        key = (key << 2 & mask) | poly[i];

    // Add all the complete oligonucleotides of oligo.size() to
    // map and update the count for each oligonucleotide.
    for (size_t i = oligo.size() - 1; i < poly.size(); ++i){
        key= (key << 2 & mask) | poly[i];
        ++map[key];
    }

    // Generate the key for oligonucleotide.
    key = 0;
    for (size_t i = 0; i < oligo.size(); ++i) {
        key = (key << 2) | CODE_FOR_NUCLEOTIDE(oligo[i]);
    }

    // Output the count for oligonucleotide to output.
    return map.find(key)->second;
}

static inline int fasta_next() {
    static constexpr int IM = 139968, IA = 3877, IC = 29573;
    static int state = 42;

    state = (state * IA + IC) % IM;
    float p = state * (1.0f / IM);
    return (p >= 0.3029549426680f) + (p >= 0.5009432431601f) + (p >= 0.6984905497992f);
}


BENCHMARK(knucleotide) {
    constexpr size_t n = 25000000;

    for (size_t i = 0; i < n * 3; ++i)
        (void)fasta_next();

    std::vector<char> poly(n * 5);
    for (size_t i = 0; i < poly.size(); ++i) {
        poly[i] = fasta_next();
    }

    bench.beginMeasure("Count GGTATTTTAATTTATAGT");
    bench.endMeasure(893, count(poly, "GGTATTTTAATTTATAGT"));

    bench.beginMeasure("Count GGTATTTTAATT");
    bench.endMeasure(893, count(poly, "GGTATTTTAATT"));

    bench.beginMeasure("Count GGTATT");
    bench.endMeasure(47336, count(poly, "GGTATT"));

    bench.beginMeasure("Count GGTA");
    bench.endMeasure(446535, count(poly, "GGTA"));

    bench.beginMeasure("Count GGT");
    bench.endMeasure(1471758, count(poly, "GGT"));
}


// Copyright © 2004-2008 Brent Fulgham, 2005-2023 Isaac Gouy All rights
// reserved. Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//
// Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
//
//
// Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
//
// Neither the name "The Computer Language Benchmarks Game" nor the name "The
// Benchmarks Game" nor the name "The Computer Language Shootout Benchmarks"
// nor the names of its contributors may be used to endorse or promote products
// derived from this software without specific prior written permission.
//
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.


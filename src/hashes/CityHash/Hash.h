#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

static const char* HashName = "CityHash";

// absl::Hash makes use of CityHash
struct CityHashSeeder {
	static const void* const kSeed;
};

template <typename T>
struct Hash {
	static constexpr uint64_t kMul = sizeof(size_t) == 4 ? uint64_t{0xcc9e2d51} : uint64_t{0x9ddfea08eb382d69};

	static uint64_t Seed() {
		return static_cast<uint64_t>(reinterpret_cast<uintptr_t>(CityHashSeeder::kSeed));
	}

	static uint64_t Mix(uint64_t state, uint64_t v) {
		using MultType = std::conditional<sizeof(size_t) == 4, uint64_t, unsigned __int128>::type;

		MultType m = state + v;
		m *= kMul;
		return static_cast<uint64_t>(m ^ (m >> (sizeof(m) * 8 / 2)));
	}

	size_t operator()(T const& value) const {
		return static_cast<size_t>(Mix(Seed(), static_cast<uint64_t>(value)));
	}
};

#ifndef HASHES_H
#define HASHES_H

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace hash {

// Linux uses a noop hash
template <typename T>
struct Null {
	size_t operator()(T const& key) const {
		return static_cast<size_t>(key);
	}
};

// Visual Studio uses a FNV1a implementation.
template <typename T>
struct FNV1a {
	size_t operator()(T const& key) const {
#if SIZE_MAX == UINT32_MAX
		static constexpr size_t FNV_offset_basis = UINT64_C(14695981039346656037);
		static constexpr size_t FNV_prime = UINT64_C(1099511628211);
#else
		static constexpr size_t FNV_offset_basis = UINT32_C(2166136261);
		static constexpr size_t FNV_prime = UINT32_C(16777619);
#endif
		auto const data = reinterpret_cast<uint8_t const*>(&key);
		size_t val = FNV_offset_basis;
		for (size_t i = 0; i < sizeof(T); ++i) {
			val ^= static_cast<size_t>(data[i]);
			val *= FNV_prime;
		}
		return val;
	}
};

// absl::Hash makes use of CityHash
struct Seeder {
	static const void* const kSeed;
};

template <typename T>
struct CityHash {
	static constexpr uint64_t kMul = sizeof(size_t) == 4 ? uint64_t{0xcc9e2d51} : uint64_t{0x9ddfea08eb382d69};

	static uint64_t Seed() {
		return static_cast<uint64_t>(reinterpret_cast<uintptr_t>(Seeder::kSeed));
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

} // namespace hash

#endif

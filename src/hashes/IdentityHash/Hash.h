#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

static const char* HashName = "Identity";

// Linux uses an identity hash that does nothing. It's obviously fast, but extemely bad hash. Some hashmap implementations can't deal at all with this.
template <typename T>
struct Hash {
	size_t operator()(T const& key) const {
		return static_cast<size_t>(key);
	}
};

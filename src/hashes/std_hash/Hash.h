#pragma once

#include <functional>

static const char* HashName = "std::hash";

template <typename T>
using Hash = std::hash<T>;
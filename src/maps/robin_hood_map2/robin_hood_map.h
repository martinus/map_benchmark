#ifndef ROBIN_HOOD_MAP_H_INCLUDED
#define ROBIN_HOOD_MAP_H_INCLUDED

#include <algorithm>
#include <functional>
#include <utility>

/*  Macro to cut down on compiler warnings. */
#if 1                                 /*  there should be no more any compilers needing the "#else" version */
#define ROBIN_HOOD_UNUSED(identifier) /* identifier */
#else                                 /*  stupid, broken compiler */
#define ROBIN_HOOD_UNUSED(identifier) identifier
#endif

#if UINTPTR_MAX == UINT32_MAX
#define ROBIN_HOOD_BITNESS 32
#elif UINTPTR_MAX == UINT64_MAX
#define ROBIN_HOOD_BITNESS 64
#else
#error Unsupported bitness
#endif

#ifdef _WIN32
#define ROBIN_HOOD_NOINLINE __declspec(noinline)
#else
#if __GNUC__ >= 4
#define ROBIN_HOOD_NOINLINE __attribute__((noinline))
#else
#define ROBIN_HOOD_NOINLINE
#endif
#endif

#if defined(__GNUC__) || defined(__clang__)
#define ROBIN_HOOD_ATTRIBUTE_MAY_ALIAS __attribute__((__may_alias__))
#else
#define ROBIN_HOOD_ATTRIBUTE_MAY_ALIAS
#endif

namespace robin_hood {

namespace detail {

template <class E, class... Args>
ROBIN_HOOD_NOINLINE void doThrow(Args&&... args) {
	throw E(std::forward<Args>(args)...);
}

template <class E, class T, class... Args>
inline T* assertNotNull(T* t, Args&&... args) {
	if (nullptr == t) {
		doThrow<E>(std::forward<Args>(args)...);
	}
	return t;
}

template <class E, class... Args>
inline void assertTrue(bool x, Args&&... args) {
	if (!x) {
		doThrow<E>(std::forward<Args>(args)...);
	}
}

template <class T>
struct aliasing_value_wrapper {
	typedef T ROBIN_HOOD_ATTRIBUTE_MAY_ALIAS T_a;
	T_a value;
} ROBIN_HOOD_ATTRIBUTE_MAY_ALIAS;

template <class T>
using load_address_arg_t = std::conditional_t<std::is_volatile<T>::value, void const volatile*, void const*>;

template <class T>
using store_address_arg_t = std::conditional_t<std::is_volatile<T>::value, void volatile*, void*>;

#ifdef __IBMCPP__
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif
// having the structs packed forces the compiler to handle unaligned access
template <class T>
struct unaligned_value_wrapper {
	typedef T ROBIN_HOOD_ATTRIBUTE_MAY_ALIAS T_a;
	T_a value;
	unsigned char dummy[(sizeof(T) & 1) ? 2 : 1]; // try to force odd size (force compiler to expect unaligned start address)
} ROBIN_HOOD_ATTRIBUTE_MAY_ALIAS;
#pragma pack(pop)

template <typename T>
inline T aliasing_load(detail::load_address_arg_t<T> addr) {
	static_assert(std::is_integral<T>::value, "T must be integral");
	return static_cast<detail::aliasing_value_wrapper<T> const*>(addr)->value;
}

template <typename T>
inline void aliasing_store(detail::store_address_arg_t<T> addr, std::add_const_t<T> value) {
	// We use `std::add_const_t` to suppress template parameter type deduction because it would be DANGEROUS here!
	// (Adding const isn't necessary, but it also doesn't hurt here, and since there is no std::identity...)
	static_assert(std::is_integral<T>::value, "T must be integral");
	static_cast<detail::aliasing_value_wrapper<T>*>(addr)->value = value;
}

template <typename T>
inline T unaligned_load(detail::load_address_arg_t<T> addr) {
	static_assert(std::is_integral<T>::value, "T must be integral");
	return static_cast<detail::unaligned_value_wrapper<T> const*>(addr)->value;
}

template <typename T>
inline void unaligned_store(detail::store_address_arg_t<T> addr, std::add_const_t<T> value) {
	// We use `std::add_const_t` to suppress template parameter type deduction because it would be DANGEROUS here!
	// (Adding const isn't necessary, but it also doesn't hurt here, and since there is no std::identity...)
	static_assert(std::is_integral<T>::value, "T must be integral");
	static_cast<detail::unaligned_value_wrapper<T>*>(addr)->value = value;
}

// Traits class for aligned memory access - can be used to specialize algorithms
struct AlignedMemoryAccessTraits {
	template <class T>
	static T load(detail::load_address_arg_t<T> addr) {
		return aliasing_load<T>(addr);
	}

	template <class T>
	static void store(detail::store_address_arg_t<T> addr, std::add_const_t<T> value) {
		// We use `std::add_const_t` to suppress template parameter type deduction because it would be DANGEROUS here!
		// (Adding const isn't necessary, but it also doesn't hurt here, and since there is no std::identity...)
		return aliasing_store<T>(addr, value);
	}
};

// Traits class for unaligned memory access - can be used to specialize algorithms
struct UnalignedMemoryAccessTraits {
	template <class T>
	static T load(detail::load_address_arg_t<T> addr) {
		return unaligned_load<T>(addr);
	}

	template <class T>
	static void store(typename detail::store_address_arg_t<T>::type addr, std::add_const_t<T> value) {
		// We use `std::add_const_t` to suppress template parameter type deduction because it would be DANGEROUS here!
		// (Adding const isn't necessary, but it also doesn't hurt here, and since there is no std::identity...)
		return unaligned_store<T>(addr, value);
	}
};

template <typename T>
inline uintptr_t pointer_to_offset(const T* const t) {
	return static_cast<const char*>(t) - static_cast<const char*>(nullptr);
}

template <typename T>
inline bool check_alignment(const void* const ptr) {
	return pointer_to_offset(ptr) % std::alignment_of<T>::value == 0;
}

// does NOT perform native to little conversion!
template <class AlignmentTraits>
inline uint64_t murmur_hash_2_no_native_to_little_64A(void const* key, size_t len, uint64_t seed) {
	static uint64_t const m = UINT64_C(0xc6a4a7935bd1e995);
	static int const r = 47;

	uint64_t h = seed ^ (len * m);

	uint64_t const* data = reinterpret_cast<uint64_t const*>(key);
	uint64_t const* end = data + (len / 8);

	while (data != end) {
		// uint64_t k = boost::endian::native_to_little(AlignmentTraits::template load<uint64_t>(data++));
		uint64_t k = AlignmentTraits::template load<uint64_t>(data++);

		k *= m;
		k ^= k >> r;
		k *= m;

		h ^= k;
		h *= m;
	}

	unsigned char const* data2 = reinterpret_cast<unsigned char const*>(data);

	switch (len & 7) {
	case 7:
		h ^= static_cast<uint64_t>(data2[6]) << 48;
		/* FALLTHRU */
	case 6:
		h ^= static_cast<uint64_t>(data2[5]) << 40;
		/* FALLTHRU */
	case 5:
		h ^= static_cast<uint64_t>(data2[4]) << 32;
		/* FALLTHRU */
	case 4:
		h ^= static_cast<uint64_t>(data2[3]) << 24;
		/* FALLTHRU */
	case 3:
		h ^= static_cast<uint64_t>(data2[2]) << 16;
		/* FALLTHRU */
	case 2:
		h ^= static_cast<uint64_t>(data2[1]) << 8;
		/* FALLTHRU */
	case 1:
		h ^= static_cast<uint64_t>(data2[0]);
		h *= m;
	};

	h ^= h >> r;
	h *= m;
	h ^= h >> r;

	return h;
}

inline uint64_t murmur_hash_2_no_native_to_little_64A(void const* key, size_t len, uint64_t seed = 0xe17a1465) {
	return check_alignment<uint64_t>(key) ? murmur_hash_2_no_native_to_little_64A<AlignedMemoryAccessTraits>(key, len, seed)
										  : murmur_hash_2_no_native_to_little_64A<UnalignedMemoryAccessTraits>(key, len, seed);
}

// Allocates bulks of memory for objects of type T. This deallocates the memory in the destructor, and keeps a linked list of the allocated memory
// around. Overhead per allocation is the size of a pointer.
template <class T, size_t MinNumAllocs = 4, size_t MaxNumAllocs = 256>
class BulkPoolAllocator {
public:
	BulkPoolAllocator()
		: mHead(0)
		, mListForFree(0) {}

	// does not copy anything, just creates a new allocator.
	BulkPoolAllocator(const BulkPoolAllocator& ROBIN_HOOD_UNUSED(o))
		: mHead(0)
		, mListForFree(0) {}

	BulkPoolAllocator(BulkPoolAllocator&& o)
		: mHead(o.mHead)
		, mListForFree(o.mListForFree) {
		o.mListForFree = 0;
		o.mHead = 0;
	}

	BulkPoolAllocator& operator=(BulkPoolAllocator&& o) {
		reset();
		mHead = o.mHead;
		mListForFree = o.mListForFree;
		o.mListForFree = 0;
		o.mHead = 0;
		return *this;
	}

	BulkPoolAllocator& operator=(const BulkPoolAllocator& ROBIN_HOOD_UNUSED(o)) {
		// does not do anything
		return *this;
	}

	~BulkPoolAllocator() {
		reset();
	}

	// Deallocates all allocated memory.
	inline void reset() {
		while (mListForFree) {
			T* tmp = *mListForFree;
			free(mListForFree);
			mListForFree = reinterpret_cast<T**>(tmp);
		}
		mHead = 0;
	}

	// allocates, but does NOT initialize. Use in-place new constructor, e.g.
	//   T* obj = pool.allocate();
	//   new (obj) T();
	inline T* allocate() {
		T* tmp = mHead;
		if (!tmp) {
			tmp = performAllocation();
		}

		mHead = *reinterpret_cast<T**>(tmp);
		return tmp;
	}

	// does not actually deallocate but puts it in store.
	// make sure you have already called the destructor! e.g. with
	//  obj->~T();
	//  pool.deallocate(obj);
	inline void deallocate(T* obj) {
		*reinterpret_cast<T**>(obj) = mHead;
		mHead = obj;
	}

	// Adds an already allocated block of memory to the allocator. This allocator is from now on responsible for freeing the data (with free()). If
	// the provided data is not large enough to make use of, it is immediately freed. Otherwise it is reused and freed in the destructor.
	inline void addOrFree(void* ptr, const size_t numBytes) {
		// calculate number of available elements in ptr
		if (numBytes < ALIGNMENT + ALIGNED_SIZE) {
			// not enough data for at least one element. Free and return.
			free(ptr);
		} else {
			add(ptr, numBytes);
		}
	}

	inline void swap(BulkPoolAllocator<T, MinNumAllocs, MaxNumAllocs>& other) {
		std::swap(mHead, other.mHead);
		std::swap(mListForFree, other.mListForFree);
	}

private:
	// iterates the list of allocated memory to calculate how many to alloc next.
	// Recalculating this each time saves us a size_t member.
	// This ignores the fact that memory blocks might have been added manually with addOrFree. In practice, this should not matter much.
	inline size_t calcNumElementsToAlloc() const {
		T** tmp = mListForFree;
		size_t numAllocs = MinNumAllocs;

		while (numAllocs * 2 <= MaxNumAllocs && tmp) {
			tmp = *reinterpret_cast<T***>(tmp);
			numAllocs *= 2;
		}

		return numAllocs;
	}

	// WARNING: Underflow if numBytes < ALIGNMENT! This is guarded in addOrFree().
	void add(void* ptr, const size_t numBytes) {
		const size_t numElements = (numBytes - ALIGNMENT) / ALIGNED_SIZE;

		T** data = reinterpret_cast<T**>(ptr);

		// link free list
		*reinterpret_cast<T***>(data) = mListForFree;
		mListForFree = data;

		// create linked list for newly allocated data
		T* const headT = reinterpret_cast<T*>(reinterpret_cast<char*>(ptr) + ALIGNMENT);

		char* const head = reinterpret_cast<char*>(headT);

		// Visual Studio compiler automatically unrolls this loop, which is pretty cool
		for (size_t i = 0; i < numElements; ++i) {
			*reinterpret_cast<char**>(head + i * ALIGNED_SIZE) = head + (i + 1) * ALIGNED_SIZE;
		}

		// last one points to 0
		*reinterpret_cast<T**>(head + (numElements - 1) * ALIGNED_SIZE) = mHead;
		mHead = headT;
	}

	// Called when no memory is available (mHead == 0).
	// Don't inline this slow path.
	ROBIN_HOOD_NOINLINE T* performAllocation() {
		const size_t numElementsToAlloc = calcNumElementsToAlloc();

		// alloc new memory: [prev |T, T, ... T]
		// std::cout << (sizeof(T*) + ALIGNED_SIZE * numElementsToAlloc) << " bytes" << std::endl;
		size_t bytes = ALIGNMENT + ALIGNED_SIZE * numElementsToAlloc;
		add(assertNotNull<std::bad_alloc>(malloc(bytes)), bytes);
		return mHead;
	}

	// enforce byte alignment of the T's
	static const size_t ALIGNMENT = (std::max)(std::alignment_of<T>::value, std::alignment_of<T*>::value);
	static const size_t ALIGNED_SIZE = ((sizeof(T) - 1) / ALIGNMENT + 1) * ALIGNMENT;

	static_assert(MinNumAllocs >= 1, "MinNumAllocs");
	static_assert(MaxNumAllocs >= MinNumAllocs, "MaxNumAllocs");
	static_assert(ALIGNED_SIZE >= sizeof(T*), "ALIGNED_SIZE");
	static_assert(0 == (ALIGNED_SIZE % sizeof(T*)), "ALIGNED_SIZE mod");
	static_assert(ALIGNMENT >= sizeof(T*), "ALIGNMENT");

	T* mHead;
	T** mListForFree;
};

// fmix functions taken from MurmurHash3. See https://github.com/aappleby/smhasher/blob/master/src/MurmurHash3.cpp
// Necessary when e.g. purepath uses two small 32bit values concatenated.
static inline uint32_t fmix32(uint32_t h) {
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;
	return h;
}

static inline uint64_t fmix64(uint64_t k) {
	k ^= k >> 33;
	k *= 0xff51afd7ed558ccdULL;
	k ^= k >> 33;
	k *= 0xc4ceb9fe1a85ec53ULL;
	k ^= k >> 33;
	return k;
}

inline size_t calcMaxNumElementsAllowed128(size_t maxElements, uint8_t maxLoadFactor128) {
	// make sure we can't get an overflow, use floatingpoint arithmetic if necessary.
	return (maxElements > static_cast<size_t>(-1) / 128) ? static_cast<size_t>((static_cast<double>(maxElements) * maxLoadFactor128) / 128.0)
														 : (maxElements * maxLoadFactor128) / 128;
}

inline uint8_t calcMaxLoadFactor128(float ml) {
	// convert max load factor to multiple of it (factor 128) so we can perform fixed point operations.
	// factor 128 should be precise enough.
	return ml >= 1 ? 128 : static_cast<uint8_t>(128 * ml + 0.5f);
}

// All empty maps initial mInfo point to this infobyte. That way lookup in an empty map
// always returns false, and this is a very hot byte.
static uint8_t sDummyInfoByte = 0;

template <class T, size_t MinSize, size_t MaxSize, bool IsDirect>
struct NodeAllocator;

// dummy allocator that does nothing
template <class T, size_t MinSize, size_t MaxSize>
struct NodeAllocator<T, MinSize, MaxSize, true> {

	// we are not using the data, so just free it.
	void addOrFree(void* ptr, size_t ROBIN_HOOD_UNUSED(numBytes)) {
		free(ptr);
	}
};

template <class T, size_t MinSize, size_t MaxSize>
struct NodeAllocator<T, MinSize, MaxSize, false> : public BulkPoolAllocator<T, MinSize, MaxSize> {};

} // namespace detail

template <class T>
struct EqualTo;

template <class T>
struct FastHash;

template <class First, class Second>
struct Pair;

template <class Key, class T, class Hash = FastHash<Key>, class KeyEqual = EqualTo<Key>,
		  // Use direct map only when move does not throw, so swap and resize is possible without copying stuff.
		  // also make sure data is not too large, then swap might be slow.
		  bool IsDirect = sizeof(Key) + sizeof(T) <= sizeof(void*) * 3 &&
						  std::is_nothrow_move_constructible<std::pair<Key, T>>::value&& std::is_nothrow_move_assignable<std::pair<Key, T>>::value>
class map;

struct is_transparent_tag {};

// provide my own equal_to implementation because equal_to<> only exists since C++14
// see http://en.cppreference.com/w/cpp/utility/functional/equal_to_void
template <class T = void>
struct EqualTo {
	bool operator()(const T& lhs, const T& rhs) const {
		return lhs == rhs;
	}
};

template <>
struct EqualTo<void> {
	template <class T, class U>
	bool operator()(const T& lhs, const U& rhs) const {
		return lhs == rhs;
	}
};

template <class T = void>
struct FastHash {
	size_t operator()(const T& t) const {
		return std::hash<T>()(t);
	}
};

template <>
struct FastHash<void> {
	template <class T>
	size_t operator()(const T& t) const {
		// forward to FastHash<T>, NOT std::hash<T> so we get our overloads
		return FastHash<T>()(t);
	}
};

template <>
struct FastHash<std::string> {
	size_t operator()(const std::string& t) const {
		return detail::murmur_hash_2_no_native_to_little_64A(t.data(), t.size());
	}
};

template <size_t N>
struct FastHash<char[N]> {
	size_t operator()(char const(str)[N]) const {
		// -1 so we don't hash the trailing \0
		return detail::murmur_hash_2_no_native_to_little_64A(str, N - 1);
	}
};

template <>
struct FastHash<uint32_t> {
	size_t operator()(const uint32_t& t) const {
#if ROBIN_HOOD_BITNESS == 64
		return detail::fmix64(t);
#elif ROBIN_HOOD_BITNESS == 32
		return detail::fmix32(t);
#endif
	}
};

template <>
struct FastHash<int32_t> {
	size_t operator()(const uint32_t& t) const {
		return FastHash<uint32_t>()(t);
	}
};

template <>
struct FastHash<uint64_t> {
	size_t operator()(const uint64_t& t) const {
		// always use fmix64 to make sure all bits of t are mixed
		return detail::fmix64(t);
	}
};

template <>
struct FastHash<int64_t> {
	size_t operator()(const int64_t& t) const {
		return FastHash<uint64_t>()(t);
	}
};

// Added a custom Pair implementation because std::pair is not is_trivially_copyable, which means it is not allowed to use it in std::memcpy. This
// struct is copyable, which is also tested.
template <class First, class Second>
struct Pair {
	// pair constructors are explicit so we don't accidentally call this ctor when we don't have to.
	explicit Pair(std::pair<First, Second> const& pair)
		: first(pair.first)
		, second(pair.second) {}

	// pair constructors are explicit so we don't accidentally call this ctor when we don't have to.
	explicit Pair(std::pair<First, Second>&& pair)
		: first(std::move(pair.first))
		, second(std::move(pair.second)) {}

	constexpr Pair(const First& firstArg, const Second& secondArg)
		: first(firstArg)
		, second(secondArg) {}

	constexpr Pair(First&& firstArg, Second&& secondArg)
		: first(std::move(firstArg))
		, second(std::move(secondArg)) {}

	template <class... Args1, class... Args2>
	inline Pair(std::piecewise_construct_t, std::tuple<Args1...> firstArgs, std::tuple<Args2...> secondArgs)
		: Pair(firstArgs, secondArgs, std::index_sequence_for<Args1...>(), std::index_sequence_for<Args2...>()) {}

	// constructor called from the std::piecewise_construct_t ctor
	template <class Tuple1, class Tuple2, size_t... Indexes1, size_t... Indexes2>
	inline Pair(Tuple1& val1, Tuple2& val2, std::index_sequence<Indexes1...>, std::index_sequence<Indexes2...>)
		: first(std::get<Indexes1>(std::move(val1))...)
		, second(std::get<Indexes2>(std::move(val2))...) {}

	First first;
	Second second;
};

/// A highly optimized hashmap implementation, using the Robin Hood algorithm.
/// This implementation is based on https://github.com/martinus/robin-hood-hashing/
/// with permission from the author.
///
/// In most cases, this map should be usable as a drop-in replacement for
/// boost::unordered_map or std::unordered_map, but be about 2x faster in most cases
/// and require much less allocations.
///
/// This implementation uses the following memory layout:
///
/// [Node, Node, ... Node | info, info, ... infoSentinel ]
///
/// * Node: either a DataNode that directly has the std::pair<key, val> as member,
///   or a DataNode with a pointer to std::pair<key,val>. Which DataNode representation to use depends
///   on how fast the swap() operation is. Heuristically, this is automatically choosen based on sizeof().
///   there are always 2^n Nodes.
///
/// * info: Each Node in the map has a corresponding info byte, so there are 2^n info bytes.
///   Each byte is initialized to 0, meaning the corresponding Node is empty. Set to 1 means the corresponding
///   node contains data. Set to 2 means the corresponding Node is filled, but it actually belongs to the
///   previous position and was pushed out because that place is already taken.
///
/// * infoSentinel: Sentinel byte set to 1, so that iterator's ++ can stop at end() without the need for a idx
///   variable.
template <class Key, class T, class Hash, class KeyEqual, bool IsDirect>
class map : Hash, KeyEqual, detail::NodeAllocator<Pair<Key, T>, 4, 16384, IsDirect> {
	// configuration defaults
	static constexpr uint8_t MaxLoadFactor128 = 102; // 1 byte
	static constexpr size_t InitialNumElements = 4;

	using DataPool = detail::NodeAllocator<Pair<Key, T>, 4, 16384, IsDirect>;

public:
	using key_type = Key;
	using mapped_type = T;
	using value_type = Pair<Key, T>;
	using size_type = size_t;
	using hasher = Hash;
	using key_equal = KeyEqual;
	using Self = map<key_type, mapped_type, hasher, key_equal, IsDirect>;

private:
	// Primary template for the data node. We have special implementations for small and big objects.
	// For large objects it is assumed that swap() is fairly slow, so we allocate these on the heap
	// so swap merely swaps a pointer.
	template <class M, bool>
	class DataNode {};

	// Small: just allocate on the stack.
	template <class M>
	class DataNode<M, true> {
	public:
		template <class... Args>
		DataNode(M& ROBIN_HOOD_UNUSED(map), Args&&... args)
			: mData(std::forward<Args>(args)...) {}

		// doesn't do anything
		void destroy(M& ROBIN_HOOD_UNUSED(map)) {}
		void destroyDoNotDeallocate() {}

		value_type const* operator->() const {
			return &mData;
		}
		value_type* operator->() {
			return &mData;
		}

		const value_type& operator*() const {
			return mData;
		}

		value_type& operator*() {
			return mData;
		}

		void swap(DataNode<M, true>& o) {
			std::swap(mData.first, o.mData.first);
			std::swap(mData.second, o.mData.second);
		}

	private:
		value_type mData;
	};

	// big object: allocate on heap.
	template <class M>
	class DataNode<M, false> {
	public:
		template <class... Args>
		DataNode(M& map, Args&&... args)
			: mData(map.allocate()) {
			new (mData) value_type(std::forward<Args>(args)...);
		}

		void destroy(M& map) {
			// don't deallocate, just put it into list of datapool.
			mData->~value_type();
			map.deallocate(mData);
		}

		void destroyDoNotDeallocate() {
			mData->~value_type();
		}

		value_type const* operator->() const {
			return mData;
		}

		value_type* operator->() {
			return mData;
		}

		const value_type& operator*() const {
			return *mData;
		}

		value_type& operator*() {
			return *mData;
		}

		void swap(DataNode<M, false>& o) {
			std::swap(mData, o.mData);
		}

	private:
		value_type* mData;
	};

	typedef DataNode<Self, IsDirect> Node;

	size_t calcNumBytesInfo(size_t numElements) const {
		const size_t s = sizeof(uint8_t) * (numElements + 1);
		if (s / sizeof(uint8_t) != numElements + 1) {
			throwOverflowError();
		}
		return s;
	}
	size_t calcNumBytesNode(size_t numElements) const {
		const size_t s = sizeof(Node) * numElements;
		if (s / sizeof(Node) != numElements) {
			throwOverflowError();
		}
		return s;
	}
	size_t calcNumBytesTotal(size_t numElements) const {
		const size_t si = calcNumBytesInfo(numElements);
		const size_t sn = calcNumBytesNode(numElements);
		const size_t s = si + sn;
		if (s <= si || s <= sn) {
			throwOverflowError();
		}
		return s;
	}

	// forwards the index by one, wrapping around at the end
	void next(int& info, size_t& idx) const {
		idx = (idx + 1) & mMask;
		++info;
	}

	void nextWhileLess(int& info, size_t& idx) const {
		// unrolling this by hand did not bring any speedups.
		while (info < mInfo[idx]) {
			next(info, idx);
		}
	}

	void bubbleDown(size_t& idx, size_t const& insertion_idx) {
		while (idx != insertion_idx) {
			size_t const prev_idx = (idx - 1) & mMask;
			mKeyVals[idx].swap(mKeyVals[prev_idx]);
			std::swap(mInfo[idx], mInfo[prev_idx]);

			// increase the shifted up element
			if (0xFF == ++mInfo[idx]) {
				mMaxNumElementsAllowed = 0;
			}
			idx = prev_idx;
		}
	}

	// copy of find(), except that it returns iterator instead of const_iterator.
	template <class Other>
	size_t findIdx(const Other& key) const {
		size_t idx = Hash::operator()(key) & mMask;
		int info = 1;
		nextWhileLess(info, idx);

		// check while info matches with the source idx
		while (info == mInfo[idx]) {
			if (KeyEqual::operator()(key, mKeyVals[idx]->first)) {
				return idx;
			}
			next(info, idx);
		}

		// nothing found!
		return mMask + 1;
	}

	template <class M, bool UseMemcpy>
	struct Cloner;

	// fast path: Just copy data, without allocating anything.
	template <class M>
	struct Cloner<M, true> {
		void operator()(M const& source, M& target) const {
			// std::memcpy(target.mKeyVals, source.mKeyVals, target.calcNumBytesTotal(target.mMask + 1));
			std::copy(source.mKeyVals, source.mKeyVals + target.calcNumBytesTotal(target.mMask + 1), target.mKeyVals);
		}
	};

	template <class M>
	struct Cloner<M, false> {
		void operator()(M const& source, M& target) const {
			// make sure to copy initialize sentinel as well
			// std::memcpy(target.mInfo, source.mInfo, target.calcNumBytesInfo(target.mMask + 1));
			std::copy(source.mInfo, source.mInfo + target.calcNumBytesInfo(target.mMask + 1), target.mInfo);

			for (size_t i = 0; i < target.mMask + 1; ++i) {
				if (target.mInfo[i]) {
					new (target.mKeyVals + i) Node(target, *source.mKeyVals[i]);
				}
			}
		}
	};

	void cloneData(const map& o) {
		Cloner<map, IsDirect && std::is_trivially_copyable<Node>::value>()(o, *this);
	}

	// inserts a keyval that is guaranteed to be new, e.g. when the hashmap is resized.
	// @return index where the element was created
	size_t insert_move(Node&& keyval) {
		// we don't retry, fail if overflowing
		// don't need to check max num elements
		if (0 == mMaxNumElementsAllowed) {
			throwOverflowError();
		}

		size_t idx = Hash::operator()(keyval->first) & mMask;

		// skip forward. Use <= because we are certain that the element is not there.
		int info = 1;
		while (info <= mInfo[idx]) {
			idx = (idx + 1) & mMask;
			++info;
		}

		// key not found, so we are now exactly where we want to insert it.
		const size_t insertion_idx = idx;
		uint8_t insertion_info = info;
		if (0xFF == insertion_info) {
			mMaxNumElementsAllowed = 0;
		}

		// find an empty spot
		while (0 != mInfo[idx]) {
			next(info, idx);
		}

		// put at empty spot
		new (mKeyVals + idx) Node(std::move(keyval));
		mInfo[idx] = insertion_info;

		// bubble down into correct position
		bubbleDown(idx, insertion_idx);

		++mNumElements;
		return insertion_idx;
	}

	// generic iterator for both const_iterator and iterator.
	template <bool IsConst>
	class Iter {
	private:
		typedef typename std::conditional<IsConst, Node const*, Node*>::type NodePtr;

	public:
		typedef std::ptrdiff_t difference_type;
		typedef typename Self::value_type value_type;
		typedef typename std::conditional<IsConst, value_type const&, value_type&>::type reference;
		typedef typename std::conditional<IsConst, value_type const*, value_type*>::type pointer;
		typedef std::forward_iterator_tag iterator_category;

		// both const_iterator and iterator can be constructed from a non-const iterator
		Iter(Iter<false> const& other)
			: mKeyVals(other.mKeyVals)
			, mInfo(other.mInfo) {}

		Iter(NodePtr valPtr, uint8_t const* infoPtr)
			: mKeyVals(valPtr)
			, mInfo(infoPtr) {}

		// prefix increment. Undefined behavior if we are at end()!
		Iter& operator++() {
			do {
				mKeyVals++;
				mInfo++;
			} while (0 == *mInfo);
			return *this;
		}

		reference operator*() const {
			return **mKeyVals;
		}

		pointer operator->() const {
			return &**mKeyVals;
		}

		template <bool O>
		bool operator==(Iter<O> const& o) const {
			return mKeyVals == o.mKeyVals;
		}

		template <bool O>
		bool operator!=(Iter<O> const& o) const {
			return mKeyVals != o.mKeyVals;
		}

	private:
		friend class map<key_type, mapped_type, hasher, key_equal, IsDirect>;
		NodePtr mKeyVals;
		uint8_t const* mInfo;
	};

public:
	typedef Iter<false> iterator;
	typedef Iter<true> const_iterator;

	/// Creates an empty hash map. Nothing is allocated yet, this happens at the first insert.
	/// This tremendously speeds up ctor & dtor of a map that never receives an element. The
	/// penalty is payed at the first insert, and not before. Lookup of this empty map works
	/// because everybody points to sDummyInfoByte.
	/// parameter bucket_count is dictated by the standard, but we can ignore it.
	explicit map(size_t ROBIN_HOOD_UNUSED(bucket_count) = 0, const Hash& hash = Hash(), const KeyEqual& equal = KeyEqual())
		: Hash(hash)
		, KeyEqual(equal) {}

	template <class Iter>
	map(Iter first, Iter last, size_t ROBIN_HOOD_UNUSED(bucket_count) = 0, const Hash& hash = Hash(), const KeyEqual& equal = KeyEqual())
		: Hash(hash)
		, KeyEqual(equal) {
		insert(first, last);
	}

	map(std::initializer_list<value_type> init, size_t ROBIN_HOOD_UNUSED(bucket_count) = 0, const Hash& hash = Hash(),
		const KeyEqual& equal = KeyEqual())
		: Hash(hash)
		, KeyEqual(equal) {
		insert(init.begin(), init.end());
	}

	map(map&& o)
		: Hash(std::move(static_cast<Hash&>(o)))
		, KeyEqual(std::move(static_cast<KeyEqual&>(o)))
		, DataPool(std::move(static_cast<DataPool&>(o)))
		, mKeyVals(std::move(o.mKeyVals))
		, mInfo(std::move(o.mInfo))
		, mNumElements(std::move(o.mNumElements))
		, mMask(std::move(o.mMask))
		, mMaxNumElementsAllowed(std::move(o.mMaxNumElementsAllowed)) {
		// set other's mask to 0 so its destructor won't do anything
		o.mMask = 0;
	}

	map& operator=(map&& o) {
		if (&o != this) {
			// different, move it
			destroy();
			mKeyVals = std::move(o.mKeyVals);
			mInfo = std::move(o.mInfo);
			mNumElements = std::move(o.mNumElements);
			mMask = std::move(o.mMask);
			mMaxNumElementsAllowed = std::move(o.mMaxNumElementsAllowed);
			Hash::operator=(std::move(static_cast<Hash&>(o)));
			KeyEqual::operator=(std::move(static_cast<KeyEqual&>(o)));
			DataPool::operator=(std::move(static_cast<DataPool&>(o)));
			// set other's mask to 0 so its destructor won't do anything
			o.mMask = 0;
		}
		return *this;
	}

	map(const map& o)
		: Hash(static_cast<const Hash&>(o))
		, KeyEqual(static_cast<const KeyEqual&>(o))
		, DataPool(static_cast<const DataPool&>(o)) {

		if (!o.empty()) {
			// not empty: create an exact copy. it is also possible to just iterate through all elements and insert them, but
			// copying is probably faster.

			mKeyVals = reinterpret_cast<Node*>(detail::assertNotNull<std::bad_alloc>(malloc(calcNumBytesTotal(o.mMask + 1))));
			// no need for calloc because clonData does memcpy
			mInfo = reinterpret_cast<uint8_t*>(mKeyVals + o.mMask + 1);
			mNumElements = o.mNumElements;
			mMask = o.mMask;
			mMaxNumElementsAllowed = o.mMaxNumElementsAllowed;
			cloneData(o);
		}
	}

	// Creates a copy of the given map. Copy constructor of each entry is used.
	map& operator=(const map& o) {
		if (&o == this) {
			// prevent assigning of itself
			return *this;
		}

		// we keep using the old allocator and not assign the new one, because we want to keep the memory available.
		// when it is the same size.
		if (o.empty()) {
			if (0 == mMask) {
				// nothing to do, we are empty too
				return *this;
			}

			// not empty: destroy what we have there
			// clear also resets mInfo to 0, that's sometimes not necessary.
			destroy();
			mKeyVals = reinterpret_cast<Node*>(&detail::sDummyInfoByte) - 1;
			mInfo = &detail::sDummyInfoByte;
			Hash::operator=(static_cast<const Hash&>(o));
			KeyEqual::operator=(static_cast<const KeyEqual&>(o));
			mNumElements = 0;
			mMask = 0;
			mMaxNumElementsAllowed = 0;
			return *this;
		}

		// clean up old stuff
		destroyNodes();

		if (mMask != o.mMask) {
			// no luck: we don't have the same array size allocated, so we need to realloc.
			if (0 != mMask) {
				// only deallocate if we actually have data!
				free(mKeyVals);
			}

			mKeyVals = reinterpret_cast<Node*>(detail::assertNotNull<std::bad_alloc>(malloc(calcNumBytesTotal(o.mMask + 1))));

			// no need for calloc here because cloneData  performs a memcpy.
			mInfo = reinterpret_cast<uint8_t*>(mKeyVals + o.mMask + 1);
			// sentinel is set in cloneData
		}
		Hash::operator=(static_cast<const Hash&>(o));
		KeyEqual::operator=(static_cast<const KeyEqual&>(o));
		mNumElements = o.mNumElements;
		mMask = o.mMask;
		mMaxNumElementsAllowed = o.mMaxNumElementsAllowed;
		cloneData(o);

		return *this;
	}

	// Swaps everything between the two maps.
	void swap(map& o) {
		std::swap(mKeyVals, o.mKeyVals);
		std::swap(mInfo, o.mInfo);
		std::swap(mNumElements, o.mNumElements);
		std::swap(mMask, o.mMask);
		std::swap(mMaxNumElementsAllowed, o.mMaxNumElementsAllowed);
		std::swap(static_cast<Hash&>(*this), static_cast<Hash&>(o));
		std::swap(static_cast<KeyEqual&>(*this), static_cast<KeyEqual&>(o));
		// no harm done in swapping datapool
		std::swap(static_cast<DataPool&>(*this), static_cast<DataPool&>(o));
	}

	// Clears all data, without resizing.
	void clear() {
		if (empty()) {
			// don't do anything! also important because we don't want to write to sDummyInfoByte, even
			// though we would just write 0 to it.
			return;
		}

		destroyNodes();

		// clear everything except the sentinel
		// std::memset(mInfo, 0, sizeof(uint8_t) * (mMask + 1));
		std::fill(mInfo, mInfo + (sizeof(uint8_t) * (mMask + 1)), 0);
	}

	/// Destroys the map and all it's contents.
	~map() {
		destroy();
	}

	/// Checks if both maps contain the same entries. Order is irrelevant.
	bool operator==(const map& other) const {
		if (other.size() != size()) {
			return false;
		}
		const_iterator myEnd = end();
		for (const_iterator otherIt = other.begin(), otherEnd = other.end(); otherIt != otherEnd; ++otherIt) {
			Self::const_iterator myIt = find(otherIt->first);
			if (myIt == myEnd || !(myIt->second == otherIt->second)) {
				return false;
			}
		}

		return true;
	}

	bool operator!=(const map& other) const {
		return !operator==(other);
	}

	mapped_type& operator[](const key_type& key) {
		return doCreateByKey(key);
	}

	mapped_type& operator[](key_type&& key) {
		return doCreateByKey(std::move(key));
	}

	template <class Iter>
	void insert(Iter first, Iter last) {
		for (; first != last; ++first) {
			// value_type ctor needed because this might be called with std::pair's
			insert(value_type(*first));
		}
	}

	template <class... Args>
	std::pair<iterator, bool> emplace(Args&&... args) {
		return insert(std::move(value_type(std::forward<Args>(args)...)));
	}

	std::pair<iterator, bool> insert(const value_type& keyval) {
		return doInsert(keyval);
	}

	std::pair<iterator, bool> insert(value_type&& keyval) {
		return doInsert(std::move(keyval));
	}

	size_t count(const key_type& key) const {
		return findIdx(key) == (mMask + 1) ? 0 : 1;
	}

	const_iterator find(const key_type& key) const {
		const size_t idx = findIdx(key);
		return const_iterator(mKeyVals + idx, mInfo + idx);
	}

	template <class OtherKey>
	const_iterator find(const OtherKey& key, is_transparent_tag) const {
		const size_t idx = findIdx(key);
		return const_iterator(mKeyVals + idx, mInfo + idx);
	}

	iterator find(const key_type& key) {
		const size_t idx = findIdx(key);
		return iterator(mKeyVals + idx, mInfo + idx);
	}

	template <class OtherKey>
	iterator find(const OtherKey& key, is_transparent_tag) {
		const size_t idx = findIdx(key);
		return iterator(mKeyVals + idx, mInfo + idx);
	}

	iterator begin() {
		if (empty()) {
			return end();
		}
		return ++iterator(mKeyVals - 1, mInfo - 1);
	}
	const_iterator begin() const {
		return cbegin();
	}
	const_iterator cbegin() const {
		if (empty()) {
			return cend();
		}
		return ++const_iterator(mKeyVals - 1, mInfo - 1);
	}

	iterator end() {
		// no need to supply valid info pointer: end() must not be dereferenced, and only node pointer is compared.
		return iterator(reinterpret_cast<Node*>(mInfo), 0);
	}
	const_iterator end() const {
		return cend();
	}
	const_iterator cend() const {
		return const_iterator(reinterpret_cast<Node*>(mInfo), 0);
	}

	iterator erase(const_iterator pos) {
		// its safe to perform const cast here
		return erase(iterator(const_cast<Node*>(pos.mKeyVals), const_cast<uint8_t*>(pos.mInfo)));
	}

	// Erases element at pos, returns iterator to the next element.
	iterator erase(iterator pos) {
		// we assume that pos always points to a valid entry, and not end().

		// perform backward shift deletion: shift elements to the left
		// until we find one that is either empty or has zero offset.
		size_t idx = pos.mKeyVals - mKeyVals;
		size_t nextIdx = (idx + 1) & mMask;
		while (mInfo[nextIdx] > 1) {
			mInfo[idx] = mInfo[nextIdx] - 1;
			mKeyVals[idx].swap(mKeyVals[nextIdx]);
			idx = nextIdx;
			nextIdx = (idx + 1) & mMask;
		}

		mInfo[idx] = 0;
		mKeyVals[idx].destroy(*this);
		mKeyVals[idx].~Node();
		--mNumElements;

		if (*pos.mInfo) {
			// we've backward shifted, return this again
			return pos;
		}

		// no backward shift, return next element
		return ++pos;
	}

	size_t erase(const key_type& key) {
		size_t idx = Hash::operator()(key) & mMask;

		int info = 1;
		nextWhileLess(info, idx);

		// check while info matches with the source idx
		while (info == mInfo[idx]) {
			if (KeyEqual::operator()(key, mKeyVals[idx]->first)) {
				// found it! perform backward shift deletion: shift elements to the left
				// until we find one that is either empty or has zero offset.
				size_t nextIdx = (idx + 1) & mMask;
				while (mInfo[nextIdx] > 1) {
					mInfo[idx] = mInfo[nextIdx] - 1;
					mKeyVals[idx].swap(mKeyVals[nextIdx]);
					idx = nextIdx;
					nextIdx = (idx + 1) & mMask;
				}

				mInfo[idx] = 0;
				mKeyVals[idx].destroy(*this);
				mKeyVals[idx].~Node();

				--mNumElements;
				return 1;
			}
			next(info, idx);
		}

		// nothing found to delete
		return 0;
	}

	size_type size() const {
		return mNumElements;
	}

	size_type max_size() const {
		return static_cast<size_type>(-1);
	}

	bool empty() const {
		return 0 == mNumElements;
	}

	float max_load_factor() const {
		return MaxLoadFactor128 / 128.0f;
	}

	/* not supported
	void max_load_factor(float ml) {
		mMaxLoadFactor128 = calcMaxLoadFactor128(ml);
		mMaxNumElementsAllowed = calcMaxNumElementsAllowed128(mMask + 1, mMaxLoadFactor128);
	}
	*/

	// Average number of elements per bucket. Since we allow only 1 per bucket
	float load_factor() const {
		return static_cast<float>(size()) / (mMask + 1);
	}

private:
	ROBIN_HOOD_NOINLINE void throwOverflowError() const {
		throw std::overflow_error("robin_hood::map overflow");
	}

	void init_data(size_t max_elements) {
		mNumElements = 0;
		mMask = max_elements - 1;
		mMaxNumElementsAllowed = detail::calcMaxNumElementsAllowed128(max_elements, MaxLoadFactor128);

		// calloc also zeroes everything
		mKeyVals = reinterpret_cast<Node*>(detail::assertNotNull<std::bad_alloc>(calloc(1, calcNumBytesTotal(max_elements))));
		mInfo = reinterpret_cast<uint8_t*>(mKeyVals + max_elements);

		// set sentinel
		mInfo[max_elements] = 1;
	}

	template <class Arg>
	mapped_type& doCreateByKey(Arg&& key) {
		size_t const hash = Hash::operator()(key);
		while (true) {
			size_t idx = hash & mMask;

			int info = 1;
			nextWhileLess(info, idx);

			// while we potentially have a match
			while (info == mInfo[idx]) {
				if (KeyEqual::operator()(key, mKeyVals[idx]->first)) {
					// key already exists, do not insert.
					return mKeyVals[idx]->second;
				}
				next(info, idx);
			}

			// unlikely that this evaluates to true
			if (mNumElements >= mMaxNumElementsAllowed) {
				increase_size();
				continue;
			}

			// key not found, so we are now exactly where we want to insert it.
			size_t const insertion_idx = idx;
			uint8_t const insertion_info = info;
			if (0xFF == insertion_info) {
				// might overflow next time, set to 0 so we increase size next time
				mMaxNumElementsAllowed = 0;
			}

			// find an empty spot
			while (0 != mInfo[idx]) {
				next(info, idx);
			}

			// put at empty spot. This forwards all arguments into the node where the object is constructed exactly where it is needed.
			new (mKeyVals + idx) Node(*this, std::piecewise_construct, std::forward_as_tuple(std::forward<Arg>(key)), std::forward_as_tuple());

			// mKeyVals[idx]->first = std::move(key);
			mInfo[idx] = insertion_info;

			// bubble down into correct position
			bubbleDown(idx, insertion_idx);

			++mNumElements;
			return mKeyVals[insertion_idx]->second;
		}
	}

	// This is exactly the same code as operator[], except for the return values
	template <class Arg>
	std::pair<iterator, bool> doInsert(Arg&& keyval) {
		size_t const hash = Hash::operator()(keyval.first);
		while (true) {
			size_t idx = hash & mMask;

			int info = 1;
			nextWhileLess(info, idx);

			// while we potentially have a match
			while (info == mInfo[idx]) {
				if (KeyEqual::operator()(keyval.first, mKeyVals[idx]->first)) {
					// key already exists, do NOT insert.
					// see http://en.cppreference.com/w/cpp/container/unordered_map/insert
					return std::make_pair<iterator, bool>(iterator(mKeyVals + idx, mInfo + idx), false);
				}
				next(info, idx);
			}

			// unlikely that this evaluates to true
			if (mNumElements >= mMaxNumElementsAllowed) {
				increase_size();
				continue;
			}

			// key not found, so we are now exactly where we want to insert it.
			const size_t insertion_idx = idx;
			uint8_t insertion_info = info;
			if (0xFF == insertion_info) {
				mMaxNumElementsAllowed = 0;
			}

			// find an empty spot
			while (0 != mInfo[idx]) {
				next(info, idx);
			}

			// put at empty spot
			new (mKeyVals + idx) Node(*this, std::forward<Arg>(keyval));
			mInfo[idx] = insertion_info;

			// bubble down into correct position
			bubbleDown(idx, insertion_idx);

			++mNumElements;
			return std::make_pair(iterator(mKeyVals + insertion_idx, mInfo + insertion_idx), true);
		}
	}

	void increase_size() {
		// nothing allocated yet? just allocate 4 elements
		if (0 == mMask) {
			init_data(InitialNumElements);
			return;
		}

		// it seems we have a really bad hash function! don't try to resize again
		if (mNumElements * 2 < detail::calcMaxNumElementsAllowed128(mMask + 1, MaxLoadFactor128)) {
			throwOverflowError();
		}

		// std::cout << (100.0*mNumElements / (mMask + 1)) << "% full, resizing" << std::endl;
		Node* oldKeyVals = mKeyVals;
		uint8_t* oldInfo = mInfo;

		const size_t oldMaxElements = mMask + 1;

		// resize operation: move stuff
		init_data(oldMaxElements * 2);

		for (size_t i = 0; i < oldMaxElements; ++i) {
			if (oldInfo[i]) {
				insert_move(std::move(oldKeyVals[i]));
				// destroy the node but DON'T destroy the data.
				oldKeyVals[i].~Node();
			}
		}

		// don't destroy old data: put it into the pool instead
		DataPool::addOrFree(oldKeyVals, calcNumBytesTotal(oldMaxElements));
	}

	// destroys all nodes (without clearing mInfo)
	// WARNING don't call when empty, because of sentinel.
	void destroyNodes() {
		mNumElements = 0;
		if (IsDirect && std::is_trivially_destructible<Node>::value) {
			return;
		}

		// clear also resets mInfo to 0, that's sometimes not necessary.
		for (size_t idx = 0; idx <= mMask; ++idx) {
			if (0 != mInfo[idx]) {
				Node& n = mKeyVals[idx];
				n.destroy(*this);
				n.~Node();
			}
		}
	}

	void destroyNodesDoNotDeallocate() {
		mNumElements = 0;

		if (IsDirect && std::is_trivially_destructible<Node>::value) {
			return;
		}

		// clear also resets mInfo to 0, that's sometimes not necessary.
		for (size_t idx = 0; idx <= mMask; ++idx) {
			if (0 != mInfo[idx]) {
				Node& n = mKeyVals[idx];
				n.destroyDoNotDeallocate();
				n.~Node();
			}
		}
	}

	void destroy() {
		if (0 == mMask) {
			// don't deallocate! we are pointing to sDummyInfoByte.
			return;
		}

		destroyNodesDoNotDeallocate();
		free(mKeyVals);
	}

	// members are sorted so no padding occurs
	Node* mKeyVals = reinterpret_cast<Node*>(&detail::sDummyInfoByte - sizeof(Node)); // 8 byte
	// Node* mKeyVals = reinterpret_cast<Node*>(&sDummyInfoByte) - 1; // 8 byte
	uint8_t* mInfo = &detail::sDummyInfoByte; // 8 byte
	size_t mNumElements = 0;                  // 8 byte
	size_t mMask = 0;                         // 8 byte
	size_t mMaxNumElementsAllowed = 0;        // 8 byte
};

template <class Key, class T, class Hash = FastHash<Key>, class KeyEqual = EqualTo<Key>>
using direct_map = map<Key, T, Hash, KeyEqual, true>;

template <class Key, class T, class Hash = FastHash<Key>, class KeyEqual = EqualTo<Key>>
using indirect_map = map<Key, T, Hash, KeyEqual, false>;

} // namespace robin_hood

#endif

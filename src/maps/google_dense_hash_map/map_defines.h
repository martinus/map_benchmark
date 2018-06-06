
template <class K, class V, class H = std::hash<int>>
class Map {
private:
	typedef google::dense_hash_map<K, V, H> GM;
	GM mGm;

public:
	inline Map() {
		mGm.set_empty_key(-1);
		mGm.set_deleted_key(-2);
	}

	inline typename GM::data_type& operator[](const typename GM::key_type& key) {
		return mGm[key];
	}

	inline typename GM::size_type erase(const typename GM::key_type& key) {
		return mGm.erase(key);
	}

	inline typename GM::size_type size() const {
		return mGm.size();
	}

	inline typename GM::const_iterator find(const typename GM::key_type& key) const {
		return mGm.find(key);
	}

	inline typename GM::const_iterator end() const {
		return mGm.end();
	}

	inline void max_load_factor(float newLoadFactor) {
		mGm.max_load_factor(newLoadFactor);
	}

	inline size_t max_size() const {
		return mGm.max_size();
	}

	inline float load_factor() const {
		return mGm.load_factor();
	}
};

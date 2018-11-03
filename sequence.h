#include <cassert>
#include <algorithm>
#include <stdexcept>
#include <cstdint>
#include <type_traits>

using std::size_t;

template<class T, class Traits>
struct sequence {
	static_assert(std::is_same<T, typename Traits::char_type>::value, "");

	using size_type = std::size_t;
	using const_iterator = const T *;
	using reverse_const_iterator = std::reverse_iterator<const_iterator>;
	using const_reference = const T &;
	static constexpr size_type npos = size_type(-1);

	const_iterator lptr, rptr;

	sequence() noexcept {
		lptr = nullptr;
		rptr = nullptr;
	}

	sequence(const sequence &other) noexcept {
		lptr = other.lptr;
		rptr = other.rptr;
	}

	sequence(const T *s, size_type count) {
		lptr = s;
		rptr = s + count;
	}

	sequence(const T *s) {
		lptr = s;
		rptr = s + Traits::length(s);
	}

	sequence &operator=(const sequence &view) noexcept {
		lptr = view.lptr;
		rptr = view.rptr;
		return this;
	}

	const_iterator begin() const noexcept {
		return lptr;
	}

	constexpr const_iterator end() const noexcept {
		return rptr;
	}

	constexpr reverse_const_iterator rbegin() const noexcept {
		return reverse_const_iterator{ end() };
	}

	constexpr reverse_const_iterator rend() const noexcept {
		return reverse_const_iterator{ begin() };
	}

	constexpr const_reference operator[](size_type pos) const {
		return *std::next(lptr, pos);
	}

	constexpr const_reference at(size_type pos) const { //throws std::out_of_range
		if (pos >= size()) {
			throw std::out_of_range("out of range");
		}
		return this->operator[](pos);
	}

	constexpr const_reference front() const {
		return *lptr;
	}

	constexpr const_reference back() const {
		return *(rptr - 1);
	}

	constexpr size_type size() const noexcept {
		return rptr - lptr;
	}

	constexpr bool empty() const noexcept {
		return lptr == rptr;
	}

	void remove_prefix(size_type n) {
		lptr += n;
	}

	void remove_suffix(size_type n) {
		rptr -= n;
	}

	void swap(sequence &v) noexcept {
		using std::swap;
		swap(lptr, v.lptr);
		swap(rptr, v.rptr);
	}

	//copies no more than count characters, return number of copyed elements
	size_type copy(T *dest, size_type count, size_type pos = 0) const { //throws std::out_of_range
		sequence sub = subseq(pos, count);
		Traits::copy(dest, sub.lptr, sub.size());
		return sub.size();
	}

	int compare(sequence v) const noexcept {
		if (*this < v) {
			return -1;
		}
		if (*this == v) {
			return 0;
		}
		if (*this > v) {
			return 1;
		}
		assert(false);
	}

	sequence subseq(size_type pos = 0, size_type count = npos) const { //throws std::out_of_range
		if (pos >= size()) {
			throw std::out_of_range("out of range");
		}

		size_type end = size();
		if (count != npos && pos + count < end) { end = pos + count; }
		return make_seq(begin() + pos, begin() + end);
	}

	size_type find(sequence v, size_type pos = 0) const noexcept {
		size_type v_size = v.size();
		for (size_type i = pos; i < size() - v_size; ++i) {
			if (subseq(i, v_size) == v) {
				return i;
			}
		}
		return npos;
	}

	size_type rfind(sequence v, size_type pos = npos) const noexcept {
		size_type v_size = v.size();
		pos = std::min(size(), npos);
		for (size_type i = pos; i-- > 0;) {
			if (subseq(i, v_size) == v) {
				return i;
			}
		}
		return npos;
	}

	friend bool operator==(sequence lhs, sequence rhs) noexcept {
		if (lhs.size() != rhs.size()) {
			return false;
		}
		return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), Traits::eq);
	}

	friend bool operator!=(sequence lhs, sequence rhs) noexcept {
		return !(lhs == rhs);
	}

	friend bool operator<(sequence lhs, sequence rhs) noexcept {
		return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), Traits::lt);
	}

	friend bool operator>(sequence lhs, sequence rhs) noexcept {
		return rhs < lhs;
	}

	friend bool operator<=(sequence lhs, sequence rhs) noexcept {
		return !(lhs > rhs);
	}

	friend bool operator>=(sequence lhs, sequence rhs) noexcept {
		return !(rhs < lhs);
	}

private:
	static sequence make_seq(const_iterator lptr, const_iterator rptr) {
		sequence s;
		s.lptr = lptr;
		s.rptr = rptr;
		return s;
	}
};

namespace std {
	template<class T, class Traits>
	struct hash<sequence<T, Traits>> {
		std::size_t operator()(const sequence<T, Traits> &s) const noexcept {
			size_t myhash = 0;
			std::hash<T> hasher{};
			for (auto it = s.begin(); it != s.end(); ++it) {
				myhash ^= hash(*it) + 0x9e479b9 + (seed << 6);
			}
			return myhash;
		}
	};
}
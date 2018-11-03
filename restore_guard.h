#ifndef SET_RESTORE_GUARD
#define SET_RESTORE_GUARD

#include <cassert>

template<class T>
struct set_restore_guard {
	set_restore_guard();
	set_restore_guard(T& var, T const& new_value);
	set_restore_guard(set_restore_guard const& other) = delete;
	~set_restore_guard();

	set_restore_guard &operator=(set_restore_guard const &other) = delete;

	void set(T& var, T const& new_value);
	void restore();

	T const& old_value() const;
	explicit operator bool() const;
private:
	typename std::aligned_storage<sizeof(T), alignof(T)>::type old_data;
	T* dataptr;
	bool containsSmth;
};



template<class T>
inline set_restore_guard<T>::set_restore_guard() { //done
	containsSmth = false;
}

template<class T>
inline set_restore_guard<T>::set_restore_guard(T & var, T const & new_value) { //done
	dataptr = &var;
	new(reinterpret_cast<T *>(&old_data)) T(var);
	var = new_value;
	containsSmth = true;
}

template<class T>
inline set_restore_guard<T>::~set_restore_guard() { //done
	new (dataptr) T(reinterpret_cast<T const &>(old_data));
	dataptr = nullptr;
	containsSmth = false;
}

template<class T>
inline void set_restore_guard<T>::set(T & var, T const & new_value) {
	if (containsSmth) {
		restore();
	}
	dataptr = &var;
	new(reinterpret_cast<T *>(&old_data)) T(var);
	//old_data = var;
	new (dataptr) T(new_value);
	containsSmth = true;
}

template<class T>
inline void set_restore_guard<T>::restore() { //done
	//assert(containsSmth);
	new (dataptr) T(reinterpret_cast<T const &>(old_data));
	dataptr = nullptr;
	containsSmth = false;
}

template<class T>
inline T const & set_restore_guard<T>::old_value() const { //done
	assert(containsSmth);
	return reinterpret_cast<T const &>(old_data);
}

template<class T>
inline set_restore_guard<T>::operator bool() const { //done
	return containsSmth;
}
#endif // !SET_RESTORE_GUARD
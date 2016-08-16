#pragma once

#include <cstddef>
#include <cstdlib>

namespace util {
  // Because simulation uses a different vector than client.
  template<class vec31, class vec32>
  bool vec_eq(const vec31& lhs, const vec32& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
  }

  template <class T>
  int c_comparand(const void* lhs, const void* rhs) {
    const T* lhu = (const T*)lhs;
    const T* rhu = (const T*)rhs;
    return lhu->m_id - rhu->m_id;
  }

  template <class T>
  const T* c_binsearch(const T* key, const T* ptr_array, size_t count) {
    return reinterpret_cast<const T*>(std::bsearch(key, ptr_array, count, sizeof(T), c_comparand<T>));
  }

  template <class T>
  void c_qsort(T* ptr_array, size_t count) {
    std::qsort(ptr_array, count, sizeof(T), c_comparand<T>);
  }
}

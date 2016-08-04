#pragma once

#include <cstddef>


namespace util {
  template<class type>
  bool id_comparand(const type& lhs, const type& rhs) {
    return rhs.m_id > lhs.m_id;
  }

  template<class type>
  const type* id_binsearch(const type* d, size_t size, uint32_t id) {
    size_t mid;
    size_t min = 0;
    size_t max = size;
    while (min <= max) {
      mid = min + (max - min) / 2;
      if (d[mid].m_id == id) {
        return &d[mid];
      }

      if (d[mid].m_id < id) {
        min = mid + 1;
      }
      else {
        max = mid - 1;
      }
    }
    return nullptr;
  }
}

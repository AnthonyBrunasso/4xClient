#pragma once

#include <cstddef>


namespace util {
  // std binary search doesn't give me an iterator to an item.
  // It is forcing me to write this.
  template<class type, class compare>
  const type* id_binsearch(const type* d, size_t size, uint32_t id, compare comp) {
    size_t mid;
    size_t min = 0;
    size_t max = size;
    while (min <= max) {
      mid = (max - min) / 2;
      if (d[mid].m_id == id) {
        return &d[mid];
      }

      // value is greater than.
      if (comp(id, d[mid])) {
        min = mid + 1;
      }
      // value is less than.
      else {
        max = mid - 1;
      }
    }

    return nullptr;
  }
}

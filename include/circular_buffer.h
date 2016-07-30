#pragma once

#include <cstdint>

template<typename T>
class CBuffer {
public:
  CBuffer(uint32_t sz) : 
      m_max_size(sz), 
      m_point(0) { 
    m_data = new T[m_max_size];
  };

  ~CBuffer() { delete [] m_data; };

  // Insert a point at the current location indicated by point
  void insert(const T& point) {
    m_data[m_point++] = point;
    if (m_size < m_max_size) ++m_size;
    if (m_point >= m_max_size) m_point = 0;
  };

  uint32_t size() { return m_size; };
  const T* data() { return m_data; };

private:
  T* m_data;

  uint32_t m_size;
  uint32_t m_max_size;
  // The index pointer of the buffer
  uint32_t m_point;
};

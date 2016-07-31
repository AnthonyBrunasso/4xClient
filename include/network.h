
#pragma once

#include <cstdint>

const uint32_t WRITE_BUFFER_HEADER = sizeof(uint32_t);

namespace network {
  // queue messages for transmission by future calls to write_socket()
  void queue_message(const char* buffer, size_t buffer_len);
  // bypass using write_socket() and read_socket()
  // messages are removed from the write queue
  void read_message(bool& message_read, char*& buffer, size_t& buffer_len);

  int init_network(const char* host, short port);
  void stop_network();
  void alloc_read_buffer(size_t read_buffer_size);
  void dealloc_read_buffer();

  bool read_socket(bool& message_read, char*& buffer, size_t& buffer_len);
  bool write_socket();
}

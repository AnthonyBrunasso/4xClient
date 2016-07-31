
#pragma once

#include <cstdint>

namespace network {
  // Optional initialization to remote peer
  int init_transport(const char* host, short port);
  void stop_transport();
  // Required allocation/deallocation
  void alloc_read_buffer(size_t read_buffer_size);
  void dealloc_read_buffer();

  // queue messages for transmission by future calls to write_socket()
  void queue_message(const char* buffer, size_t buffer_len);
  // pumps the transport (if created)
  // or reads from the write queue (if no transport exists)
  // false is returned when no messages exist
  bool update(bool& message_read, char*& buffer, size_t& buffer_len);
}

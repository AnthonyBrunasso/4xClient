
#pragma once

#include <cstdint>
#include <cstddef>
#include <deque>

const size_t BUFFER_LEN = 256;
const uint32_t WRITE_BUFFER_HEADER = sizeof(uint32_t);
struct WriteBuffer
{
  char* buffer;
  uint32_t buffer_len;
  uint32_t write_offset = { 0 };
};
struct ReadBuffer
{
  char buffer[BUFFER_LEN];
  uint32_t read_offset = { 0 };
};

namespace network {
  // queue messages for transmission by future calls to write_socket()
  void queue_message(const char* buffer, size_t buffer_len);
  // bypass using write_socket() and read_socket()
  // messages are removed from the write queue
  void read_message(bool& message_read, char*& buffer, size_t& buffer_len);

  int init_network(const char* host, short port);
  bool read_socket(bool& message_read, char*& buffer, size_t& buffer_len);
  bool write_socket();
}


#include "network.h"
#include "x_socket.h"

namespace network {
  ReadBuffer s_read_state;
  int s_socket;
  std::deque<WriteBuffer> s_send_queue;

  void queue_message(const char* buffer, size_t buffer_len) {
    WriteBuffer game_command;
    game_command.buffer_len = buffer_len + WRITE_BUFFER_HEADER;
    game_command.buffer = new char[game_command.buffer_len];
    memcpy(game_command.buffer, &buffer_len, WRITE_BUFFER_HEADER);
    memcpy(game_command.buffer + WRITE_BUFFER_HEADER, buffer, buffer_len);
    s_send_queue.push_back(game_command);
  }

  void read_message(bool& message_read, char*& buffer, size_t& buffer_len)
  {
    size_t bytes = 0;
    message_read = false;
    if (s_send_queue.empty()) return;

    message_read = true;
    WriteBuffer command = s_send_queue.front();
    s_send_queue.pop_front();

    buffer_len = command.buffer_len - WRITE_BUFFER_HEADER;
    buffer = s_read_state.buffer;
    memcpy(s_read_state.buffer, command.buffer + WRITE_BUFFER_HEADER, buffer_len);

    delete[] command.buffer;
  }

  int init_network(const char* host, short port) {
    static int network_init = x_socket::init();
    if (!s_socket)
    {
      s_socket = x_socket::create_tcp();
      x_socket::connect(s_socket, host, port);
      x_socket::nonblocking(s_socket);
    }
    return s_socket;
  }

  bool BufferSent(const WriteBuffer& buffer) {
    return buffer.write_offset == buffer.buffer_len;
  }

  uint32_t MessageLength(const char buffer[])
  {
    const uint32_t* header = (const uint32_t*)buffer;
    return *header + WRITE_BUFFER_HEADER;
  }

  bool read_socket(bool& message_read, char*& buffer, size_t& buffer_len) {
    uint32_t bytes_read = 0;

    if (s_read_state.read_offset < WRITE_BUFFER_HEADER)
    {
      bytes_read = x_socket::read(s_socket, s_read_state.buffer + s_read_state.read_offset, WRITE_BUFFER_HEADER - s_read_state.read_offset);
      if (!bytes_read) return false;
      s_read_state.read_offset += bytes_read;
      return true;
    }

    uint32_t msg_len = MessageLength(s_read_state.buffer);
    if (s_read_state.read_offset >= msg_len) {
      message_read = true;
      buffer = s_read_state.buffer + WRITE_BUFFER_HEADER;
      buffer_len = s_read_state.read_offset - WRITE_BUFFER_HEADER;
      s_read_state.read_offset = 0;
      return true;
    }

    bytes_read = x_socket::read(s_socket, s_read_state.buffer + s_read_state.read_offset, msg_len - s_read_state.read_offset);
    if (!bytes_read) return false;
    s_read_state.read_offset += bytes_read;

    return true;
  }

  bool write_socket() {
    if (s_send_queue.empty()) return false;

    WriteBuffer& current = s_send_queue.front();
    if (BufferSent(current)) {
      delete[] current.buffer;
      s_send_queue.pop_front();
      return true;
    }

    uint32_t write_buffered = x_socket::write(s_socket, current.buffer, current.buffer_len);
    if (write_buffered == 0) return false;
    current.write_offset += write_buffered;

    return true;
  }
}
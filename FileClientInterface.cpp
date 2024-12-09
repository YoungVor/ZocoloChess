#include "FileClientInterface.h"
#include "ChessGame.h"
#include "ClientInterface.h"
#include "serializer_utils.h"
#include "system_utils.h"
#include <__algorithm/ranges_find_if_not.h>
#include <__algorithm/ranges_for_each.h>
#include <fstream>
#include <istream>
#include <ostream>
#include <sstream>
#include <streambuf>

std::string FileClientIF::get_path() {
  if (path.empty()) {
  char* envValue = getenv("PATH");
    if (envValue != nullptr) {
      path = PATH_DEFAULT;
    } else {
      path = envValue;
    }
  }
  return path;
};

std::string FileClientIF::file_path_from_id(boost::uuids::uuid id) {
  return get_path() + "chess_" + boost::uuids::to_string(id) + ".zo";
}

int round_to_nearest_block(int size) {
  if (block_size < 0) {
   char* envValue = getenv("BLOCK_SIZE");
   if (envValue != nullptr) {
     block_size = atoi(envValue);
   } else {
     block_size = BLOCK_SIZE_DEFAULT;
   }
  }
  if (size % block_size != 0) {
    return (size / block_size) * block_size + block_size;
  }
  return block_size;
}

// If cur_msg_size is non-zero, we are still reading data into the buffer and will need to copy the data to the new buffer
ClientError FileClientIF::resize_buffer(int message_size) {
  // TODO: set upper limit and force 'list' to use paging queries
  //

  if (message_size > buffer_size) {
    int new_size = round_to_nearest_block(message_size);
    log(TRACE, std::format("file client increasing buffer size to {} from {}", new_size, buffer_size));
    auto old_buffer = buffer.release();
    buffer.reset(new char[new_size]);
    buffer_size = new_size;
    if (cur_msg_size > 0 ) {
      // copy data from previous buffer
      memcpy(buffer.get(), old_buffer, cur_msg_size);
    }
    free(old_buffer);

  }
  return ClientError::success;
}
// caller provides the block of data
// client will reallocate it if more data is needed
ClientError FileClientIF::read_board(boost::uuids::uuid id, char **ret_buffer_ptr, int &ret_buffer_len) {
  cur_msg_size = 0;
  std::string fpath = file_path_from_id(id);
  if (!std::filesystem::exists(fpath)) {
    return ClientError::does_not_exist;
  }
  std::ifstream infile;
  infile.open(fpath, std::ios_base::binary | std::ios_base::in ); // clears contents first
  infile.seekg(0, std::ios_base::end);
  int in_size = infile.tellg();
  infile.seekg(0, std::ios_base::beg);
  if (in_size > buffer_size) {
    resize_buffer(in_size);
  }
  infile.read(buffer.get(), in_size);
  cur_msg_size = in_size;
  *ret_buffer_ptr = buffer.get();
  ret_buffer_len = cur_msg_size;

  log(TRACE, std::format("file client read {} bytes from file {} ", in_size, fpath));
  return ClientError::success;
};

// TODO: caller should use the client's buffer
// caller provides the block of data and the id
ClientError FileClientIF::write_board(boost::uuids::uuid id, const char *buffer_ptr, int buffer_len) {
    // Test Serialize
  std::ofstream outfile;
  std::string file_path = file_path_from_id(id);
  outfile.open(file_path, std::ios_base::binary | std::ios_base::out ); // clears contents first
  log(TRACE, "file open:" + std::to_string(outfile.is_open()));
  outfile.write(buffer_ptr, buffer_len);
  outfile.close();

  log(TRACE, std::format("file client wrote {} bytes to file {}", buffer_len, file_path));

  return ClientError::success;
};

ClientError FileClientIF::list_games(char **ret_buffer_ptr, int &ret_buffer_size, int startIndex, int length) {
  cur_msg_size = 0;
  if (!std::filesystem::exists(get_path())) {
    return ClientError::does_not_exist;
  }
  auto it = std::filesystem::directory_iterator(std::filesystem::path(get_path()));
  int cur_msg_size = 0;
  ostreambuf<char> buffer_ostream(buffer.get(), buffer_size);
  std::ostream ostream(&buffer_ostream);
  std::ranges::for_each(it, [this, &ostream, &buffer_ostream](const auto &dir_entry) {
    if (this->cur_msg_size + dir_entry.path().string().size() + 4 > this->buffer_size) {
      this->resize_buffer(buffer_size * 2);
      buffer_ostream.reset_ptr(buffer.get(), this->buffer_size);
    }
    log(TRACE, std::format("list_games adding entry.  inc:{} ptr:{}" , sizeof(char*) * this->cur_msg_size, this->buffer.get()));
    ostream << " - " << dir_entry.path().string() << "\n";
    log(TRACE, this->buffer.get());
    this->cur_msg_size += dir_entry.path().string().size() + 4;
  });
  ostream << '\0';
  //this->buffer.get()[cur_msg_size++] = '\0'; // complete the string
  TRACE("list_games buffer:" + std::string(buffer.get()));
  *ret_buffer_ptr = buffer.get();
  ret_buffer_size = cur_msg_size;
  return ClientError::success;
}

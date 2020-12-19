#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity) {
    if (capacity >= 0) {
        this->capacity = capacity;
    } else {
        this->capacity = 1;
        throw "Invalid capacity for byte stream";
    }
}

size_t ByteStream::write(const string &data) {
    unsigned int bytesWritten = 0;
    auto bytesLeft = remaining_capacity();
    remainingStream += data;

    if (bytesLeft == 0) {
        return 0;
    }

    if (bytesLeft >= remainingStream.size()) {
        for (auto ch : remainingStream) {
            stream.emplace_back(ch);
        }
        bytesWritten = remainingStream.size();
        remainingStream.clear();
    } else {
        for (int i = 0; i != bytesLeft; ++i) {
            stream.emplace_back(remainingStream[i]);
        }
        remainingStream.erase(remainingStream.begin(), remainingStream.begin() + bytesLeft);
        bytesWritten = bytesLeft;
    }

    return bytesWritten;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    std::string copied;
    for (int i = 0; i != len; ++i) {
        copied += stream[i];
    }

    return copied;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) { stream.erase(stream.begin(), stream.begin() + len); }

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    std::string copied = peek_output(len);
    pop_output(len);

    return copied;
}

void ByteStream::end_input() {
    if (remainingStream.empty()) {
        std::puts("Byte stream ended");
    }
}

bool ByteStream::input_ended() const { return remainingStream == ""; }

size_t ByteStream::buffer_size() const { return stream.size(); }

bool ByteStream::buffer_empty() const { return stream.empty(); }

bool ByteStream::eof() const { return false; }

size_t ByteStream::bytes_written() const { return bytesWritten; }

size_t ByteStream::bytes_read() const { return bytesRead; }

size_t ByteStream::remaining_capacity() const { return capacity - stream.size(); }

#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    : _output(capacity), _capacity(capacity), lastByteWritten(0), segmentSize(0) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    segmentSize = sizeof(data);

    if (index == lastByteWritten + 1) {    // if current received segment is next in order
        if (allowWritingToBuffer(data)) {  // check if there's enough space on buffer to write segment
            _output.write(data);
            lastByteWritten += segmentSize;  // update lastByteWritten to check next segment
        }
    } else {
        if (segmentSize <= freeMemory()) {  // make sure capacity is not exceeded
            storedInMemory.emplace_back(std::make_pair(data, index));
        }
    }

    if (segmentSize > freeMemory()) {
        std::sort(storedInMemory.begin(),
                  storedInMemory.end(),
                  [](std::pair<std::string, size_t> &rhs, std::pair<std::string, size_t> &lhs) {
                      return rhs.second > lhs.second;
                  });

        for (const auto & segment : storedInMemory){
            _output.write(segment.first);
        }
        lastByteWritten = index + segmentSize;
    }
}

size_t StreamReassembler::unassembled_bytes() const { return storedInMemory.size() * sizeof(storedInMemory[0].first); }

bool StreamReassembler::empty() const { return storedInMemory.empty(); }

bool StreamReassembler::allowWritingToBuffer(const std::string &segment) const {
    return sizeof(segment) >= _output.remaining_capacity();
}

size_t StreamReassembler::freeMemory() { return _capacity - unassembled_bytes() - _output.buffer_size(); }

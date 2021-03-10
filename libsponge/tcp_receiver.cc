#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

//template <typename... Targs>
//void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    if (seg.header().syn){
        isn = seg.header().seqno;
        checkpoint = 0;
    }

    auto index = unwrap(seg.header().seqno, isn, checkpoint);
    checkpoint = index;

    if (not seg.payload().str().empty())
    {
        _reassembler.push_substring(static_cast<std::string>(seg.payload().str()), index, seg.header().fin);
    }
//
//    if (seg.header().fin){
//        ;
//    }
}

optional<WrappingInt32> TCPReceiver::ackno() const{
    if (isn.raw_value()){
        return std::optional<WrappingInt32>{ WrappingInt32{static_cast<uint32_t >(_reassembler.getLastByteWritten())} };
    }

    return std::optional<WrappingInt32>{};
}


size_t TCPReceiver::window_size() const { return _capacity - _reassembler.stream_out().buffer_size(); }

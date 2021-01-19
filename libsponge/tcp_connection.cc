#include "tcp_connection.hh"

#include <iostream>

// Dummy implementation of a TCP connection

// For Lab 4, please replace with a real implementation that passes the
// automated checks run by `make check`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

size_t TCPConnection::remaining_outbound_capacity() const { return _receiver.window_size(); }

size_t TCPConnection::bytes_in_flight() const { return _sender.bytes_in_flight(); }

size_t TCPConnection::unassembled_bytes() const { return _receiver.unassembled_bytes(); }

size_t TCPConnection::time_since_last_segment_received() const {
    if (_sender.getElapsedTime() > timeAtSegmentArrival) {
        return _sender.getElapsedTime() - timeAtSegmentArrival;
    } else {
        return std::numeric_limits<uintmax_t>::max() - timeAtSegmentArrival + _sender.getElapsedTime();
    }
}

void TCPConnection::segment_received(const TCPSegment &seg) {
    timeAtSegmentArrival = _sender.getElapsedTime();
    if (!seg.header().rst)
    {
        _receiver.segment_received(seg);
    } else {
        connectionAlive = false;
    }
}

bool TCPConnection::active() const { return connectionAlive or _linger_after_streams_finish; }

size_t TCPConnection::write(const string &data) {
    DUMMY_CODE(data);
    return {};
}

//! \param[in] ms_since_last_tick number of milliseconds since the last call to this method
void TCPConnection::tick(const size_t ms_since_last_tick) { _sender.tick(ms_since_last_tick); }

void TCPConnection::end_input_stream() {}

void TCPConnection::connect() {
    TCPSegment seg;
    seg.payload().remove_prefix(seg.payload().size());
    seg.header().syn = true;
    _sender.segments_out().push(seg);
}

TCPConnection::~TCPConnection() {
    try {
        if (active()) {
            cerr << "Warning: Unclean shutdown of TCPConnection\n";

            TCPSegment rst;
            rst.payload().remove_prefix(rst.payload().size());
            rst.header().rst = true;
            rst.header().syn = false;
            rst.header().fin = false;

            _segments_out.push(rst);
        }
    } catch (const exception &e) {
        std::cerr << "Exception destructing TCP FSM: " << e.what() << std::endl;
    }
}

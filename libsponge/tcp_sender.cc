#include "tcp_sender.hh"

#include "tcp_helpers/tcp_config.hh"

// Dummy implementation of a TCP sender

// For Lab 3, please replace with a real implementation that passes the
// automated checks run by `make check_lab3`.

using namespace std;

//! \param[in] capacity the capacity of the outgoing byte stream
//! \param[in] retx_timeout the initial amount of time to wait before retransmitting the oldest outstanding segment
//! \param[in] fixed_isn the Initial Sequence Number to use, if set (otherwise uses a random ISN)
TCPSender::TCPSender(const size_t capacity, const uint16_t retx_timeout, const std::optional<WrappingInt32> fixed_isn)
    : _isn(fixed_isn.value_or(WrappingInt32{random_device()()}))
    , _initial_retransmission_timeout{retx_timeout}
    , _stream(capacity)
    , consecutiveRetransmissions(0)
    , segmentsStored(0, 0)
    , lastWindowSize(1)
    , elapsedTime(0)
    , timeAlive(0)
    , RTO{retx_timeout}
    , clock(1000000, std::bind(&TCPSender::callback, this)) {}

uint64_t TCPSender::bytes_in_flight() const {
    size_t bytes = 0;
    for (auto segment : segmentsStored) {
        bytes += segment.first.length_in_sequence_space();
    }
    return bytes;
}

void TCPSender::fill_window() {
    auto segmentSize = std::min(static_cast<uint16_t>(TCPConfig::MAX_PAYLOAD_SIZE), lastWindowSize) - 2;
    if (segmentSize <= 0) {
        segmentSize = 1;
    }
    TCPSegment toSend;
    Buffer content(_stream.read(segmentSize));
    toSend.parse(content);

    // something about setting up SYN FIN ACKNO

    _segments_out.push(toSend);
    segmentsStored[toSend] = std::make_pair(toSend.header().ackno, clock.getElapsedTime());
}

//! \param ackno The remote receiver's ackno (acknowledgment number)
//! \param window_size The remote receiver's advertised window size
void TCPSender::ack_received(const WrappingInt32 ackno, const uint16_t window_size) {
    std::vector<TCPSegment> acknowledged;
    lastWindowSize = window_size;

    for (auto segment : segmentsStored) {
        if (ackno - segment.second.first >= 0) {
            acknowledged.emplace_back(segment.first);
        } else {
            break;
        }
    }

    for (auto segment : acknowledged) {
        segmentsStored.erase(segment);
    }
    acknowledged.clear();

    if (window_size > 0) {
        fill_window();
    } else {
        send_empty_segment();
    }

    consecutiveRetransmissions = 0;
    RTO = _initial_retransmission_timeout;
    clock.startTimer(RTO);
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void TCPSender::tick(const size_t ms_since_last_tick) {
    timeAlive += ms_since_last_tick;

    for (auto unsent : segmentsStored){
        if (clock.getElapsedTime() - unsent.second.second > RTO){
                  _segments_out.push(unsent.first);
                  if (lastWindowSize > 0){
                      ++consecutiveRetransmissions;
                      RTO *= 2;
                  }
                  clock.startTimer(RTO);
//                  break;
        }
    }
}

unsigned int TCPSender::consecutive_retransmissions() const { return consecutiveRetransmissions; }

void TCPSender::send_empty_segment() {
    TCPSegment empty;
    empty.payload().remove_prefix(empty.payload().size());
    empty.header().syn = true;
    empty.header().fin = true;
    _segments_out.push(empty);
}

void TCPSender::callback(size_t milliseconds) {
    tick(milliseconds);
}

Clock::Clock() {}  // delegate

Clock::Clock(const std::uintmax_t period, std::function<void(size_t)> &&func)
    : period(period)
    , elapsedTime(0)
    , running(true)
    , th(&Clock::run, this)
    , RTOTimer(&Clock::timer, this)
    , runningTimer(false)
    , initial(0)
    , milliseconds(0)
    , function(func) {}

Clock::~Clock() {
    if (th.joinable()) {
        running = false;
        th.join();
    }

    if (RTOTimer.joinable()) {
        runningTimer = false;
        RTOTimer.join();
    }
}

void Clock::run() {
    while (running) {
        for (std::uintmax_t i = 0; i < period; ++i) {
        }

        ++elapsedTime;
    }
}

std::size_t Clock::getElapsedTime() const { return elapsedTime; }

void Clock::timer() {
//    initial = elapsedTime;
    while (this->runningTimer) {
        if (elapsedTime - initial >= milliseconds) {
            initial = elapsedTime;
            runningTimer = false;
            function(milliseconds);
        }
    }
}

void Clock::startTimer(const uintmax_t milliseconds) {
    this->milliseconds = milliseconds;
    this->runningTimer = true;
}

bool Clock::isTimerExpired() {
//    if (elapsedTime - initial > period) {
//        return true;
//    }
    return !runningTimer;
}
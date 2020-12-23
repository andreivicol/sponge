cmake_minimum_required(VERSION 3.9)
project(sponge)

set(CMAKE_CXX_STANDARD 17)

include_directories(libsponge)
include_directories(libsponge/util)
include_directories(tests)

add_executable(sponge
        apps/webget.cc
        doctests/address_dt.cc
        doctests/address_example_1.cc
        doctests/address_example_2.cc
        doctests/address_example_3.cc
        doctests/parser_dt.cc
        doctests/parser_example.cc
        doctests/socket_dt.cc
        doctests/socket_example_1.cc
        doctests/socket_example_2.cc
        doctests/socket_example_3.cc
        libsponge/util/address.cc
        libsponge/util/address.hh
        libsponge/util/buffer.cc
        libsponge/util/buffer.hh
        libsponge/util/eventloop.cc
        libsponge/util/eventloop.hh
        libsponge/util/file_descriptor.cc
        libsponge/util/file_descriptor.hh
        libsponge/util/parser.cc
        libsponge/util/parser.hh
        libsponge/util/socket.cc
        libsponge/util/socket.hh
        libsponge/util/tun.cc
        libsponge/util/tun.hh
        libsponge/util/util.cc
        libsponge/util/util.hh
        libsponge/byte_stream.cc
        libsponge/byte_stream.hh
        tests/byte_stream_capacity.cc
        tests/byte_stream_construction.cc
        tests/byte_stream_many_writes.cc
        tests/byte_stream_one_write.cc
        tests/byte_stream_test_harness.cc
        tests/byte_stream_test_harness.hh
        tests/byte_stream_two_writes.cc
        tests/test_err_if.hh
        tests/test_should_be.hh)

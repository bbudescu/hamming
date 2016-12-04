cmake_minimum_required(VERSION 3.5)
project(job_test VERSION 0.1 LANGUAGES CXX)

option(HAMMING_BUILD_TESTS "Disable before installing" ON)

add_subdirectory(libhamming)
add_subdirectory(hamming)

if(HAMMING_BUILD_TESTS)
    add_subdirectory(hamming_test)
endif(HAMMING_BUILD_TESTS)

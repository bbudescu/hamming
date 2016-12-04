cmake_minimum_required(VERSION 3.5)

# minimal googletest
add_subdirectory(3rdparty/gtest1.8)
install(DIRECTORY )

file(GLOB src_files src/*.*)
add_executable(hamming_test ${src_files})

target_link_libraries(hamming_test PRIVATE hamming gtest)
target_include_directories(hamming_test PRIVATE ${gtest_SOURCE_DIR}/include)

set_target_properties(hamming_test PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED ON
        DEBUG_POSTFIX d)

# @TODO: pass the cxx compile flags of the hamming_test to googletest if they
# @TODO: are customized (although it generally works without this, as well)

install(TARGETS hamming_test
        INCLUDES DESTINATION "include"
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        COMPONENT "development")

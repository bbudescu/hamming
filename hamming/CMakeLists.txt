cmake_minimum_required(VERSION 3.5)

file(GLOB src_files src/*.c*)

add_executable(hamming_bin ${src_files})
# @TODO: change out file name from "hamming_bin" to "hamming"

target_link_libraries(hamming_bin PRIVATE hamming)

set_target_properties(hamming_bin PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED ON
        DEBUG_POSTFIX d
        OUTPUT_NAME hamming)

install(TARGETS hamming_bin
        INCLUDES DESTINATION "include"
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        COMPONENT "development")

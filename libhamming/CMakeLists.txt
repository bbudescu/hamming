cmake_minimum_required(VERSION 3.5)

option(HAMMING_USE_OPENMP "Use OpenMP if available" ON)
option(HAMMING_WITH_INTRINSICS_WEIGHT "Include the compiler-intrinsic-based implementation of popcnt64" ON)
option(HAMMING_WITH_VANILLA_WEIGHT "Include the vanilla implementation of popcnt64" ON)
option(HAMMING_WITH_2x32_WEIGHT "Include the 2x32 implementation of popcnt64" ON)
option(HAMMING_WITH_LUT_WEIGHT "Include the lookup-table-based implementation of popcnt64" ON)
option(HAMMING_WITH_SPARSE_WEIGHT "Include an implementation of popcnt64 which is efficient for small numbers of 1s" ON)

# I know globbing in cmake for source files is sometimes frowned upon
# because no change to the cmake file is required when adding a new
# file, so the build system does not know when to rerun cmake by itself,
# but this way cmake files are much cleaner, whereas the user still
# has to do something (i.e. rerun cmake, instead of modifying the cmake file)
file(GLOB src_files src/*.c*) # should match .c, .cpp, .cxx
file(GLOB h_files include/hamming/*.h*) # should match .h, .hpp, .hxx
file(GLOB internal_h_files/hamming/internal/*.h*)

list(APPEND all_src_files ${src_files})
list(APPEND all_src_files ${h_files})
list(APPEND all_src_files ${internal_h_files})

if(HAMMING_USE_OPENMP)
    find_package(OpenMP REQUIRED)
    if(OPENMP_FOUND)
        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        # target_compile_options(hamming PRIVATE ${OpenMP_CXX_FLAGS})
        # apparently, on linux one can't specify the compile options after
        # defining the target...
    endif(OPENMP_FOUND)
endif (HAMMING_USE_OPENMP)

# message(STATUS "all_src_files: ${all_src_files}")

add_library(hamming SHARED ${all_src_files})

target_include_directories(hamming PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/>
        )

if(HAMMING_BUILD_TESTS)
    target_compile_definitions(hamming PUBLIC "EXPORT_INTERNALS")
endif(HAMMING_BUILD_TESTS)

#if not cross-compiling we can run the benchmark and choose the fastest at build time
if(HAMMING_WITH_VANILLA_WEIGHT)
    target_compile_definitions(hamming PUBLIC HAMMING_WITH_VANILLA)
endif(HAMMING_WITH_VANILLA_WEIGHT)

if(HAMMING_WITH_2x32_WEIGHT)
    target_compile_definitions(hamming PUBLIC HAMMING_WITH_2x32)
endif(HAMMING_WITH_2x32_WEIGHT)

if(HAMMING_WITH_SPARSE_WEIGHT)
    target_compile_definitions(hamming PUBLIC HAMMING_WITH_SPARSE)
endif(HAMMING_WITH_SPARSE_WEIGHT)

if(HAMMING_WITH_LUT_WEIGHT)
    target_compile_definitions(hamming PUBLIC HAMMING_WITH_LUT)
endif(HAMMING_WITH_LUT_WEIGHT)

if(HAMMING_WITH_INTRINSICS_WEIGHT)
    target_compile_definitions(hamming PUBLIC HAMMING_WITH_INTRINSICS)
endif(HAMMING_WITH_INTRINSICS_WEIGHT)

# target_compile_options(libhamming PUBLIC "-std=c++11")
set_target_properties(hamming PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED ON
        DEBUG_POSTFIX d)

install(TARGETS hamming EXPORT hamming-targets
        INCLUDES DESTINATION "include"
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        COMPONENT "development")

install(EXPORT hamming-targets
        DESTINATION "cmake"
        COMPONENT "development")


install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/hamming"
        DESTINATION "include"
        COMPONENT "development"
        PATTERN "*internal*"
        EXCLUDE)

# string (REPLACE ";" " " install_h_files "${h_files}")

# should be the same as above, but this works, that doesn't
#file(GLOB install_h_files "${CMAKE_CURRENT_SOURCE_DIR}/include/hamming/*.h*") # should match .h, .hpp, .hxx
#
#install(FILES ${install_h_files}
#        DESTINATION "include"
#        COMPONENT "development")

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/cmake/hammingConfig.cmake"
        DESTINATION .
        COMPONENT "development")

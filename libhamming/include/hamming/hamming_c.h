#pragma once

// one must make sure to
// #include <cstddef>
// before including this file

// the reason for which this is required, is because we also wrap this file
// into a namespace. Most people would use the high level api anyway, and
// for the purpose of this test, it's an overkill to create another file
// just for this reason (one single include...).

#ifdef _WIN32
// we use _WIN32 instead of _MSC_VER, as mingw, cygwin understand __declspec
#ifdef HAMMING_EXPORT
#define HAMMING_API __declspec(dllexport)
#else
#define HAMMING_API __declspec(dllimport)
#endif
#define HAMMING_CALL __cdecl
#else
// intel compiler is gcc-compatible on unix
#define HAMMING_API __attribute__ ((visibility ("default")))
#define HAMMING_CALL
#endif // _MSC_VER

#ifdef __cplusplus
// C ABI & name mangling is compatible across compilers
extern "C"
{
#endif

#define HAMMING_VERSION 100

typedef enum
{
    HAMMING_STATUS_SUCCESS                      = 0,
    HAMMING_STATUS_BAD_PARAM_STR_1              = 1,
    HAMMING_STATUS_BAD_PARAM_STR_2              = 2,
    HAMMING_STATUS_BAD_PARAM_DISTANCE           = 3,
    HAMMING_STATUS_IMPLEMENTATION_NOT_AVAILABLE = 4,
    HAMMING_STATUS_UNKNOWN_IMPLEMENTATION       = 5
} hamming_status_t;

// select implementation (useful for benchmarking)
// default chooses compiler-intrinsic-based implementation, if available
// or falls back on vanilla, 2x32 or lut, in this order, depending on which
// is available. Vanilla should be faster than lookup-table on powerfull
// processors, as modern arithmetic instructions take less than 1 cycle
typedef enum
{
#if defined(HAMMING_WITH_VANILLA) || defined(HAMMING_WITH_2x32) || defined(HAMMING_WITH_LUT) || defined(HAMMING_WITH_SPARSE) || defined(HAMMING_WITH_INTRINSICS)
    HAMMING_IMPL_DEFAULT = 0,
#endif

#ifdef HAMMING_WITH_VANILLA
    HAMMING_IMPL_VANILLA = 1,
#endif

#ifdef HAMMING_WITH_2x32
    HAMMING_IMPL_2x32 = 2,
#endif

#ifdef HAMMING_WITH_LUT
    HAMMING_IMPL_LUT = 3,
#endif

#ifdef HAMMING_WITH_SPARSE
    HAMMING_IMPL_SPARSE = 4
#endif
} hamming_impl_t;

// NOTE 1: we return primitive status codes, as we don't want to throw
// exceptions across shared object boundaries

HAMMING_API hamming_status_t HAMMING_CALL hamming_distance(const unsigned char str1[],
                                                           const unsigned char str2[],
                                                           const size_t n_bytes,
                                                           size_t* distance,
                                                           hamming_impl_t = HAMMING_IMPL_DEFAULT);

HAMMING_API const char* HAMMING_CALL get_hamming_error_string(hamming_status_t);

HAMMING_CALL int HAMMING_CALL hamming_version();

#ifdef __cplusplus
} // ~extern "C"{
#endif
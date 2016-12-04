#pragma once

#include <cstddef>
#include <hamming/hamming_c.h>
#include <type_traits>

#ifdef EXPORT_INTERNALS
#define INTERNAL_HAMMING_API HAMMING_API
#else // ifdef EXPORT_INTERNALS
// we want to hide the internal symbols in release mode
#ifdef _WIN32 // true for msvc, gcc on windows (cygwin, mingw)
// default linkage for symbols on windows is hidden
#define INTERNAL_HAMMING_API
#else // ifdef _WIN32
// gcc, icc use this syntax for linker stuff
#define INTERNAL_HAMMING_API __attribute__ ((visibility ("hidden")))
#endif // ifdef _WIN32
#endif // ifdef EXPORT_INTERNALS

// @TODO: all functions here are supposed to have the inline specifier, but
// @TODO: they are not exported any more when inline is added, although the
// @TODO: gcc documentation states that the default visibility for inlines is
// @TODO: public; the inline specifier is not very important nowadays anyway,
// @TODO: and we expect the compiler to inline the functions, at least within
// @TODO: the same module

// internal implementations
#ifdef HAMMING_WITH_VANILLA
INTERNAL_HAMMING_API /*inline*/ unsigned int HAMMING_CALL popcount64_vanilla(const unsigned long long int&);
#endif

#ifdef HAMMING_WITH_2x32
// helper for popcount64_2x32
INTERNAL_HAMMING_API /*inline*/ unsigned int HAMMING_CALL popcount32(const unsigned int&);
INTERNAL_HAMMING_API /*inline*/ unsigned int HAMMING_CALL popcount64_2x32(const unsigned long long int&);
#endif

#ifdef HAMMING_WITH_SPARSE
INTERNAL_HAMMING_API /*inline*/ unsigned int HAMMING_CALL popcount64_sparse(const unsigned long long int&);
#endif

#ifdef HAMMING_WITH_LUT
// helper for popcount64_lut
// defined for IntegralType=unsigned char and short
template<typename IntegralType> INTERNAL_HAMMING_API
typename std::enable_if<std::is_integral<IntegralType>::value, IntegralType>::type* HAMMING_CALL
init_lut(void);

INTERNAL_HAMMING_API /*inline*/ unsigned int HAMMING_CALL popcount64_lut(const unsigned long long int&);
#endif

#if defined(HAMMING_WITH_VANILLA) || defined(HAMMING_WITH_2x32) || defined(HAMMING_WITH_LUT) || defined(HAMMING_WITH_SPARSE)
// selector for internal implementation
INTERNAL_HAMMING_API /*inline*/ unsigned int HAMMING_CALL popcount64_internal(const unsigned long long int&);
#endif

#ifdef HAMMING_WITH_INTRINSICS
// platform specific implementations
// NOTE: we prefer the ops that work on the largest number of bytes (8), as
//       it's probably faster than us calling ops on 1, 2 or 4 bytes multiple
//       times (we give the compiler that much credit...)

#ifdef __INTEL_COMPILER
INTERNAL_HAMMING_API /*inline*/ unsigned int HAMMING_CALL popcount64_icc(const unsigned long long int&);
#elif defined(__GNUC__)
INTERNAL_HAMMING_API /*inline*/ unsigned int HAMMING_CALL popcount64_gcc(const unsigned long long int&);
#elif defined(_MSC_VER) && (defined(_M_AMD64) || defined(_M_IX86))
INTERNAL_HAMMING_API /*inline*/ bool HAMMING_CALL has_popcnt();
INTERNAL_HAMMING_API /*inline*/ unsigned int HAMMING_CALL popcount64_msvc(const unsigned long long int&);
#endif // compiler

#endif // defined(HAMMING_WITH_INTRINSICS)

#if defined(HAMMING_WITH_VANILLA) || defined(HAMMING_WITH_2x32) || defined(HAMMING_WITH_LUT) || defined(HAMMING_WITH_SPARSE) || defined(HAMMING_WITH_INTRINSICS)
// final implementation selector
INTERNAL_HAMMING_API /*inline*/ unsigned int HAMMING_CALL popcount64(const unsigned long long int&);
#endif



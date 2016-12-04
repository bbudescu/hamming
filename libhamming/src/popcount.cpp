//# several implementations for popcount (hamming weight) of uint64

#include <hamming/internal/popcount.h>
#include <limits>

#if defined(_MSC_VER) && (defined(_M_AMD64) || defined(_M_IX86))
#include <array>
#include <bitset>
#endif // defined(_MSC_VER) && (defined(_M_AMD64) || defined(_M_IX86))

using namespace std;

// https://chessprogramming.wikispaces.com/Population+Count

#ifdef HAMMING_WITH_VANILLA
// the implementation with fewer arithmetic ops should be faster on modern
// CPUs, even though it has a multiplication, which is usually less than
// 5 times slower than simple ops (we could replace it by 5 simple ops)

INTERNAL_HAMMING_API /*inline*/ unsigned int HAMMING_CALL popcount64_vanilla(const unsigned long long int& x_param)
{
    unsigned long long int x = x_param;
    x -= (x >> 1) & 0x5555555555555555ULL;
    x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
    x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0fULL;
    return static_cast<unsigned char>((x * 0x0101010101010101ULL) >> 56);
}
#endif // HAMMING_WITH_VANILLA

#ifdef HAMMING_WITH_2x32
INTERNAL_HAMMING_API /*inline*/ unsigned int HAMMING_CALL popcount32(const unsigned int& x_param)
{
    unsigned int x = x_param;
    x = x  - ((x >> 1)  & 033333333333U) - ((x >> 2)  & 011111111111U);
    x = (x +  (x >> 3)) & 030707070707U;
    return static_cast<unsigned char>(x % 63);
}

// @TODO: benchmark this against popcount64. Probably slower, as %
// @TODO: takes quite some time compared to multiplication; then again,
// @TODO: most processors optimize for 32bit integers...
INTERNAL_HAMMING_API /*inline*/ unsigned int HAMMING_CALL popcount64_2x32(const unsigned long long int& x)
{
    // return popcount32(static_cast<unsigned int>(x & 0xFFFFFFFF)) +
    //        popcount32(static_cast<unsigned int>(x >> 32));
    const unsigned int* u32 = reinterpret_cast<const unsigned int*>(&x);
    return popcount32(u32[0]) + popcount32(u32[1]);
}
#endif // HAMMING_WITH_2x32

#ifdef HAMMING_WITH_SPARSE
INTERNAL_HAMMING_API /*inline*/ unsigned int HAMMING_CALL popcount64_sparse(const unsigned long long int& x_param)
{
    unsigned int count = 0;
    for (unsigned long long int x = x_param; x; x &= x - 1)
        ++count;
    return count;
}
#endif

#ifdef HAMMING_WITH_LUT

// if T is unsigned short, we will have a 64KB lookup table, which should
// fit in most L2 caches
// if T is unsigned char, we will have a 256B lut, which would fit in
// L1 caches, but the number of lookups would be doubled; anyway, L1 cache
// latency is more than twice as low as the one of L2 cache, but there are
// also more additions; I think the only way to figure out which is better
// is through a benchmark on the deployment platform
template<typename IntegralType> INTERNAL_HAMMING_API
typename std::enable_if<std::is_integral<IntegralType>::value, IntegralType>::type*
HAMMING_CALL init_lut(void)
{
    // in C++ 14 this function can be constexpr; C++11 doesn't allow local
    // variables and loops; constexpr would allow the compile-time init
    static IntegralType lut[static_cast<long long int>(numeric_limits<IntegralType>::max()) -
                            static_cast<long long int>(numeric_limits<IntegralType>::min()) + 1];

    // if we wouldn't use openmp, which requires a signed integer loop var,
    // we would have looped using a pointer, like this:
    // for (IntegralType* k = lut; k < lut + sizeof(lut) / sizeof(lut[0]);++ k)
    //     //...
    //     ++(*k);
#pragma omp parallel for
    for (int i=0; i <= sizeof(lut) / sizeof(lut[0]); ++i)
        for (IntegralType x = static_cast<IntegralType>(i); x; x &= x - 1)
            ++lut[i];

    return lut;
}

// template specialization for byte-sized lut generation
// also instantiates the template for unsigned char
template<> INTERNAL_HAMMING_API unsigned char* HAMMING_CALL init_lut<unsigned char>(void)
{
    static unsigned char lut[256] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3,
                                     3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4,
                                     3, 4, 4, 5, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3,
                                     3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5,
                                     3, 4, 4, 5, 4, 5, 5, 6, 1, 2, 2, 3, 2, 3,
                                     3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4,
                                     3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3,
                                     3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
                                     3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6,
                                     6, 7, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4,
                                     3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4,
                                     4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5,
                                     3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5,
                                     5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 2, 3, 3, 4,
                                     3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4,
                                     4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
                                     3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6,
                                     6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7,
                                     6, 7, 7, 8};
    return lut;
};

// explicit instantiation
template INTERNAL_HAMMING_API unsigned short* HAMMING_CALL init_lut<unsigned short>(void);


// // helper type for strict-aliasing-safe implementation of lookup
// union U64
// {
//     unsigned long long u64;
//     unsigned short u16[4];
// };


INTERNAL_HAMMING_API /*inline*/ unsigned int HAMMING_CALL popcount64_lut(const unsigned long long int& x)
{
    // NOTE 1: if C++11 (MSVC2015 / gcc 4.6), magic statics should make sure
    // the init code is only called once; however, even if the compiler is
    // not standard compliant, the result will still be correct, as the
    // concurrent threads will try to write the same values at the same
    // locations, although it might be a bit slow, due to too much overhead
    // for context switching so many threads
    // @TODO: investigate whether concurrently writing the same value
    // @TODO: corrupts memory
    //
    // NOTE 2: /*inline*/ functions only declare statics once, so it's ok to use them

    // static unsigned short* lut = init_lut<unsigned short>();
    static unsigned char* lut = init_lut<unsigned char>();

    // Type punning is regarded as bad practice, but we don't write to the
    // type-punned x, so there should be no trouble caused by aliasing.
    // For what it's worth, here's how a safe implementation would look
    // like. We don't use it, as it hurts performance on some compilers.
    // http://cellperformance.beyond3d.com/articles/2006/06/understanding-strict-aliasing.html
    //
    // const U64* combined = reinterpret_cast<const U64*>(&x);
    // return lut[combined->u16[0]] + lut[combined->u16[1]] +
    //        lut[combined->u16[2]] + lut[combined->u16[3]];

    // shorts:
    // const unsigned short* u16 = const_cast<const unsigned short*>(&x);
    // return lut[u16[0]] + lut[u16[1]] + lut[u16[2]] + lut[u16[3]];

    // type punning to [unsigned] char is aliasing-safe
    const unsigned char* u8 = reinterpret_cast<const unsigned char*>(&x);
    return lut[u8[0]] + lut[u8[1]] + lut[u8[2]] + lut[u8[3]] +
           lut[u8[4]] + lut[u8[5]] + lut[u8[6]] + lut[u8[7]];
}
#endif // HAMMING_WITH_LUT

#if defined(HAMMING_WITH_VANILLA) || defined(HAMMING_WITH_2x32) || defined(HAMMING_WITH_LUT) || defined(HAMMING_WITH_SPARSE)
INTERNAL_HAMMING_API /*inline*/ unsigned int HAMMING_CALL popcount64_internal(unsigned long long int x)
{
    // selector; should be modified at compile time, depending on the target system
#ifdef HAMMING_WITH_VANILLA
    return popcount64_vanilla(x);
#elif defined(HAMMING_WITH_2x32)
    return popcount64_2x32(x);
#elif defined(HAMMING_WITH_LUT)
    return popcount64_lut(x);
#elif defined(HAMMING_WITH_SPARSE)
    return popcount64_sparse(x);
#endif
}
#endif // defined(HAMMING_WITH_VANILLA) || defined(HAMMING_WITH_2x32) || defined(HAMMING_WITH_LUT) || defined(HAMMING_WITH_SPARSE)

#ifdef HAMMING_WITH_INTRINSICS

#ifdef __INTEL_COMPILER
// intel defines __GNUC__, but has different intrinsic syntax
INTERNAL_HAMMING_API /*inline*/ unsigned char HAMMING_CALL popcount64_icc(unsigned long long int x)
{
    // not tested @TODO: test when we have intel compiler
    // @TODO: check if we need any special checks before calling this (like msvc)
    return _mm_popcnt_u64 (x);
}
#elif defined(__GNUC__)
INTERNAL_HAMMING_API /*inline*/ unsigned int HAMMING_CALL popcount64_gcc(const unsigned long long int& x)
{
    // according to documentation, gcc builtin automatically checks whether
    // hardware support is available, and, if it's not, it has a "fast"
    // implementation for the operation. We decide to trust them for now...
    // Also, the builtin is documented to be available for all platforms
    // (e.g. x86, x64, arm etc.)
    return __builtin_popcountll (x);
}
#elif defined(_MSC_VER) && (defined(_M_AMD64) || defined(_M_IX86))
INTERNAL_HAMMING_API /*inline*/ bool HAMMING_CALL has_popcnt()
{
    // only available on x86, x64 archs
    array<int, 4> cpu_info;
    // __cpuidex(cpu_info.data(), 1, 0);
    __cpuid(cpu_info.data(), 1);
    bitset<32> ecx = cpu_info[2];
    return ecx[23];
}

INTERNAL_HAMMING_API /*inline*/ unsigned char HAMMING_CALL popcount64_msvc(const unsigned long long int& x)
{
    // not tested @TODO: test when we have intel compiler
    // called just once
    static bool has_popcnt_builtin = has_popcnt ();
    if (has_popcnt_builtin):
        return __popcnt64 (x);
    return popcount64_internal (x);
    // @TODO: print a warning if falling back on our own implementation
}
#endif // compilers predefs

#endif // defined(HAMMING_WITH_INTRINSICS)

#if defined(HAMMING_WITH_VANILLA) || defined(HAMMING_WITH_2x32) || defined(HAMMING_WITH_LUT) || defined(HAMMING_WITH_SPARSE) || defined(HAMMING_WITH_INTRINSICS)
INTERNAL_HAMMING_API /*inline*/ unsigned int HAMMING_CALL popcount64(const unsigned long long int& x)
{
    return
#ifdef HAMMING_WITH_INTRINSICS
  #ifdef __INTEL_COMPILER
        popcount64_icc(x);
  #elif defined(__GNUC__)
        popcount64_gcc(x);
  #elif defined(_MSC_VER) && (defined(_M_AMD64) || defined(_M_IX86))
        popcount64_msvc(x);
  #else // unknown compiler - fall back on our own implementation
    #if defined(HAMMING_WITH_VANILLA) || defined(HAMMING_WITH_2x32) || defined(HAMMING_WITH_LUT) || defined(HAMMING_WITH_SPARSE)
        popcount64_internal(x);
    #endif
  #endif // compilers
#else // HAMMING_WITH_INTRINSICS
  #if defined(HAMMING_WITH_VANILLA) || defined(HAMMING_WITH_2x32) || defined(HAMMING_WITH_LUT) || defined(HAMMING_WITH_SPARSE)
        popcount64_internal(x);
  #endif
#endif // HAMMING_WITH_INTRINSICS
}
#endif // defined(HAMMING_WITH_VANILLA) || defined(HAMMING_WITH_2x32) || defined(HAMMING_WITH_LUT) || defined(HAMMING_WITH_SPARSE) || defined(HAMMING_WITH_INTRINSICS)

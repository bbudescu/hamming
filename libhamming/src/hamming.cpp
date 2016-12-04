#include <cstddef>
#include <hamming/hamming_c.h>
#include <hamming/internal/popcount.h>
#include <algorithm>

// we generally prefer using types like unsigned long long instead of
// types uint64_t, as MSVC doesn't natively support those types (stdint.h)

using namespace std;

namespace{
    typedef unsigned int(*popcount_ull_t)(const unsigned long long int&);
    // template by popcount function to give compiler a chance to inline
    template<popcount_ull_t popcount_ull>
    size_t hamming_distance_impl (const unsigned char str1[],
                                  const unsigned char str2[],
                                  const size_t n_bytes)
    {
        if (!n_bytes)
            return 0;

        const unsigned long long* str1_ull = reinterpret_cast<const unsigned long long*>(str1);
        const unsigned long long* str2_ull = reinterpret_cast<const unsigned long long*>(str2);
        const size_t n_longs = n_bytes / sizeof(str1_ull[0]);
        size_t dist = 0;
#pragma omp parallel for reduction(+:dist)
        for(ptrdiff_t ull_idx = 0; ull_idx < n_longs; ++ull_idx)
            dist += popcount_ull(str1_ull[ull_idx] ^ str2_ull[ull_idx]);

        // remaining_bytes < 7
        unsigned char remaining_bytes = static_cast<unsigned char>(n_bytes % sizeof(str1_ull[0]));
        if (!remaining_bytes)
            return dist;

        unsigned long long int remaining_1(0), remaining_2(0);
        copy(str1 + n_bytes - remaining_bytes, str1 + n_bytes, reinterpret_cast<unsigned char*>(&remaining_1));
        copy(str2 + n_bytes - remaining_bytes, str2 + n_bytes, reinterpret_cast<unsigned char*>(&remaining_2));

        dist += popcount_ull(remaining_1 ^ remaining_2);

        return dist;
    }
}

HAMMING_API hamming_status_t HAMMING_CALL hamming_distance(const unsigned char str1[],
                                                           const unsigned char str2[],
                                                           const size_t n_bytes,
                                                           size_t* distance,
                                                           hamming_impl_t impl)
{
    if (!str1)
        return HAMMING_STATUS_BAD_PARAM_STR_1;

    if (!str2)
        return HAMMING_STATUS_BAD_PARAM_STR_2;

    if (!distance)
        return HAMMING_STATUS_BAD_PARAM_DISTANCE;

    switch(impl)
    {
        case HAMMING_IMPL_DEFAULT:
#if defined(HAMMING_WITH_VANILLA) || defined(HAMMING_WITH_2x32) || defined(HAMMING_WITH_LUT) || defined(HAMMING_WITH_SPARSE) || defined(HAMMING_WITH_INTRINSICS)
            *distance = hamming_distance_impl<popcount64>(str1, str2, n_bytes);
            return HAMMING_STATUS_SUCCESS;
#else
            return HAMMING_STATUS_IMPLEMENTATION_NOT_AVAILABLE;
#endif
        case HAMMING_IMPL_VANILLA:
#ifdef HAMMING_WITH_VANILLA
            *distance = hamming_distance_impl<popcount64_vanilla>(str1, str2, n_bytes);
            return HAMMING_STATUS_SUCCESS;
#else
            return HAMMING_STATUS_IMPLEMENTATION_NOT_AVAILABLE;
#endif
        case HAMMING_IMPL_2x32:
#ifdef HAMMING_WITH_2x32
            *distance = hamming_distance_impl<popcount64_vanilla>(str1, str2, n_bytes);
            return HAMMING_STATUS_SUCCESS;
#else
            return HAMMING_STATUS_IMPLEMENTATION_NOT_AVAILABLE;
#endif
        case HAMMING_IMPL_LUT:
#ifdef HAMMING_WITH_LUT
            *distance = hamming_distance_impl<popcount64_lut> (str1, str2, n_bytes);
            return HAMMING_STATUS_SUCCESS;
#else
            return HAMMING_STATUS_IMPLEMENTATION_NOT_AVAILABLE;
#endif
        case HAMMING_IMPL_SPARSE:
#ifdef HAMMING_WITH_SPARSE
            *distance = hamming_distance_impl<popcount64_sparse> (str1, str2, n_bytes);
            return HAMMING_STATUS_SUCCESS;
#else
            return HAMMING_STATUS_IMPLEMENTATION_NOT_AVAILABLE;
#endif
        default:
            return HAMMING_STATUS_UNKNOWN_IMPLEMENTATION;
    }
}

HAMMING_API const char* HAMMING_CALL get_hamming_error_string(hamming_status_t error)
{
    switch(error)
    {
        case HAMMING_STATUS_SUCCESS:
            return "Hamming distance function finished with no incident";
        case HAMMING_STATUS_BAD_PARAM_STR_1:
            return "input parameter <str1> is an invalid pointer";
        case HAMMING_STATUS_BAD_PARAM_STR_2:
            return "input parameter <str2> is an invalid pointer";
        case HAMMING_STATUS_BAD_PARAM_DISTANCE:
            return "output parameter <distance> is an invalid pointer";
        case HAMMING_STATUS_IMPLEMENTATION_NOT_AVAILABLE:
            return "when the hamming library was compiled, the selected "
                    "configuration was disabled";
        case HAMMING_STATUS_UNKNOWN_IMPLEMENTATION:
            return "unknown implementation selected; maybe you have the headers "
                    "of a newer version of libhamming (i.e. mismatch between "
                    "header and lib)";
        default:
            return "unknown error code; maybe you got it from a different "
                    "version of the library (i.e. mismatch between header and "
                    "lib)";
    }
}

HAMMING_CALL int HAMMING_CALL hamming_version()
{
    return HAMMING_VERSION;
}

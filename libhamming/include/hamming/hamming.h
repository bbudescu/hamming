#pragma once

#include <system_error>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>

namespace hamming_c{
#include "hamming_c.h"
}

// high level C++ API

namespace hamming
{
    enum class implementation: int
    {
#if defined(HAMMING_WITH_VANILLA) || defined(HAMMING_WITH_2x32) || defined(HAMMING_WITH_LUT) || defined(HAMMING_WITH_SPARSE) || defined(HAMMING_WITH_INTRINSICS)
        Default_impl = hamming_c::HAMMING_IMPL_DEFAULT,
#endif

#ifdef HAMMING_WITH_VANILLA
        Vanilla = hamming_c::HAMMING_IMPL_VANILLA,
#endif

#ifdef HAMMING_WITH_2x32
        Impl_2x32 = hamming_c::HAMMING_IMPL_2x32,
#endif

#ifdef HAMMING_WITH_LUT
        Lut = hamming_c::HAMMING_IMPL_LUT,
#endif

#ifdef HAMMING_WITH_SPARSE
        Sparse = hamming_c::HAMMING_IMPL_SPARSE
#endif
    };

    class hamming_error_category : public std::error_category
    {
    public:
        virtual const char* name() const noexcept;
        virtual std::string message(int errval) const;
        static const hamming_error_category& instance();

    private:
        static const hamming_error_category instance_;
    };

    size_t distance(const unsigned char str1[], const unsigned char str2[],
                    size_t n_bytes, implementation impl = implementation::Default_impl);

    size_t distance(const std::vector<bool>& v1, const std::vector<bool>& v2);
}

// In every module you want to use this API, make sure to include the hpp ONLY ONCE.
// You have to do this, because throwing exceptions over shared lib borders
// is dangerous (oooh...)
// #include <hamming/hamming.hpp>

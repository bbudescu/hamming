#include <hamming/hamming.h>

const hamming::hamming_error_category hamming::hamming_error_category::instance_;

const char* hamming::hamming_error_category::name() const noexcept
{
    return "libhamming";
}

std::string hamming::hamming_error_category::message(int errval) const
{
    return hamming_c::get_hamming_error_string(static_cast<hamming_c::hamming_status_t>(errval));
}

const hamming::hamming_error_category& hamming::hamming_error_category::instance()
{
    return instance_;
}

size_t hamming::distance(const unsigned char str1[], const unsigned char str2[], size_t n_bytes, implementation impl)
{

    size_t dist = 0;
    hamming_c::hamming_status_t status =
            hamming_c::hamming_distance(str1, str2, n_bytes, &dist, static_cast<hamming_c::hamming_impl_t>(impl));
    if (status != hamming_c::HAMMING_STATUS_SUCCESS)
        throw std::system_error(status, hamming_error_category::instance());

    return dist;
}

size_t hamming::distance(const std::vector<bool>& v1, const std::vector<bool>& v2)
{
    // there is no easy way of using std::vector<bool> to wrap around raw
    // data from files
    // @TODO: try boost::dynamic_bitset for an alternate implementation

    if(v1.size() != v2.size())
        throw std::invalid_argument("input blobs must have the same size");

    std::vector<bool> xored;
    xored.reserve(v1.size());
    std::transform(v1.begin(), v1.end(), v2.begin(), xored.begin(),
                   [](bool bit1, bool bit2){return bit1 ^ bit2;});

    return static_cast<size_t>(std::count(xored.begin(), xored.end(), true));
}
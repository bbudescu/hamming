#include <iostream>

#include <gtest/gtest.h>
#include <random>
#include <algorithm>
#include <functional>
#include <vector>
#include <hamming/hamming.h>
#include <hamming/hamming.hpp>
#include <hamming/internal/popcount.h>

using namespace std;

vector<unsigned char> rand_vect(size_t size)
{
    default_random_engine generator;
    auto distribution = uniform_int_distribution<unsigned char>();
    auto uchar_generator = bind(distribution, generator);
    vector<unsigned char> rval;
    rval.reserve(size);
    generate_n(back_inserter(rval), size, ref(uchar_generator));
    return rval; // RVO => we don't pass it as out param
}

void negate_vect(vector<unsigned char>& char_vec)
{
    transform(char_vec.begin(), char_vec.end(), char_vec.begin(),
              [](unsigned char c){return ~c;});
}

TEST(popcount, simple)
{
    // all of the popcount methods are open source / builtin, so, for now,
    // there is no need for very thorough testing (a production work, however,
    // should include more tests); also, for each implementation, not just the
    // default one
    unsigned long long int xs[4] = {1, 3, 7, 0xFFFFFFFFFFFFFFFFULL};
    unsigned char ones[4] = {1, 2, 3, 64};

    for (unsigned char i = 0; i < 4; ++i)
        EXPECT_EQ(popcount64(xs[i]), ones[i]);

    // random, guaranteed to be obtained by roll of fair die
    unsigned long long int x = 10;
    unsigned int popcount_x = popcount64(x);

    EXPECT_EQ(popcount_x, 2);

    EXPECT_EQ(popcount_x, 64 - popcount64(~x));

    EXPECT_EQ(popcount64(x & 0xFFFFFFFF) + popcount64(x & 0xFFFFFFFF00000000), popcount_x);

    unsigned int alt_x = popcount64_lut(x);

    EXPECT_EQ(alt_x, popcount_x);

    alt_x = popcount64_2x32(x);

    EXPECT_EQ(alt_x, popcount_x);

    // etc.
}

TEST(hamming, low_level_param_check)
{
    // ASSERT_TRUE
    // EXPECT_TRUE
    // EXPECT_EQ
    size_t out = 0;
    EXPECT_EQ(hamming_c::hamming_distance(nullptr, (const unsigned char*)1, 100, &out),
              hamming_c::HAMMING_STATUS_BAD_PARAM_STR_1);

    EXPECT_EQ(hamming_c::hamming_distance((const unsigned char*)1, nullptr, 100, &out),
              hamming_c::HAMMING_STATUS_BAD_PARAM_STR_2);

    EXPECT_EQ(hamming_c::hamming_distance((const unsigned char*)1,
                               (const unsigned char*)1, 100, nullptr),
              hamming_c::HAMMING_STATUS_BAD_PARAM_DISTANCE);
}

TEST(hamming, low_level_simple)
{
    // test whether the function works on strings shorter than 8 bytes
    const unsigned char str1[3] = {0xFF, 0xFF, 0xFF};
    const unsigned char str2[3] = {0x00, 0xF0, 0xFF};

    const size_t expected_dist_3 = 8 + 4 + 0;
    size_t dist_3 = 0;

    hamming_c::hamming_status_t status = hamming_c::hamming_distance(str1, str2, 3, &dist_3);
    EXPECT_EQ(status, hamming_c::HAMMING_STATUS_SUCCESS);
    EXPECT_EQ(dist_3, expected_dist_3);

    auto v1 = rand_vect(100000), v2 = rand_vect(100000);
    size_t orig_dist = 0;
    status = hamming_c::hamming_distance(v1.data(), v2.data(), v1.size(), &orig_dist);
    EXPECT_EQ(status, hamming_c::HAMMING_STATUS_SUCCESS);

    // after inversion, hamming distance should remain the same
    negate_vect(v1);
    negate_vect(v2);

    size_t dist = 0;
    status = hamming_c::hamming_distance(v1.data(), v2.data(), v1.size(), &dist);
    EXPECT_EQ(status, hamming_c::HAMMING_STATUS_SUCCESS);
    EXPECT_EQ(dist, orig_dist);

    // after reverse, hamming distance should remain the same
    reverse(v1.begin(), v1.end());
    reverse(v2.begin(), v2.end());

    dist = 0;
    status = hamming_c::hamming_distance(v1.data(), v2.data(), v1.size(), &dist);
    EXPECT_EQ(status, hamming_c::HAMMING_STATUS_SUCCESS);
    EXPECT_EQ(dist, orig_dist);

    // etc.
}

TEST(hamming, high_level_simple)
{
    auto v1 = rand_vect(100), v2 = rand_vect(100);
    auto dist_orig = hamming::distance(v1.data(), v2.data(), v1.size());

    // after reverse, hamming distance should remain the same
    reverse(v1.begin(), v1.end());
    reverse(v2.begin(), v2.end());

    auto dist = hamming::distance(v1.data(), v2.data(), v1.size());

    EXPECT_EQ(dist, dist_orig);
}

int main(int argc, char* argv[]) {
//    unsigned char* lut = init_lut<unsigned char>();
//
//    for (unsigned int id = 0; id < 256; ++id)
//        cout << int(lut[id]) << ",";
//
// #ifdef EXPORT_INTERNALS
//    cout << "exporting externals";
// #endif

    ::testing::InitGoogleTest(&argc, argv);
//    if(argc < 2)
//    {
//        cout << "usage: " << argv[0] << " " << "<filename>" << endl;
//        return EXIT_FAILURE;
//    }
//    filename = argv[1];
    return RUN_ALL_TESTS();
}

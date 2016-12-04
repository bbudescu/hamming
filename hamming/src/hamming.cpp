#include <cstddef>
#include <hamming/hamming.h>
#include <hamming/hamming.hpp>
#include <iostream>
#include <fstream>
#include <cerrno>
#include <cstring>
#include <vector>

using namespace std;
using namespace hamming;

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        cerr << "Usage:" << argv[0] << " <file1> <file2>" << endl;
        return EXIT_FAILURE;
    }

    ifstream f1(argv[1], ios::binary);
    if (!f1)
    {
        cerr << "Error opening file " << argv[1] << ":" << strerror(errno) << endl;
        return EXIT_FAILURE;
    }

    ifstream f2(argv[2], ios::binary);
    if (!f1)
    {
        cerr << "Error opening file " << argv[2] << ":" << strerror(errno) << endl;
        return EXIT_FAILURE;
    }

    f1.seekg(0, std::ios_base::end);
    std::streamsize n_bytes = f1.tellg();
    f1.seekg(0); // reset stream

    f2.seekg(0, std::ios_base::end);

    if (f2.tellg() != n_bytes)
    {
        cerr << "Files must have the same number of bytes" << endl;
        return EXIT_FAILURE;
    }

    f2.seekg(0); // reset stream

    vector<unsigned char> v1(static_cast<size_t>(n_bytes));
    f1.read(reinterpret_cast<char*>(v1.data()), n_bytes);
    f1.close();

    vector<unsigned char> v2(static_cast<size_t>(n_bytes));
    f2.read(reinterpret_cast<char*>(v2.data()), n_bytes);
    f2.close();

    size_t h_dist = hamming::distance(v1.data(), v2.data(), v1.size());

    cout << "Hamming distance between data in files: " << h_dist << endl;

    return EXIT_SUCCESS;
}
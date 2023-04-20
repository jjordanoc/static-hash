#include <iostream>
#include <fstream>

#define BLOCK_SIZE 10

template<typename RecordType>
const long N = (BLOCK_SIZE - sizeof(long)) / sizeof(RecordType);

#define N N<RecordType>

template<typename RecordType>
struct Bucket {
    RecordType records[N];
    long size = 0;
    long next = -1;

    void insert(RecordType record) {
        for (int i = 0; i < size; ++i) {

        }
    }
};

template<typename RecordType, typename KeyType>
class StaticHashFile {
    std::string filename;
    std::fstream file;
    std::_Ios_Openmode flags = std::ios::in | std::ios::out | std::ios::binary;
    int M = 0;
public:
    StaticHashFile(const std::string &filename, int m) : filename(filename), M(m) {}

    void insert(RecordType record) {
        file.open(filename, flags);
        auto pagePos = std::hash(record.key) % M;
        file.seekp(pagePos * BLOCK_SIZE);
        Bucket<RecordType> bucket{};
        file.read((char *) &bucket, BLOCK_SIZE);
        bucket.insert(record);
    }
};





int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}

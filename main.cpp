#include <iostream>
#include <fstream>

#define BLOCK_SIZE 1024

template<typename RecordType>
const long N = (BLOCK_SIZE - sizeof(long)) / sizeof(RecordType);

#define N N<RecordType>

#define SEEK_ALL(pos) \
file.seekp(pos);      \
file.seekg(pos);

#define SEEK_ALL_RELATIVE(pos, start) \
file.seekp(pos, start);      \
file.seekg(pos, start);

template<typename RecordType>
struct Bucket {
    RecordType records[N];
    long size = 0;
    long next = -1;
};

struct Alumno {
    char key[10];
    char nombre[30];
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
        // search for duplicated key
        auto bucketLogicalPos = std::hash(record.key) % M;
        SEEK_ALL(bucketLogicalPos * BLOCK_SIZE)
        Bucket<RecordType> bucket{};
        file.read((char *) &bucket, BLOCK_SIZE);
        for (long i = 0; i < bucket.size; ++i) {
            if (bucket.records[i].key == record.key) {
                file.close();
                throw std::runtime_error("No se puede insertar una llave duplicada.");
            }
        }
        while (true) {
            if (bucket.next != -1) {
                SEEK_ALL(bucket.next)
                file.read((char *) &bucket, BLOCK_SIZE);
                for (long i = 0; i < bucket.size; ++i) {
                    if (bucket.records[i].key == record.key) {
                        file.close();
                        throw std::runtime_error("No se puede insertar una llave duplicada.");
                    }
                }
            } else {
                break;
            }
        }
        // end of search
        SEEK_ALL(bucketLogicalPos * BLOCK_SIZE)
        file.read((char *) &bucket, BLOCK_SIZE);
        if (bucket.size < N) {
            bucket.records[bucket.size++] = record;
            SEEK_ALL(bucketLogicalPos * BLOCK_SIZE)
            file.write((char *) &bucket, BLOCK_SIZE);
        } else {
            long parentPos = bucketLogicalPos * BLOCK_SIZE;
            while (true) {
                if (bucket.size < N) {
                    bucket.records[bucket.size++] = record;
                    SEEK_ALL(bucketLogicalPos * BLOCK_SIZE)
                    file.write((char *) &bucket, BLOCK_SIZE);
                    break;
                } else {
                    if (bucket.next == -1) {
                        Bucket<RecordType> newBucket{};
                        newBucket.records[0] = record;
                        newBucket.size = 1;
                        newBucket.next = -1;
                        SEEK_ALL_RELATIVE(0, std::ios::end)
                        long newBucketPos = file.tellp();
                        file.write((char *) &newBucket, BLOCK_SIZE);
                        SEEK_ALL(parentPos)
                        bucket.next = newBucketPos;
                        file.write((char *) &bucket, BLOCK_SIZE);
                        break;
                    } else {
                        parentPos = bucket.next;
                        SEEK_ALL(bucket.next)
                        file.read((char *) &bucket, BLOCK_SIZE);
                    }
                }
            }
        }
        file.close();
    }

    RecordType search(KeyType key) {
        file.open(filename, flags);
        auto bucketLogicalPos = std::hash(key) % M;
        SEEK_ALL(bucketLogicalPos * BLOCK_SIZE)
        Bucket<RecordType> bucket{};
        file.read((char *) &bucket, BLOCK_SIZE);
        for (long i = 0; i < bucket.size; ++i) {
            if (bucket.records[i].key == key) {
                file.close();
                return bucket.records[i];
            }
        }
        while (true) {
            if (bucket.next != -1) {
                SEEK_ALL(bucket.next)
                file.read((char *) &bucket, BLOCK_SIZE);
                for (long i = 0; i < bucket.size; ++i) {
                    if (bucket.records[i].key == key) {
                        file.close();
                        return bucket.records[i];
                    }
                }
            } else {
                file.close();
                throw std::runtime_error("No se encontró el registro.");
            }
        }
    }
};


int main() {
    StaticHashFile<Alumno, char[]> staticHashFile{"data.dat", 6};
    Alumno alumno;
    std::cin >> alumno.key;
    std::cin >> alumno.nombre;
    staticHashFile.insert(alumno);
    return 0;
}

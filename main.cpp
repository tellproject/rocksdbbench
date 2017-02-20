#include <rocksdb/db.h>
#include <iostream>
#include <random>
#include <cstdint>
#include <chrono>

void status(rocksdb::Status status) {
    if (!status.ok()) {
        std::cout << status.getState() << std::endl;
        std::terminate();
    }
}

void populate(rocksdb::DB* db, int tuples)
{
    // for a tuple we do not create the correct schema.
    // Instead we make sure that bytes 4-7 contain the correct
    // column B
    char row[72];
    uint64_t key = 0;
    int32_t max = std::numeric_limits<int32_t>::min();
    std::mt19937 dev;
    std::uniform_int_distribution<int32_t> dist;
    std::uniform_int_distribution<char> cdist;
    for (int i = 0; i < 72; ++i) {
        *(row + i) = cdist(dev);
    }
    rocksdb::WriteOptions options;
    for (int i = 0; i < tuples; ++i) {
        int32_t d = dist(dev);
        max = std::max(d, max);
        *reinterpret_cast<int32_t*>(row + 4) = d;
        db->Put(options, rocksdb::Slice(reinterpret_cast<char*>(&key), sizeof(key)), rocksdb::Slice(row, 72));
        ++key;
    }
    std::cout << "Done with Population - max is: " << max << std::endl;
}

using Clock = std::chrono::steady_clock;

int main(int argc, char** argv)
{
    rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = true;
    options.error_if_exists = true;
    status(rocksdb::DB::Open(options, "/tmp/db", &db));
    std::unique_ptr<rocksdb::DB> dbptr(db); // for deletion
    rocksdb::ReadOptions roptions;
    rocksdb::WriteOptions woptions;
    populate(db, 12500000);
    auto begin = Clock::now();
    std::unique_ptr<rocksdb::Iterator> iter(db->NewIterator(roptions));
    int32_t max = std::numeric_limits<int32_t>::min();
    for (iter->SeekToFirst(); iter->Valid(); iter->Next()) {
        int32_t d = *reinterpret_cast<const int32_t*>(iter->value().data() + 4);
        max = std::max(d, max);
    }
    auto end = Clock::now();
    std::cout << "Max is " << max << std::endl;
    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " ms" << std::endl;
}

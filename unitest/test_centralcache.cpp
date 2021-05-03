
#include <assert.h>
#include <string.h>
#include "CentralCache.h"

int main() {
    srand((unsigned int)time(nullptr));
    void* start = nullptr;
    void* end = nullptr;
    size_t byte = 1 + rand()%128;
    size_t nbyte = 1 + rand()%100;
    auto size = CentralCache::GetInstance()->FetchRangeObj(start, end, nbyte, byte);
    memset(start, 0, ClassSize::Index(byte)*size);
    printf("start: %p, end : %p, byte: %ld, nbyte: %ld, size: %ld, fetch byte: %ld\n", start, end, byte, nbyte, size, ClassSize::Index(byte));
    return 0;
}
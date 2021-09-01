#include <unistd.h>
#include <stdlib.h>
#include "ThreadCache.h"


bool test_new_block() {
    ThreadCache thc;
    void* ptr = thc.Allocate(7);
    if (ptr == nullptr) {
        printf("allocate cache ptr is null");
        return false;
    }
    return true;
}

int main() {
    assert(test_new_block());
    return 0;
}
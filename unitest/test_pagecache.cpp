#include <unistd.h>
#include <stdlib.h>
#include "PageCache.h"


bool test_new_span() {
    srand((unsigned int)time(nullptr));
    int n = rand() % 128;
    auto r = PageCache::GetInstance()->NewSpan(n);
    if (r == nullptr) {
        std::cout << "NewSpan failed" << std::endl;
        return false;
    }

    int intn = getpagesize()*n / sizeof(int);
    unsigned int * indexr = (unsigned int*)r;
    int i = 0;
    for (i = 0; i < intn; i++) {
        indexr[i] = i;
        printf("r address: %p, num: %d, index: %d, npage: %d\n", ((unsigned int*)r+i), (*((unsigned int*)r+i)), i, n);
    }

    if (n * 1024 != (i + 1)) {
        return false;
    }
    return true;
}

int main() {
    assert(!test_new_span());

    return 0;
}
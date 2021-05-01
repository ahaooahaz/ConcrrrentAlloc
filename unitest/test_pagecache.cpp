#include <unistd.h>
#include <stdlib.h>
#include "PageCache.h"


bool test_new_span() {
    srand((unsigned int)time(nullptr));
    int n = 1 + rand() % 127;
    Span* span = PageCache::GetInstance()->NewSpan(n);
    if (span == nullptr) {
        std::cout << "NewSpan failed" << std::endl;
        return false;
    }

    unsigned int intn = getpagesize()*n / sizeof(int);
    unsigned int * indexr = (unsigned int*)span->_objlist;
    int i = 0;
    for (i = 0; i < intn; i++) {
        indexr[i] = i;
        printf("r address: %p, num: %d, index: %d, npage: %d, intn: %d\n", ((unsigned int*)indexr+i), (*((unsigned int*)indexr+i)), i, n, intn);
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
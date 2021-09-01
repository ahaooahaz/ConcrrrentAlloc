#include <vector>
#include <ConcurrentAlloc.hpp>
using namespace std;

int main() {
    vector<int, ConcurrentAlloc<int>> vt;
    for (int x = 0; x < 100; x++) {
        vt.push_back(1);
        printf("insert index [%d] value [%d]\n", x, vt[x]);
    }
    printf("vt size: %ld\n", vt.size());
    vt.clear();
    return 0;
}
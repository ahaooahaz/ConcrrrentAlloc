#include <vector>
#include <ConcurrentAlloc.hpp>
using namespace std;

int main() {
    vector<int,ConcurrentAlloc<int>> vt;
    vt.push_back(1);
    vt.clear();
    return 0;
}
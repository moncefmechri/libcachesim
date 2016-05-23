#include <iostream>
#include "LRUCache.h"

using namespace std;

int main(void)
{
    LRUCache cache(32768, 64, 64);
    cout << cache.access(0xdeadbeef) << endl;
    cout << cache.access(0xdeadbeef) << endl;

    cout << cache.access(0xbaba) << endl;
    cout << cache.access(0xbaba) << endl;
}

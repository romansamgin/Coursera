#include "test_runner.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <queue>
#include <stdexcept>
#include <set>
using namespace std;


template <class T>
class ObjectPool {
public:
    T* Allocate() {
        if (!free.empty()) {
            T* result = free.front();
            busy.insert(free.front());
            free.pop();
            auto it = busy.find(result);
            return *it;
        }
        else {
            T* object = new T;
            busy.insert(object);
            auto it = busy.find(object);
            return *it;
        }
    }

    T* TryAllocate() {
        if (!free.empty()) {
            T* result = free.front();
            busy.insert(free.front());
            free.pop();
            auto it = busy.find(result);
            return *it;
        }
        else {
            return nullptr;
        }

    }

    void Deallocate(T* object) {
        if (!busy.count(object)) {
            throw invalid_argument("");
        }
        else {
            free.push(object);
            busy.erase(object);
        }
    }


    ~ObjectPool() {
        for (T* object : busy) {
            delete object;
        }

        while (!free.empty()) {
            delete free.front();
            free.pop();
        }
    }

private:
    set<T*> busy{};
    queue<T*> free{};
};

void TestObjectPool() {
    ObjectPool<string> pool;

    auto p1 = pool.Allocate();
    auto p2 = pool.Allocate();
    auto p3 = pool.Allocate();

    *p1 = "first";
    *p2 = "second";
    *p3 = "third";

    pool.Deallocate(p2);
    ASSERT_EQUAL(*pool.Allocate(), "second");

    pool.Deallocate(p3);
    pool.Deallocate(p1);
    ASSERT_EQUAL(*pool.Allocate(), "third");
    ASSERT_EQUAL(*pool.Allocate(), "first");

    pool.Deallocate(p1);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestObjectPool);
    return 0;
}

    



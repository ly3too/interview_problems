#include "heap/heap.hpp"
#include "gtest/gtest.h"
#include "utils.hpp"
#include <vector>

using namespace std;

TEST(test, make_heap) {
    vector<int> heap = {5, 3, 2, 1, 6};
    cout << "original: " << heap << endl;

    make_heap<int>(heap);
    cout << "min heap: " << heap << endl;
    ASSERT_EQ(heap[0], 1);

    make_heap<int>(heap, greater<>());
    cout << "max heap: " << heap << endl;
    ASSERT_EQ(heap[0], 6);
}

TEST(test, heap) {
    vector<int> ori = {5, 3, 2, 1, 6, 0};
    vector<int> tmp = ori;
    Heap<int> heap(move(tmp));
    ASSERT_TRUE(tmp.empty());
    ASSERT_FALSE(heap.empty());

    // check heap sort
    sort(ori.begin(), ori.end());
    vector<int> heap_sorted;
    while(!heap.empty()) {
        heap_sorted.emplace_back(heap.top());
        heap.pop();
    }
    ASSERT_EQ(ori, heap_sorted);

    // check max heap sort
    Heap<int, vector<int>, greater<>> max_heap{};
    for (auto &item: ori) {
        max_heap.emplace(item);
    }
    sort(ori.begin(), ori.end(), greater<>());
    heap_sorted.clear();
    while(!max_heap.empty()) {
        heap_sorted.emplace_back(max_heap.top());
        max_heap.pop();
    }
    ASSERT_EQ(heap_sorted, ori);
}

TEST(test, relax) {
    RelaxHeap<int> heap({5, 3, 2, 1, 6});
    ASSERT_EQ(heap.top(), 1);
    heap.relaxKey(5, 0);
    ASSERT_EQ(heap.top(), 0);

    vector<int> sorted;
    while (heap.size()) {
        sorted.emplace_back(heap.top());
        heap.pop();
    }
    cout << "after sorted: " << sorted << endl;
    ASSERT_EQ(vector<int>({0, 1, 2, 3, 6}), sorted);
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
#include "gtest/gtest.h"
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;

template<typename Iter, typename Cmp>
Iter quickPartition(Iter begin, Iter end, Iter piv, const Cmp &cmp) {
    --end;
    std::swap(*piv, *begin);
    auto savedBegin = begin;
    auto pivotal = *begin;
    ++begin;
    while (true) {
        // find first val < pivotal
        while (!(end < begin) && !cmp(*end, pivotal)) {
            --end;
        }
        while (!(end < begin) && cmp(*begin, pivotal)) {
            ++begin;
        }
        if (!(begin < end)) {
            break;
        }
        swap(*begin, *end);
    }
    // end is the first val < piv
    swap(*savedBegin, *end);
    return end;
}

template<typename Iter, typename Cmp>
void quickSortInternal(Iter begin, Iter end, const Cmp &cmp) {
    if (begin < end) {
        auto piv = quickPartition(begin, end, begin, cmp);
        quickSortInternal(begin, piv, cmp);
        quickSortInternal(piv + 1, end, cmp);
    }
}

template <typename Iter, typename Cmp>
Iter partition2(Iter begin, Iter end, Iter pivotal, const Cmp &cmp) {
    if (!(begin < end)) {
        return pivotal;
    }
    auto piv = *pivotal;
    auto savedBeg = begin;
    std::swap(*begin, *pivotal);
    ++begin;
    --end;
    while (true) {
        // end >= begin && *end >= piv
        while (!(end < begin) && !cmp(*end, piv)) {
            --end;
        }
        // end >= begin && *begin < piv
        while (!(end < begin) && cmp(*begin, piv)) {
            ++begin;
        }
        if (!(begin < end)) {
            break;
        }
        swap(*begin, *end);
    }
    swap(*savedBeg, *end);
    return end;
}

template<typename Iter, typename Cmp>
Iter partition3(Iter begin, Iter end, Iter piv, const Cmp& cmp) {
    swap(*begin, *piv);
    auto val = *begin;
    auto start = begin;
    ++start;
    while (start < end) {
        if (cmp(*start, val)) {
            ++start;
        } else {
            swap(*start, *(--end));
        }
    }
    --end;
    swap(*begin, *end);
    return end;
}

template<typename Iter, typename Cmp>
void quickSort(Iter begin, Iter end, const Cmp &cmp) {
    auto seed = chrono::system_clock::now().time_since_epoch().count();
    shuffle(begin, end, default_random_engine(seed));
    quickSortInternal(begin, end, cmp);
}

TEST(test, partition) {
    {
        vector<int> vals = {2, 3, 5, 3, 1, 4, 2, 23,};
        auto piv = partition2(vals.begin(), vals.end(), vals.begin() + 1, std::less<>());
        ASSERT_EQ(*piv, 3);
        for (auto it = vals.begin(); it != piv; ++it) {
            ASSERT_TRUE(*it < *piv);
        }
        for (auto it = piv + 1; it != vals.end(); ++it) {
            ASSERT_TRUE(*it >= *piv);
        }
    }

    {
        vector<int> vals = {2, 3, 5, 3, 1, 4, 2, 23,};
        auto piv = partition3(vals.begin(), vals.end(), vals.begin() + 1, std::less<>());
        ASSERT_EQ(*piv, 3);
        for (auto it = vals.begin(); it != piv; ++it) {
            ASSERT_TRUE(*it < *piv);
        }
        for (auto it = piv + 1; it != vals.end(); ++it) {
            ASSERT_TRUE(*it >= *piv);
        }
    }
}

TEST(test, qsort) {
    vector<int> vals = {2, 3, 5, 1, 4, 2, 23,};
    // sort(vals.begin(), vals.end(), [](int a, int b){});
    quickSort(vals.begin(), vals.end(), std::less<>());
    ASSERT_EQ(vals, vector<int>({1, 2, 2, 3, 4, 5,23}));
    vector<int> vals2 = {1};
    quickSort(vals2.begin(), vals2.end(), std::less<>());
    ASSERT_EQ(vals2, vector<int>({1}));
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}

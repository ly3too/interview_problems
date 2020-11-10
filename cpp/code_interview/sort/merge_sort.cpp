#include "gtest/gtest.h"
#include <vector>

using namespace std;

template <typename ToIter, typename SrcIter, typename Cmp>
void mergeSortInteral(ToIter dstBegin, ToIter dstEnd, SrcIter srcBegin,
                      SrcIter srcEnd, const Cmp &cmp) {
    auto leftLen = (srcEnd - srcBegin) / 2;
    if (leftLen == 0) {
        return;
    }
    auto srcMid = srcBegin + leftLen;
    auto dstMid = dstBegin + leftLen;
    // sort left half and right half to src
    mergeSortInteral(srcBegin, srcMid, dstBegin, dstMid, cmp);
    mergeSortInteral(srcMid, srcEnd, dstMid, dstEnd, cmp);
    // merge src to dst
    auto srcRight = srcMid;
    while (srcBegin < srcMid && srcRight < srcEnd) {
        if (cmp(*srcBegin, *srcRight)) {
            *dstBegin = *srcBegin;
            ++srcBegin;
        } else {
            *dstBegin = *srcRight;
            ++srcRight;
        }

        ++dstBegin;
    }
    while (srcBegin < srcMid) {
        *(dstBegin++) = *(srcBegin++);
    }
    while (srcRight < srcEnd) {
        *(dstBegin++) = *(srcRight++);
    }
}

template <typename Iter, typename Cmp>
void mergeSort(Iter begin, Iter end, const Cmp &cmp) {
    using ValType = typename Iter::value_type;
    // copy tmp vals
    auto tmp = vector<ValType>(begin, end);
    mergeSortInteral(begin, end, tmp.begin(), tmp.end(), cmp);
}

TEST(test, merge_sort) {
    vector<int> vals = {9, 5, 3, 2, 1, 1, 2, 3, 4, 6};
    mergeSort(vals.begin(), vals.end(), less<>());
    ASSERT_EQ(vals, vector<int>({1, 1, 2, 2, 3, 3, 4, 5, 6, 9}));
    mergeSort(vals.begin(), vals.end(), greater<>());
    ASSERT_EQ(vals, vector<int>({9, 6, 5, 4, 3, 3, 2, 2, 1, 1}));
}
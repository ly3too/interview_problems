#include "set/disjoint_set.hpp"
#include "gtest/gtest.h"

using namespace std;

TEST(test, disjoin_set) {
    auto dset = DisjointSetHelper<int>();
    vector<DisjointSetHelper<int>::Node*> nodes;
    nodes.reserve(100);
    for (auto i = 0; i < 50; ++i) {
        nodes.emplace_back(dset.makeSet(i));
        ASSERT_EQ(dset.find(nodes[i]), nodes[i]);
    }

    for (auto i = 0; i < 49; ++i) {
        dset.unionSet(nodes[i], nodes[i + 1]);
        ASSERT_EQ(dset.find(nodes[i + 1]), nodes[0]);
    }
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
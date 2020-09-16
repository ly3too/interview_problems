#include "list/list.hpp"
#include "gtest/gtest.h"
#include <forward_list>
#include <list>

using namespace std;

TEST(test, forward_list) {
    auto list = ForwardList<int>();
    list.emplace_front(4);
    list.emplace_front(3);
    list.emplace_front(2);
    list.emplace_front(1);
    ASSERT_EQ(list.size(), 4);

    vector<int> tmp;
    for (auto &item: list) {
        tmp.emplace_back(item);
    }
    ASSERT_EQ(tmp, vector<int>({1, 2, 3, 4}));

    list.erase_after(nullptr);
    ASSERT_EQ(list.size(), 3);
    tmp.clear();
    for (auto &item: list) {
        tmp.emplace_back(item);
    }
    ASSERT_EQ(tmp, vector<int>({2, 3, 4}));

    list.erase_after(++list.begin());
    ASSERT_EQ(list.size(), 2);
    ASSERT_EQ(*list.begin(), 2);
    ASSERT_EQ(*(++list.begin()), 3);

    list.remove(2);
    ASSERT_EQ(list.size(), 1);
    ASSERT_EQ(*list.begin(), 3);
    list.clear();
    ASSERT_TRUE(list.empty());
}

template <typename T>
bool operator==(const List<T> &list1, const list<T> &list2) {
    if (list1.size() != list2.size()) {
        return false;
    }
    auto it2 = list2.begin();
    for (auto it = list1.begin(); it != list1.end(); ++it, ++it2) {
        if (*it != *it2) {
            return false;
        }
    }
    return true;
}

TEST(test, List) {
    list<int> std_list = {1, 2, 3, 4};
    List<int> my_list = {1, 2, 3, 4};
    ASSERT_EQ(std_list.size(), my_list.size());

    int val1 = 3;
    std_list.emplace_back(val1);
    my_list.emplace_back(val1);
    ASSERT_EQ(std_list.size(), my_list.size());
    ASSERT_EQ(my_list, std_list);

    std_list.erase(++std_list.begin());
    my_list.erase(++my_list.begin());
    ASSERT_EQ(my_list, std_list);
    std_list.erase(std_list.begin());
    my_list.erase(my_list.begin());
    ASSERT_EQ(my_list, std_list);

    std_list.emplace_front(0);
    my_list.emplace_front(0);
    ASSERT_EQ(my_list, std_list);

    std_list.emplace(std_list.begin(), 2);
    my_list.empalce(my_list.begin(), 2);
    ASSERT_EQ(my_list, std_list);
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}

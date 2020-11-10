//
// Created by ly3too on 2020/11/8.
//

#include "gtest/gtest.h"
#include <utility>
#include <vector>

using namespace std;

template<typename Val>
class List {
protected:
    template<typename T>
    struct BaseNode {
        T *next = nullptr;
    };

    struct Node: public BaseNode<Node> {
        Val val;

        template<typename ...Args>
        explicit Node(Args&& ...args): val(std::forward<Args>(args)...) {}
    };

    BaseNode<Node> head{};

public:
    template<typename ...Args>
    void add(Args&& ...args) {
        auto next = new Node(std::forward<Args>(args)...);
        next->next = head.next;
        head.next = next;
    }

    bool remove(const Val &val) {
        auto prev = &head;
        for (auto it = head.next; it != nullptr; it = it -> next) {
            if (it->val == val) {
                prev->next = it->next;
                delete it;
                return true;
            }
            prev = it;
        }
        return false;
    }

    void reverse() {
        Node* tail = nullptr;
        Node* it = head.next;
        while (it) {
            auto cur = it;
            it = it->next;
            cur->next = tail;
            tail = cur;
        }
        head.next = tail;
    }

    vector<Val> toVector() {
        vector<Val> ret;
        for (auto it = head.next; it != nullptr; it = it->next) {
            ret.emplace_back(it->val);
        }
        return ret;
    }

    ~List() {
        auto cur = head.next;
        while (cur) {
            auto saved = cur;
            cur = cur->next;
            delete saved;
        }
    }
};

TEST(test, list) {
    List<int> list;
    list.add(1);
    list.add(2);
    list.add(3);
    ASSERT_EQ(list.toVector(), vector<int>({3, 2, 1}));
    list.remove(2);
    ASSERT_EQ(list.toVector(), vector<int>({3, 1}));
    list.add(4);
    list.reverse();
    ASSERT_EQ(list.toVector(), vector<int>({1, 3, 4}));
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
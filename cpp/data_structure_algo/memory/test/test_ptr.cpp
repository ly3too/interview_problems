#include "memory/smart_ptr.hpp"
#include "gtest/gtest.h"
#include <memory>
#include <functional>

using namespace std;

class DelCall {
public:
    DelCall(function<void()> func): m_cb(move(func)) {}

    ~DelCall() {
        m_cb();
    }

    function<void()> m_cb;
    int m_int = 10;
};

TEST(test, ptr) {
    auto cnt1 = 0;
    {
        auto fun = [&cnt1](){++cnt1;};
        auto ptr1 = SharedPtr<DelCall>::makeShared(fun);
        auto ptr2 = ptr1;
        auto ptr3 = ptr2;
        ASSERT_EQ(ptr1.get(), ptr3.get());
        ASSERT_EQ(ptr1.getCount(), 3);
        ASSERT_EQ(ptr1->m_int, 10);
    }
    ASSERT_EQ(cnt1, 1);

    cnt1 = 0;
    int cnt2 = 0;
    {
        auto ptr = SharedPtr<int>(new int(100), [&cnt1](const int *ptr){ ++cnt1; delete ptr;});
        SharedPtr<int> ptr2(move(ptr));
        ASSERT_EQ(ptr2.getCount(), 1);
        auto ptr3 = ptr2;
        ASSERT_EQ(*ptr2, 100);
        *ptr3 = 20;
        ASSERT_EQ(*ptr2, 20);

        auto ptr4 = SharedPtr<int>(new int(11), [&cnt2](const int *ptr){++cnt2; delete ptr;});
        ptr4.reset(new int(12));
        ASSERT_EQ(*ptr4, 12);
        ASSERT_EQ(cnt2, 1);
    }
    ASSERT_EQ(cnt1, 1);


    cout << "sizeof(SharedPtr): " << sizeof(SharedPtr<int>) << endl;
    cout << "sizeof(shared_ptr) " << sizeof(shared_ptr<int>) << endl;
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
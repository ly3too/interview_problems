#include "gtest/gtest.h"
#include <functional>
#include <string>

using namespace std;

template <typename ValueType>
class SmartPointer {
public:
    using Pointer = ValueType *;
    using ConstPointer = const ValueType *;

protected:
    struct Holder {
        Pointer pt = nullptr;
        std::function<void (ConstPointer pt)> deleteter = defaultDeletor;
        int refCnt = 1;

        Holder() = default;

        template<typename Del>
        Holder(Pointer pt1, Del&& del): pt(pt1), deleteter(std::forward<Del>(del)) {}

        ~Holder() {
            deleteter(pt);
            pt = nullptr;
        }

        static void defaultDeletor(ConstPointer pt) {
                delete pt;
        }
    };

    Holder * m_holder = nullptr;
public:

    SmartPointer() = default;

    template<typename Del = std::function<void (ConstPointer)>>
    explicit SmartPointer(Pointer pt, Del&& del = Holder::defaultDeletor):
    m_holder(new Holder(pt, std::forward<Del>(del))) {
    }

    SmartPointer(const SmartPointer& other): m_holder(other.m_holder) {
        ++m_holder->refCnt;
    }

    SmartPointer(SmartPointer &&other)  noexcept = default;

    SmartPointer &operator=(const SmartPointer& other) {
        if (this == &other) {
            return *this;
        }
        this->~SmartPointer();
        new (this) SmartPointer(other);
        return *this;
    }

    SmartPointer &operator=(SmartPointer&& other)  noexcept {
        std::swap(m_holder, other.m_holder);
        return *this;
    }

    int getRefCount() const {
        return m_holder == nullptr ? 1 : m_holder->refCnt;
    }

    ValueType &operator*() const {
        return *m_holder->pt;
    }

    Pointer operator->() const {
        return m_holder->pt;
    }

    explicit operator bool() const {
        return m_holder && m_holder->pt;
    }

    ~SmartPointer() {
        if (!m_holder) {
            return;
        }
        --m_holder->refCnt;
        if (m_holder->refCnt == 0) {
            delete m_holder;
        }
        m_holder = nullptr;
    }
};

TEST(test, smart_ptr) {
    auto ptr = SmartPointer<string>();
    ASSERT_FALSE(ptr);
    auto ptr2 = SmartPointer<string>(new string("hello"));
    auto ptr3 = ptr2;
    ASSERT_EQ(ptr2.getRefCount(), 2);
    ASSERT_EQ(*ptr3, "hello");
    bool del_called = false;
    {
        auto ptr4 = SmartPointer<int>(new int(11), [&del_called](const int *ptr) {delete ptr; del_called = true;});
        ASSERT_FALSE(del_called);
        auto ptr5 = ptr4;
        ASSERT_FALSE(del_called);
    }
    ASSERT_TRUE(del_called);
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}

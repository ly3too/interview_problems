#pragma once
#include <functional>

template <typename T>
class SharedCounter {
public:
    using element_type = T;
    using count_type = std::size_t;
    using deleter_type = std::function<void(element_type *)>;

protected:

    element_type *m_ele = nullptr;
    count_type m_counter = 1;
    deleter_type m_del = defaultDelete;
public:
    static void defaultDelete(element_type *ptr) {
        if (!ptr) {
            return;
        }
        if (std::is_array<element_type>::value) {
            delete []ptr;
        } else {
            delete ptr;
        }
    }

    SharedCounter() = default;

    ~SharedCounter() {
        m_del(m_ele);
    }

    bool operator==(count_type cnt) {
        return m_counter == cnt;
    }

    explicit SharedCounter(element_type *ele, deleter_type del = defaultDelete):
    m_ele(ele), m_del(std::move(del)) {}

    element_type *get() {
        return m_ele;
    }

    SharedCounter &operator--() {
        --m_counter;
        return *this;
    }

    SharedCounter &operator++() {
        ++m_counter;
        return *this;
    }

    count_type getCount() {
        return m_counter;
    }
};

template<typename T>
class SharedPtr {
public:
    using element_type = T;
    using count_type = typename SharedCounter<T>::count_type;

protected:
    using counter_type = SharedCounter<T>;

    element_type *m_ele = nullptr;
    counter_type *m_counter = nullptr;

public:
    SharedPtr() = default;

    SharedPtr(element_type *ptr): m_ele(ptr), m_counter(new counter_type(ptr)) {}

    template<typename Tp, typename Del>
    SharedPtr(Tp&& ptr, Del del): m_ele(std::forward<Tp>(ptr)),
    m_counter(new counter_type(ptr, std::move(del))) {
    }

    SharedPtr(const SharedPtr &other): m_ele(other.m_ele), m_counter(other.m_counter) {
        if (m_counter) {
            ++(*m_counter);
        }
    }

    SharedPtr &operator=(const SharedPtr &other) {
        if (this == &other) {
            return *this;
        }

        this->~SharedPtr();
        new (this) SharedPtr(other);
        // another way:
        // swap(*this, SharedPtr(other));

        return *this;
    }

    SharedPtr(SharedPtr &&other) noexcept : SharedPtr() {
        std::swap(this->m_counter, other.m_counter);
        std::swap(this->m_ele, other.m_ele);
    }

    SharedPtr &operator=(SharedPtr &&other)  noexcept {
        std::swap(this->m_counter, other.m_counter);
        std::swap(this->m_ele, other.m_ele);
        return *this;
    }

    ~SharedPtr() {
        if (m_counter && --(*m_counter) == 0) {
            delete m_counter;
            m_counter = nullptr;
        }
    }

    element_type *get() {
        return m_ele;
    }

    template<typename Del = typename counter_type::deleter_type>
    void reset(element_type *ptr, Del del = counter_type::defaultDelete) {
        SharedPtr<T> tmp(ptr, std::move(del));
        std::swap(*this, tmp);
    }

    element_type &operator*() {
        return *m_ele;
    }

    element_type *operator->() {
        return m_ele;
    }

    template<typename ...Args>
    static SharedPtr<T> makeShared(Args&&... args) {
        return SharedPtr<T>(new T(std::forward<Args>(args)...));
    }

    count_type getCount() const {
        if (m_counter) {
            return m_counter->getCount();
        }
        return 1;
    }
};
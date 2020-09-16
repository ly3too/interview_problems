#pragma once
#include <functional>

template<typename T>
class SharedPtr {
public:
    using element_type = T;

protected:
    using count_type = std::size_t;
    using deleter_type = std::function<void(element_type *)>;

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

    element_type *m_ele = nullptr;
    count_type *const m_counter = nullptr;
    deleter_type m_del = defaultDelete;
public:
    SharedPtr() = default;

    template<typename Tp, typename Del>
    SharedPtr(Tp&& ptr, Del del): m_ele(std::forward<Tp>(ptr)), m_counter(new count_type(1)),
        m_del(std::move(del)) {
    }

    SharedPtr(const SharedPtr &other): m_ele(other.m_ele),
        m_counter(other.m_counter), m_del(other.m_del) {
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
        // swap(*this, SharedPtr(other));

        return *this;
    }

    SharedPtr(SharedPtr &&other) noexcept : SharedPtr() {
        std::swap(*this, other);
    }

    SharedPtr &operator=(SharedPtr &&other)  noexcept {
        std::swap(*this, other);
        return *this;
    }

    ~SharedPtr() {
        if (!m_counter) {
            return;
        }
        --(*m_counter);
        if (*m_counter == 0) {
            delete m_counter;
            m_del(m_ele);
        }
    }

    element_type *get() {
        return m_ele;
    }

    void reset(element_type *ptr) {
        std::swap(*this, SharedPtr(ptr, m_del));
    }
};
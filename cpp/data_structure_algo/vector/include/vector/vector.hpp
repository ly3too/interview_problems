#pragma once
#include <utility>
#include <memory>
#include <algorithm>

template <typename T, typename Alloc = std::allocator<T>>
class Vector {
public:
    using value_type = T;
    using size_type = std::size_t;
    using allocator_type = Alloc;
    using pointer = T*;
    using difference_type = std::ptrdiff_t;

protected:
    value_type *m_begin = nullptr;
    value_type *m_end = nullptr;
    value_type *m_limit = nullptr;
    allocator_type m_alloc = allocator_type();
    constexpr static std::size_t DEF_MIN_CAP = 8;
    constexpr static float FACTOR = 2.0;

public:
    Vector() = default;

    explicit Vector(size_type n, const value_type &val = value_type(),
           const allocator_type& alloc = allocator_type()) {
        m_alloc = alloc;
        while(n--) {
            emplace_back(val);
        }
    }

    Vector(const Vector &other) {
        for (auto &item: other) {
            emplace_back(item);
        }
    }

    Vector &operator=(const Vector &other) {
        if (this == &other) {
            return *this;
        }
        clear();
        for (auto &item: other) {
            emplace_back(item);
        }
    }

    ~Vector() {
        clear();
    }

    class iterator {
    public:
        iterator(pointer cur): m_cur(cur) {}

        iterator &operator++() {
            ++m_cur;
            return *this;
        }

        iterator operator++(int) {
            return iterator(m_cur++);
        }

        difference_type operator-(const iterator &other) const {
            return m_cur - other.m_cur;
        }

        iterator operator+(difference_type diff) {
            return iterator(m_cur + diff);
        }

        bool operator==(const iterator &other) const {
            return m_cur == other.m_cur;
        }

        bool operator!=(const iterator &other) const {
            return m_cur != other.m_cur;
        }

        value_type &operator*() {
            return *m_cur;
        }

        value_type *operator->() {
            return m_cur;
        }

    protected:
        value_type *m_cur;
    };

    void reserve(size_type len) {
        if (len <= size() || len == capacity()) {
            return;
        }

        // get new space
        auto new_begin = m_alloc.allocate(len);
        pointer new_end = nullptr;
        try {
            // copy to new place
            new_end = std::uninitialized_copy(m_begin, m_end, new_begin);

            // delete old ones
            for (auto it = m_begin; it != m_end; ++it) {
                m_alloc.destroy(it);
            }
            m_alloc.deallocate(m_begin, m_limit - m_begin);

            // assign new pointers
            m_begin = new_begin;
            m_end = new_end;
            m_limit = new_begin + len;
        } catch (...) {
            if (new_end) {
                for (auto it = new_begin; it != new_end; ++it) {
                    m_alloc.destroy(it);
                }
            }
            m_alloc.deallocate(new_begin, len);
            throw;
        }
    }

    template<typename ...Args>
    void emplace_back(Args&&... args) {
        if (m_end == m_limit) {
            auto len = std::max(size_type(size() * FACTOR), DEF_MIN_CAP);
            reserve(len);
        }
        m_alloc.construct(m_end, std::forward<Args>(args)...);
        ++m_end;
    }

    void pop_back() {
        if (m_end <= m_begin) {
            throw std::out_of_range("pop_back");
        }
        m_alloc.destroy(m_end - 1);
        --m_end;
        if (size() < size_type(capacity() / FACTOR / 2) && capacity() > DEF_MIN_CAP){
            auto len = std::max(size_type(capacity() / FACTOR), DEF_MIN_CAP);
            reserve(len);
        }
    }

    size_type capacity() const {
        return m_limit - m_begin;
    }

    size_type size() const {
        return size_type(m_end - m_begin);
    }

    bool empty() const {
        return size() == 0;
    }

    void clear() {
        for (auto it = m_begin; it != m_end; ++it) {
            m_alloc.destroy(it);
        }
        m_alloc.deallocate(m_begin, m_limit - m_begin);
        m_begin = m_limit = m_end = nullptr;
    }

    iterator begin() const {
        return iterator(const_cast<pointer>(m_begin));
    }

    iterator end() const {
        return iterator(const_cast<pointer>(m_end));
    }

    value_type &back() {
        return *(m_end - 1);
    }
};

template<typename T, typename Alloc>
constexpr std::size_t Vector<T, Alloc>::DEF_MIN_CAP;
template<typename T, typename Alloc>
constexpr float Vector<T, Alloc>::FACTOR;
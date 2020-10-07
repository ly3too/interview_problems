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
           const allocator_type& alloc = allocator_type()): m_alloc(alloc) {
        reserve(n);
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
        return *this;
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

        value_type &operator*() const {
            return *m_cur;
        }

        value_type *operator->() const {
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
        if (size() <= size_type(capacity() / FACTOR / 2) && capacity() > DEF_MIN_CAP){
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

    const value_type &operator[](size_type idx) const {
        return *(m_begin + idx);
    }

    value_type &operator[](size_type idx) {
        return *(m_begin + idx);
    }
};

template<typename T, typename Alloc>
constexpr std::size_t Vector<T, Alloc>::DEF_MIN_CAP;
template<typename T, typename Alloc>
constexpr float Vector<T, Alloc>::FACTOR;

/**
 * a vector which can automatically grow in front and back
 * @tparam T value type
 * @tparam Alloc allocator
 * @tparam AUTO_FRONT_RESIZE enable front auto resize
 * @tparam AUTO_BACK_RESIZE enable back auto resize
 * @tparam MIN_CAP the minimum cap to grow must be positive
 */
template<typename T, typename Alloc = std::allocator<T>,
        bool AUTO_FRONT_RESIZE = true, bool AUTO_BACK_RESIZE = true,
        std::size_t MIN_CAP = 8>
class DqVector {
public:
    using value_type = T;
    using reference = value_type &;
    using pointer = value_type *;
    using size_type = std::size_t;
    using allocator_type = Alloc;
    using iterator = pointer;
    using const_iterator = const value_type *;

protected:
    pointer m_begin = nullptr; // begin of data
    pointer m_end = nullptr; // end of data
    pointer m_start = nullptr; // start of buffer
    pointer m_limit = nullptr; // end of buffer
    allocator_type m_alloc = allocator_type();

    /**
     * allocate new space and copy elements to new space
     * @param total
     * @param begin
     */
    void reallocate_and_relocate(size_type total, size_type begin) {
        auto new_start = m_alloc.allocate(total);
        auto new_limit = new_start + total;
        auto new_begin = new_start + begin;
        pointer new_end = nullptr;

        try {
            // copy elements
            new_end = std::uninitialized_copy(m_begin, m_end, new_begin);

            // destroy old ones
            for (auto it = m_begin; it != m_end; ++it) {
                m_alloc.destroy(it);
            }
            m_alloc.deallocate(m_start, m_limit - m_start);

            // assign pointers
            m_start = new_start;
            m_limit = new_limit;
            m_begin = new_begin;
            m_end = new_end;
        } catch (...) {
            // destroy new ones
            if (new_end) {
                for (auto it = new_begin; it != new_end; ++it) {
                    m_alloc.destroy(it);
                }
            }
            // deallocate new buffer
            m_alloc.deallocate(new_start, total);
            // rethrow
            throw;
        }
    }

public:
    DqVector(allocator_type alloc = allocator_type()): m_alloc(std::move(alloc)) {};
    DqVector(size_type size, const value_type &val = value_type(),
             allocator_type alloc = allocator_type()): m_alloc(std::move(alloc)) {
        spare_back(size);
        while (size--) {
            emplace_back(val);
        }
    }

    ~DqVector() {
        clear();
    }

    void clear() {
        for (auto it = m_begin; it != m_end; ++it) {
            m_alloc.destroy(it);
        }
        m_alloc.deallocate(m_start, capacity());
        m_begin = m_start = m_end = m_limit = nullptr;
    }

    size_type size() const {
        return m_end - m_begin;
    }

    size_type capacity() const {
        return m_limit - m_start;
    }

    size_type front_space() const {
        return m_begin - m_start;
    }

    size_type back_space() const {
        return m_limit - m_end;
    }

    bool empty() const {
        return size() == 0;
    }

    /**
     * reserve space in front
     * @param len total empty space in front
     */
    void spare_front(size_type len) {
        if (len == front_space()) {
            return;
        }
        size_type total_len = m_limit - m_begin + len;
        reallocate_and_relocate(total_len, len);
    }

    /**
     * reserve space at the end
     * @param len
     */
    void spare_back(size_type len) {
        if (len == back_space()) {
            return;
        }
        size_type total_len = m_end - m_start + len;
        reallocate_and_relocate(total_len, front_space());
    }

    /**
     * reserve even space in front and at the end
     * @param cap total capacity of new vector
     */
    void reserve(size_type cap) {
        if (cap < size()) {
            return;
        }
        auto left_len = (cap - size()) / 2;
        reserve(cap, left_len);
    }

    /**
     * reserve total and front space
     * @param cap
     * @param start
     */
    void reserve(size_type cap, size_type start) {
        if (start > cap) {
            throw std::invalid_argument("start");
        }
        if (start == front_space() && cap == capacity()) {
            return;
        }
        reallocate_and_relocate(cap, start);
    }

    reference front() const {
        return *m_begin;
    }

    reference back() const {
        return *(m_end - 1);
    }

    template<typename ...Args>
    void emplace_front(Args&&... args) {
        if (!front_space()) {
            if (AUTO_FRONT_RESIZE) {
                spare_front(std::max(size(), MIN_CAP));
            } else {
                throw std::runtime_error("no space in front");
            }
        }
        m_alloc.construct(m_begin - 1, std::forward<Args>(args)...);
        --m_begin;
    }

    void pop_front() {
        m_alloc.destroy(m_begin);
        ++m_begin;
        if (!AUTO_FRONT_RESIZE) {
            return;
        }
        auto left_len = m_end - m_start;
        if ((left_len / 4) >= size()) {
            spare_front(std::max(size(), MIN_CAP));
        }
    }

    template<typename ...Args>
    void emplace_back(Args&&... args) {
        if (!back_space()) {
            if (AUTO_BACK_RESIZE) {
                spare_back(std::max(size(), MIN_CAP));
            } else {
                throw std::runtime_error("no space at the back");
            }
        }
        m_alloc.construct(m_end, std::forward<Args>(args)...);
        ++m_end;
    }

    void pop_back() {
        m_alloc.destroy(m_end - 1);
        --m_end;
        if (!AUTO_BACK_RESIZE) {
            return;
        }
        auto right_len = m_limit - m_begin;
        if (right_len / 4 >= size()) {
            spare_back(std::max(size(), MIN_CAP));
        }
    }

    const value_type &operator[](size_type idx) const {
        return *(m_begin + idx);
    }

    value_type &operator[](size_type idx) {
        return *(m_begin + idx);
    }

    iterator begin() {
        return m_begin;
    }

    iterator end() {
        return m_end;
    }

    const_iterator begin() const {
        return m_begin;
    }

    const_iterator end() const {
        return m_end;
    }
};
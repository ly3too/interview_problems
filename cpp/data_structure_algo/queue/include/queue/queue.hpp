#pragma once

#include <utility>
#include "vector/vector.hpp"

template <typename T, typename Alloc = std::allocator<T>, size_t BLOCK_LEN = 512>
class Deque {
public:
    using value_type = T;
    using allocator_type = Alloc;
    using pointer = T *;
    using reference = T &;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

protected:
    using buffer_type = DqVector<T, Alloc, false, false, BLOCK_LEN>;
    using map_type = DqVector<buffer_type *>;

    map_type m_map = map_type(); // store inner buffer pointers
    allocator_type m_alloc = allocator_type();
public:
    explicit Deque(allocator_type alloc = allocator_type()): m_alloc(std::move(alloc)) {};

    explicit Deque(size_type n, const value_type &val = value_type(), allocator_type alloc = allocator_type()):
    m_alloc(std::move(alloc)) {
        while (n--) {
            emplace_back(val);
        }
    }

    template<typename Alloc2 = Alloc, int N = BLOCK_LEN>
    explicit Deque(const Deque<T, Alloc2, N>& other): Deque(other.m_alloc) {
        for (auto &item: other) {
            emplace_back(item);
        }
    }

    Deque(const Deque &other): Deque(other.m_alloc) {
        for (auto &item: other) {
            emplace_back(item);
        }
    }

    Deque(Deque&& other)  noexcept = default;

    void clear() {
        for (auto ptr: m_map) {
            delete ptr;
        }
        m_map.clear();
    }

    ~Deque() {
        clear();
    }

    template<typename ...Args>
    void emplace_back(Args&&... args) {
        if (m_map.empty() || m_map.back()->back_space() == 0) {
            auto new_back = new buffer_type(m_alloc);
            try {
                new_back->spare_back(BLOCK_LEN);
                new_back->emplace_back(std::forward<Args>(args)...);
                m_map.emplace_back(new_back);
            } catch (...) {
                delete new_back;
                throw;
            }
        } else {
            m_map.back()->emplace_back(std::forward<Args>(args)...);
        }
    }

    reference front() const {
        return m_map.front()->font();
    }

    void pop_front() {
        m_map.front()->pop_front();
        if (m_map.front()->empty()) {
            delete m_map.front();
            m_map.pop_front();
        }
    }

    template<typename ...Args>
    void emplace_front(Args&&... args) {
        if (m_map.empty() || m_map.front()->front_space() == 0) {
            auto new_front = new buffer_type(m_alloc);
            try {
                new_front->spare_front(BLOCK_LEN);
                new_front->emplace_front(std::forward<Args>(args)...);
                m_map.emplace_front(new_front);
            } catch (...) {
                delete new_front;
                throw;
            }
        } else {
            m_map.front()->emplace_front(std::forward<Args>(args)...);
        }
    }

    reference back() const {
        return m_map.back()->back();
    }

    void pop_back() {
        m_map.back()->pop_back();
        if (m_map.back()->empty()) {
            delete m_map.back();
            m_map.pop_back();
        }
    }

    bool empty() const {
        return m_map.empty();
    }

    size_type size() const {
        auto map_size = m_map.size();
        if (map_size == 0) {
            return 0;
        }
        if (map_size == 1) {
            return m_map.back()->size();
        }
        return (map_size - 2) * BLOCK_LEN + m_map.front()->size() + m_map.back()->size();
    }

    /**
     *
     * @param idx
     * @return
     */
    reference operator[](size_type idx) {
        auto front_buf = m_map.front();
        auto front_size = front_buf->size();
        if (idx < front_size) {
            return (*front_buf)[idx];
        }
        auto tail_len = idx - front_size;
        auto buf_idx = tail_len / BLOCK_LEN;
        auto remind = tail_len % BLOCK_LEN;
        return (*m_map[buf_idx + 1])[remind];
    }

    const value_type &operator[](size_type idx) const {
        return const_cast<Deque>(*this)[idx];
    }

    class const_iterator {
    public:
        using value_type = Deque::value_type;
        using reference = Deque::reference;
        using difference_type = Deque::difference_type;

    protected:
        using buf_it_type = typename buffer_type::iterator;
        using map_it_type = typename map_type::iterator;

        map_type *m_map;
        map_it_type m_map_cur;
        buf_it_type m_val_cur;

    public:
        const_iterator(map_type *map, map_it_type p_buf, buf_it_type p_val):
        m_map(map), m_map_cur(p_buf), m_val_cur(p_val) {}

        const_iterator(const const_iterator &other) = default;

        const_iterator operator+(difference_type diff) const {
            if (diff < 0) {
                diff = -diff;
                return (*this) - diff;
            }
            auto cur_remind = (*m_map_cur)->end() - m_val_cur;
            if (diff < cur_remind) {
                return const_iterator(m_map, m_map_cur, m_val_cur + diff);
            }
            auto map_diff = (cur_remind - diff) / BLOCK_LEN + 1;
            auto end_remind = (cur_remind - diff) % BLOCK_LEN;
            auto new_map_cur = m_map_cur + map_diff;
            if (new_map_cur >= m_map->end()) {
                return const_iterator(m_map, m_map->end(), nullptr);
            }
            auto new_val_cur = (*new_map_cur)->begin() + end_remind;
            return const_iterator(m_map, new_map_cur, new_val_cur);
        }

        const_iterator operator-(difference_type diff) const {
            if (diff < 0) {
                diff = -diff;
                return this->operator+(diff);
            }
            auto cur_remind = m_val_cur - (*m_map_cur)->begin();
            if (diff < cur_remind) {
                return const_iterator(m_map, m_map_cur, m_val_cur - diff);
            }
            auto map_diff = (cur_remind - diff) / BLOCK_LEN;
            auto beg_remind = (cur_remind - diff) % BLOCK_LEN;
            auto new_map_cur = m_map_cur - map_diff;
            if (beg_remind == 0) {
                return const_iterator(m_map, new_map_cur, (*new_map_cur)->begin());
            }

            --new_map_cur;
            if (new_map_cur < m_map->begin()) {
                return const_iterator(m_map, m_map->begin() - 1, nullptr);
            }
            auto new_val_cur = (*new_map_cur)->end() - beg_remind;
            return const_iterator(m_map, new_map_cur, new_val_cur);
        }

        bool operator==(const const_iterator &other) const {
            return m_val_cur == other.m_val_cur && m_map_cur == other.m_map_cur;
        };

        bool operator!=(const const_iterator &other) const {
            return !this->operator==(other);
        }

        const_iterator &operator++() {
            *this = *this + 1;
            return *this;
        }

        const_iterator &operator--() {
            *this = *this - 1;
            return *this;
        }

        const value_type &operator*() const {
            return *m_val_cur;
        }

        const value_type *operator->() const {
            return m_val_cur;
        }
    };

    class iterator: public const_iterator {
    protected:
        using buf_it_type = typename const_iterator::buf_it_type;
        using map_it_type = typename const_iterator::map_it_type;

    public:
        iterator(map_type *map, map_it_type p_buf, buf_it_type p_val):
        const_iterator(map, p_buf, p_val) {};

        iterator(const iterator &other) = default;

        iterator &operator++() {
            *this = *this + 1;
            return *this;
        }

        iterator &operator--() {
            *this = *this - 1;
            return *this;
        }

        const_iterator operator++(int) {
            auto ret = const_iterator(*this);
            *this = *this + 1;
            return ret;
        }

        const_iterator operator--(int) {
            auto ret = const_iterator(*this);
            *this = *this - 1;
            return ret;
        }

        value_type &operator*() {
            return *const_iterator::m_val_cur;
        }

        value_type *operator->() {
            return const_iterator::m_val_cur;
        }
    };

    iterator begin() {
        if (m_map.empty()) {
            return iterator(&m_map, m_map.begin(), nullptr);
        }
        auto map_beg = m_map.begin();
        return iterator(&m_map, map_beg, (*map_beg)->begin());
    }

    iterator end() {
        return iterator(&m_map, m_map.end(), nullptr);
    }

    const_iterator begin() const {
        return const_cast<Deque &>(*this).begin();
    }

    const_iterator end() const {
        return const_cast<Deque &>(*this).end();
    }
};


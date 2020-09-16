#pragma once
#include <type_traits>
#include <vector>
#include <functional>
#include <unordered_map>

/**
 * fix the heap from cur downward
 * @tparam T
 * @tparam Container
 * @tparam Cmp
 * @param container
 * @param cur
 * @param cmp
 */
template <typename T, typename Container = std::vector<T>,
        typename Cmp = std::less<typename Container::value_type>>
void heapify_down(Container &container, typename Container::size_type cur, const Cmp &cmp,
                  std::function<void (typename Container::size_type, typename Container::size_type)> cb
                    = [](auto i, auto j){}) {
    auto len = container.size();
    while (cur < len) {
        auto left = cur * 2 + 1;
        auto right = cur * 2 + 2;
        auto min = cur;

        // find the min idx
        if (left < len && cmp(container[left], container[min])) {
            min = left;
        }
        if (right < len && cmp(container[right], container[min])) {
            min = right;
        }
        if (min != cur) {
            cb(min, cur);
            std::swap(container[min], container[cur]);
            cur = min;
        } else {
            return;
        }
    }
}

/**
 * bubble up the current node
 * @tparam T
 * @tparam Container
 * @tparam Cmp
 * @param container
 * @param cur
 * @param cmp
 */
template <typename T, typename Container = std::vector<T>,
        typename Cmp = std::less<typename Container::value_type>>
void heapify_up(Container &container, typename Container::size_type cur, const Cmp &cmp,
                std::function<void (typename Container::size_type, typename Container::size_type)> cb
                    = [](auto i, auto j){}) {
    while (cur) {
        auto parent = (cur - 1) / 2;
        if (!cmp(container[cur], container[parent])) {
            break;
        }

        // move cur up to parent
        cb(cur, parent);
        std::swap(container[cur], container[parent]);
        cur = parent;
    }
}

/**
 * make the container to be a binary heap
 * @tparam T value type
 * @tparam Container
 * @tparam Cmp comparator
 * @param container input container
 * @param cmp comparator
 */
template <typename T, typename Container = std::vector<T>,
        typename Cmp = std::less<typename Container::value_type>>
void make_heap(Container &container, const Cmp &cmp = Cmp()) {
    auto len = container.size();
    if (len <= 1) {
        return;
    }

    auto cur = len/2 - 1;
    do {
        heapify_down<T>(container, cur, cmp);
    } while (cur--);
}

template <typename T, typename Container = std::vector<T>,
        typename Cmp = std::less<typename Container::value_type>>
class Heap {
public:
    using value_type = typename std::remove_const<
            typename std::remove_reference<T>::type>::type;
    using container_type = Container;

protected:
    container_type m_container;
    Cmp m_cmp;
public:

    Heap() = default;

    /**
     * init a heap from existing container
     * @param cont
     * @param c
     */
    Heap(Container &&cont, Cmp &&c = Cmp()): m_container(std::forward<Container>(cont)),
    m_cmp(std::forward<Cmp>(c)) {
        make_heap<T, Container, Cmp>(m_container, m_cmp);
    }

    /**
     * emplace a new element
     * @tparam Args
     * @param args
     */
    template<typename ...Args>
    void emplace(Args&& ...args) {
        m_container.emplace_back(std::forward<Args>(args)...);
        heapify_up<T, Container, Cmp>(m_container, m_container.size() - 1, m_cmp);
    }

    /**
     * get the top element
     * @return
     */
    value_type &top() {
        return m_container[0];
    }

    /**
     * pop the top
     */
    void pop() {
        std::swap(m_container[0], m_container[m_container.size() - 1]);
        m_container.pop_back();
        heapify_down<T, Container, Cmp>(m_container, 0, m_cmp);
    }

    /**
     * get the container reference
     * @return
     */
    container_type &getContainer() {
        return m_container;
    }

    /**
     * is empty?
     * @return
     */
    bool empty() {
        return m_container.empty();
    }

    /**
     * get the size of heap
     * @return
     */
    typename Container::size_type size() {
        return m_container.size();
    }
};

/**
 * a heap has relax key method
 * @tparam T
 * @tparam Container
 * @tparam Cmp
 */
template <typename T, typename Container = std::vector<T>,
        typename Cmp = std::less<typename Container::value_type>>
class RelaxHeap {
public:
    using value_type = typename std::remove_const<
            typename std::remove_reference<T>::type>::type;
    using container_type = Container;

protected:
    container_type m_container;
    Cmp m_cmp;
    std::unordered_map<value_type, typename container_type::size_type> m_map;

public:

    RelaxHeap() = default;

    /**
     * init a heap from existing container
     * @param cont
     * @param c
     */
    RelaxHeap(Container &&cont, Cmp &&c = Cmp()): m_container(std::forward<Container>(cont)),
    m_cmp(std::forward<Cmp>(c)) {
        make_heap<T, Container, Cmp>(m_container, m_cmp);
        for (typename container_type::size_type i = 0; i < m_container.size(); ++i) {
            m_map[m_container[i]] = i;
        }
    }

    /**
     * emplace a new element
     * @tparam Args
     * @param args
     */
    template<typename ...Args>
    void emplace(Args&& ...args) {
        m_container.emplace_back(std::forward<Args>(args)...);
        heapify_up<T, Container, Cmp>(m_container, m_container.size() - 1, m_cmp, [&](auto i, auto j){
            std::swap(m_map[m_container[i]], m_map[m_container[j]]);
        });
    }

    /**
     * get the top element
     * @return
     */
    value_type &top() {
        return m_container[0];
    }

    /**
     * pop the top
     */
    void pop() {
        m_map[m_container[m_container.size() - 1]] = 0;
        m_map.erase(m_container[0]);
        std::swap(m_container[0], m_container[m_container.size() - 1]);
        m_container.pop_back();
        heapify_down<T, Container, Cmp>(m_container, 0, m_cmp, [&](auto i, auto j) {
            std::swap(m_map[m_container[i]], m_map[m_container[j]]);
        });
    }

    /**
     * get the container reference
     * @return
     */
    container_type &getContainer() {
        return m_container;
    }

    /**
     * is empty?
     * @return
     */
    bool empty() {
        return m_container.empty();
    }

    /**
     * get the size of heap
     * @return
     */
    typename Container::size_type size() {
        return m_container.size();
    }

    /**
     *
     * @param val
     */
    void relaxKey(const value_type &val, T &&new_val) {
        auto old_idx = m_map[val];
        m_container[old_idx] = std::forward<T>(new_val);
        m_map.erase(val);
        m_map[new_val] = old_idx;
        heapify_up<T, Container, Cmp>(m_container, old_idx, m_cmp, [&](auto i, auto j){
            std::swap(m_container[m_map[i]], m_container[m_map[j]]);
        });
    }
};
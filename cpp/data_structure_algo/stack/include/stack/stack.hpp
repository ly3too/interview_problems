#pragma once

#include "list/list.hpp"
#include "queue/queue.hpp"

template <typename T, typename Alloc = std::allocator<T>, typename Cont = Deque<T, Alloc>>
class Stack {
public:
    using value_type = T;
    using reference = T &;
    using pointer = T *;
    using size_type = typename Cont::size_type;

protected:

    Cont m_cont = Cont();

public:
    Stack() = default;

    Stack(size_type len, const value_type &val = value_type(), Alloc alloc = Alloc()):
    m_cont(len, val, std::move(alloc)) {}

    template<typename ...Args>
    void emplace(Args&&... args) {
        m_cont.emplace_back(std::forward<Args>(args)...);
    }

    reference top() {
        return m_cont.back();
    }

    const value_type &top() const {
        return m_cont.back();
    }

    void pop() {
        m_cont.pop_back();
    }

    size_type size() const {
        return m_cont.size();
    }

    bool empty() const {
        return m_cont.empty();
    }
};
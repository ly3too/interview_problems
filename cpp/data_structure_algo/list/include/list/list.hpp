#pragma once

#include <memory>

/**
 * single linked list
 * @tparam T
 */
template<typename T>
class ForwardList {
public:
    using value_type = T;

protected:
    class NodeBase {
    public:
        NodeBase(): next(nullptr) {

        }

        NodeBase *next;
    };

    class Node: public NodeBase {
    public:
        explicit Node(T&& val): NodeBase(), m_val(std::forward<T>(val)) {

        }

        T m_val;
    };

    NodeBase m_head;
    std::size_t m_size;
public:
    class iterator {
    public:
        using value_type = std::remove_reference_t<T>;

        iterator(NodeBase *cur): m_cur(cur) {

        }

        iterator &operator++() {
            m_cur = m_cur->next;
            return *this;
        }

        value_type &operator*() {
            return static_cast<Node *>(m_cur)->m_val;
        }

        value_type *operator->() {
            return &(static_cast<Node *>(m_cur)->m_val);
        }

        bool operator==(const iterator &other) {
            return m_cur == other.m_cur;
        }

        bool operator!=(const iterator &other) {
            return m_cur != other.m_cur;
        }

    protected:
        NodeBase *m_cur;

        friend class ForwardList;
    };

    ForwardList(): m_size(0) {

    }

    ~ForwardList() {
        clear();
    }

    ForwardList(ForwardList &&)  noexcept = default;
    ForwardList(ForwardList &) = delete;
    ForwardList &operator=(ForwardList &) = delete;
    ForwardList &operator=(ForwardList &&)  noexcept = default;

    void clear() {
        auto cur = m_head.next;
        while(cur) {
            auto prev = static_cast<Node *>(cur);
            cur = cur->next;
            delete prev;
        }
        m_head.next = nullptr;
        m_size = 0;
    }

    std::size_t size() const {
        return m_size;
    }

    bool empty() const {
        return m_size == 0;
    }

    iterator begin() const {
        return iterator(m_head.next);
    }

    iterator end() const {
        return iterator(nullptr);
    }

    /**
     * emplace node after cur
     * @param cur
     * @param val
     */
    template<typename... Args>
    void emplace_after(iterator cur, Args&&... val) {
        auto node = new Node(std::forward<Args>(val)...);
        node->next = cur.m_cur->next;
        cur.m_cur->next = node;
        ++m_size;
    }

    /**
     * emplace val at front
     * @param val
     */
    template<typename ...Args>
    void emplace_front(Args&&... val) {
        return emplace_after(iterator(&m_head), std::forward<Args>(val)...);
    }

    /**
     * remove a node
     * @param val
     */
    void remove(T &&val) {
        auto prev = static_cast<Node *>(&m_head);
        auto cur = static_cast<Node *>(prev->next);
        while (cur) {
            if (cur->m_val == val) {
                prev->next = cur->next;
                --m_size;
                delete cur;
                return;
            }
            prev = cur;
            cur = static_cast<Node *>(cur->next);
        }
    }

    /**
     * erase on node after cur node, cur can be nullptr which means the head
     * @param cur
     */
    void erase_after(iterator cur) {
        if (!cur.m_cur) {
            cur.m_cur = &m_head;
        }
        auto node = static_cast<Node *>(cur.m_cur->next);
        cur.m_cur->next = node->next;
        --m_size;
        delete node;
    }
};

template<typename T, typename Alloc = std::allocator<T>>
class List {
public:
    using value_type = std::remove_reference_t<T>;
    using size_type = std::size_t;
    using allocator_type = Alloc;
    using reference = value_type &;

protected:
    struct BaseNode {
        BaseNode *prev;
        BaseNode *next;
        BaseNode(): prev(this), next(this) {}
    };

    struct Node: public BaseNode {
        value_type val;

        template<typename ...Args>
        Node(Args&&... val): BaseNode(), val(std::forward<Args>(val)...) {

        }
    };

    BaseNode m_head;
    std::size_t m_size;
    allocator_type m_alloc = allocator_type();
public:
    class iterator {
    public:
        iterator(BaseNode *node): m_cur(node) {}

        iterator operator++() {
            m_cur = m_cur->next;
            return *this;
        }

        iterator operator--() {
            m_cur = m_cur->prev;
            return *this;
        }

        value_type &operator*() {
            return static_cast<Node *>(m_cur)->val;
        }

        value_type &operator->() {
            return &static_cast<Node *>(m_cur)->val;
        }

        bool operator!=(const iterator &other) {
            return m_cur != other.m_cur;
        }

        bool operator==(const iterator &other) {
            return m_cur == other.m_cur;
        }

    protected:
        BaseNode *m_cur;

        friend class List;
    };

    List(allocator_type alloc = allocator_type()):
    m_head(), m_size(0), m_alloc(std::move(alloc)) {
    }

    explicit List(size_type n, const value_type &val = value_type(),
                  allocator_type alloc = allocator_type()): List(std::move(alloc)) {
        while(n--) {
            emplace_back(val);
        }
    }

    List(const std::initializer_list<value_type> &vals): List() {
        for (auto val: vals) {
            emplace_back(std::move(val));
        }
    }

    ~List() {
        clear();
    }

    void clear() {
        auto cur = m_head.next;
        while(cur != &m_head) {
            auto node = static_cast<Node *>(cur);
            cur = cur->next;
            delete node;
        }
        m_head.prev = &m_head;
        m_head.next = &m_head;
        m_size = 0;
    }

    std::size_t size() const {
        return m_size;
    }

    template<typename ... Args>
    void emplace_back(Args&&... val) {
        auto prev = m_head.prev;
        auto next = &m_head;
        auto node = new Node(std::forward<Args>(val)...);
        node->next = next;
        next->prev = node;
        prev->next = node;
        node->prev = prev;
        ++m_size;
    }

    template<typename ... Args>
    void emplace_front(Args&&... val) {
        auto prev = &m_head;
        auto next = m_head.next;
        auto node = new Node(std::forward<Args>(val)...);
        node->next = next;
        next->prev = node;
        prev->next = node;
        node->prev = prev;
        ++m_size;
    }

    /**
     * emplace in front of it
     * @param it
     * @param val
     */
    void empalce(const iterator &it, T &&val) {
        if (!it.m_cur) {
            return emplace_back(std::forward<T>(val));
        }
        auto node = new Node(std::forward<T>(val));

        auto next = it.m_cur;
        auto prev = it.m_cur->prev;

        node->next = next;
        next->prev = node;
        prev->next = node;
        node->prev = prev;
        ++m_size;
    }

    void erase(const iterator &it) {
        auto prev = it.m_cur->prev;
        auto next = it.m_cur->next;
        prev->next = next;
        next->prev = prev;
        --m_size;
        delete static_cast<Node *>(it.m_cur);
    }

    void remove(const value_type &val) {
        for (auto it = begin(); it != end(); ++it) {
            if (*it == val) {
                erase(it);
                return;
            }
        }
    }

    iterator begin() const {
        return iterator(const_cast<BaseNode *>(m_head.next));
    }

    iterator end() const {
        return iterator(const_cast<BaseNode *>(&m_head));
    }

    /**
     * @warning const method return a non const reference for simplicity
     * @return
     */
    reference front() const {
        return static_cast<Node *>(m_head.next)->val;
    }

    reference back() const {
        return static_cast<Node *>(m_head.prev)->val;
    }

    void pop_back() {
        erase(--end());
    }

    void pop_front() {
        erase(begin());
    }

    bool empty() const {
        return m_size == 0;
    }
};
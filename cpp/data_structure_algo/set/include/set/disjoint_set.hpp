#pragma once
#include <memory>
#include <forward_list>

template <typename T, typename Alloc = std::allocator<T>>
class DisjointSetHelper {
public:
    using value_type = T;

    struct Node {
        value_type val;
        Node* parent;

        template<typename ...Args>
        explicit Node(Args&& ...args): val(std::forward<Args>(args)...), parent(nullptr) {}
    };

    using node_type = Node;
    using node_allocator = typename Alloc::template rebind<Node>::other;

protected:

    std::forward_list<Node, node_allocator> m_elements = {};
public:
    explicit DisjointSetHelper(const Alloc& alloc = Alloc()): m_elements(node_allocator(alloc)) {}

    /**
     * make a one node set
     *
     * @tparam Args
     * @param args
     * @return node, maintained internally
     */
    template<typename ...Args>
    Node* makeSet(Args&& ...args) {
        m_elements.emplace_front(Node(std::forward<Args>(args)...));
        return &m_elements.front();
    }

    /**
     * find the root of disjoint set, with path compress
     *
     * @param node
     * @return root node
     */
    Node* find(Node* node) {
        if (!node->parent) {
            return node;
        }
        auto parent = find(node->parent);
        node->parent = parent;
        return parent;
    }

    /**
     * union two set by any two nodes
     *
     * @param left
     * @param right
     * @return the root of new set;
     */
    Node* unionSet(Node* left, Node *right) {
        auto l_root = find(left);
        auto r_root = find(right);
        if (l_root != r_root) {
            r_root->parent = l_root;
        }
        return l_root;
    }
};

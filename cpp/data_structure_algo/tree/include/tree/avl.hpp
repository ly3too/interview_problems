#pragma once

#include "bstree.hpp"

namespace bstree {

template<typename Key, typename Val, typename Cmp = std::less<Key>,
    typename Alloc = std::allocator<std::pair<Key, Val>>>
class AVLMap {
public:
    using key_type = Key;
    using mapped_type = Val;
    using value_type = std::pair<const key_type, mapped_type>;
    using size_type = std::size_t;
    using allocator_type = Alloc;
    using key_compare = Cmp;

protected:
    using node_type = BSTAugNode<value_type, int>;
    using node_alloc_type = typename Alloc::template rebind<node_type>::type;

    node_type *m_root = nullptr;
    node_alloc_type m_alloc = node_alloc_type();
    key_compare m_cmp = key_compare();
    size_type m_count = 0;

    class PairKeyCmp {
    protected:
        const key_compare &m_cmp;
    public:
        PairKeyCmp(const key_compare &cmp): m_cmp(cmp) {}

        bool operator()(const value_type &l, const value_type &r) {
            return m_cmp(l.first, r.first);
        }
    };

public:
    using iterator = BstIterator<value_type, node_type>;
    using const_iterator = BstConstIterator<value_type, node_type>;

    AVLMap(const key_compare &cmp = key_compare(), const allocator_type &alloc = allocator_type()):
    m_cmp(cmp), m_alloc(alloc) {}

    void clear() {
        PostOrderTrav(m_root, [&](auto ptr){
            m_alloc.destroy(ptr);
            m_alloc.deallocate(ptr, 1);
        });
        m_root = nullptr;
        m_count = 0;
    }

    ~AVLMap() {
        clear();
    }

    // warning return no const iterator for simplicity
    iterator find(const value_type &val) const {
        auto node = BstSearch(m_root, val, PairKeyCmp(m_cmp));
        return iterator(node);
    }

    iterator begin() const {
        return iterator(findLeftMost(m_root));
    }

    iterator end() const {
        return iterator(nullptr);
    }

    iterator rbegin() const {
        return iterator(findRightMost(m_root));
    }

    iterator rend() const {
        return iterator(nullptr);
    }

    template<typename Arg>
    std::pair<iterator, bool> insert(Arg &&arg) {
        auto node = BstSearch(m_root, arg, m_cmp);
        if (!node) {
            node = m_alloc.allocate(1);
            m_alloc.construct(node, std::forward<Arg>(arg));
            if (!m_root) {
                m_root = node;
            } else {
                BstInsert(m_root, node);
            }

            fixUp(node);
            return std::make_pair(iterator(node), true);
        }

        auto aug = node->aug;
        node->val = 
        return std::make_pair(iterator(node), false);
    }
};

}

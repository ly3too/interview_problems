#pragma once
#include "btree.hpp"

namespace bstree {
    using namespace btree;

    template<typename NT>
    struct BSTBaseNode: public BaseNode<NT> {
        NT *parent = nullptr;
        inline NT *getParent() {
            return parent;
        }
    };

    template<typename T>
    struct BSTNode: public BaseNode<BSTNode<T>> {
        using value_type = T;
        value_type val;

        template<typename ...Args>
        explicit BSTNode(Args&&... args): BaseNode<BSTNode<T>>(), val(std::forward<Args>(args)...) {}
    };

    // augmented tree node, red-back tree, avl tree
    template<typename T, typename AT>
    struct BSTAugNode: public BaseNode<BSTAugNode<T, AT>> {
        using value_type = T;
        using augment_type = AT;
        value_type val;
        augment_type aug = augment_type();

        template<typename ...Args>
        explicit BSTAugNode(Args&&... args): BaseNode<BSTAugNode<T, AT>>(), val(std::forward<Args>(args)...) {}

        template<typename ...Args>
        explicit BSTAugNode(const augment_type &a, Args&&... args):
        BaseNode<BSTAugNode<T, AT>>(), val(std::forward<Args>(args)...), aug(a) {}
    };
}

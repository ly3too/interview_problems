#pragma once

#include <utility>
#include <functional>
#include <ostream>
#include "stack/stack.hpp"

namespace btree {
    template<typename NT>
    struct BaseNode {
        NT *left = nullptr;
        NT *right = nullptr;

        // can be used to mirror the traversal
        inline NT *getLeft() {
            return left;
        }

        inline NT *getRight() {
            return right;
        }

        inline void setLeft(NT *l) {
            left = l;
        }

        inline void setRight(NT *r) {
            right = r;
        }
    };

    template<typename T>
    struct Node: public BaseNode<Node<T>> {
        using value_type = T;
        value_type val;

        template<typename ...Args>
        explicit Node(Args&&... args): BaseNode<Node<T>>(), val(std::forward<Args>(args)...) {}
    };

    /**
     * inorder traverse of tree
     * @tparam T value type
     * @param root root of tree
     * @param call call on each node, return true if continue else return false
     * @return return the last node which call return false
     */
    template <typename NT, typename Call = std::function<bool (NT *)>>
    NT *InOrderTravRecur(NT *root, const Call &call) {
        if (!root) {
            return nullptr;
        }
        auto left = InOrderTravRecur<NT, Call>(root->getLeft(), call);
        if (left) {
            return left;
        }
        if (call(root) == false) {
            return root;
        }
        return InOrderTravRecur<NT, Call>(root->getRight(), call);
    }

    /**
     * post order traversal of tree
     * @tparam T value type
     * @param root root node
     * @param call call on each node, return true if continue else return false
     * @return return the node which call returns false, otherwise nullptr
     */
    template <typename NT, typename Call = std::function<bool (NT *)>>
    NT *PostOrderTravRecur(NT *root, const Call &call) {
        if (!root) {
            return nullptr;
        }
        auto left = PostOrderTravRecur<NT, Call>(root->getLeft(), call);
        if (left) {
            return left;
        }
        auto right = PostOrderTravRecur<NT, Call>(root->getRight(), call);
        if (right) {
            return right;
        }
        if (call(root) == false) {
            return root;
        }
        return nullptr;
    }

    /**
     * pre order traversal of tree
     * @tparam T value type
     * @param root root node
     * @param call call on each node, return true if continue else return false
     * @return return the node which call returns false, otherwise nullptr
     */
    template <typename NT, typename Call = std::function<bool (NT *)>>
    NT * PreOrderTravRecur(NT *root, const Call &call) {
        if (!root) {
            return nullptr;
        }
        if (call(root) == false) {
            return root;
        }
        auto left = PreOrderTravRecur<NT, Call>(root->getLeft(), call);
        if (left) {
            return left;
        }
        return PreOrderTravRecur<NT, Call>(root->getRight(), call);
    }

    /**
     * in order traverse of tree. non recursively
     * @tparam T value type
     * @param root root of tree
     * @param call call on each node, return true if continue else return false
     * @return return the last node which call return false
     */
    template <typename NT, typename Call = std::function<bool (NT *)>>
    NT *InOrderTrav(NT *root, const Call &call) {
        if (!root) {
            return nullptr;
        }

        using PNode = NT *;
        auto stack = Stack<PNode, std::allocator<PNode>, Vector<PNode>>();
        stack.emplace(root);
        while(!stack.empty()) {
            auto top = stack.top();
            if (top) {
                stack.pop();

                if (top->getRight()) stack.emplace(top->getRight()); // right
                stack.emplace(top); // middle
                stack.emplace(nullptr);
                if (top->getLeft()) stack.emplace(top->getLeft()); // left
            } else {
                stack.pop();
                auto cur = stack.top();
                stack.pop();
                if (!call(cur)) {
                    return cur;
                }
            }
        }
        return nullptr;
    }

    /**
     * pre order traverse of tree. non recursively
     * @tparam T value type
     * @param root root of tree
     * @param call call on each node, return true if continue else return false
     * @return return the last node which call return false
     */
    template <typename NT, typename Call = std::function<bool (NT *)>>
    NT *PreOrderTrav(NT *root, const Call &call) {
        if (!root) {
            return nullptr;
        }

        using PNode = NT *;
        auto stack = Stack<PNode, std::allocator<PNode>, Vector<PNode>>();
        stack.emplace(root);
        while(!stack.empty()) {
            auto top = stack.top();
            if (top) {
                stack.pop();

                if (top->getRight()) stack.emplace(top->getRight()); // right
                if (top->getLeft()) stack.emplace(top->getLeft()); // left
                stack.emplace(top); // middle
                stack.emplace(nullptr);
            } else {
                stack.pop();
                auto cur = stack.top();
                stack.pop();
                if (!call(cur)) {
                    return cur;
                }
            }
        }
        return nullptr;
    }

    /**
     * pre order traverse of tree. non recursively
     * @tparam T value type
     * @param root root of tree
     * @param call call on each node, return true if continue else return false
     * @return return the last node which call return false
     */
    template <typename NT, typename Call = std::function<bool (NT *)>>
    NT *PostOrderTrav(NT *root, const Call &call) {
        if (!root) {
            return nullptr;
        }

        using PNode = NT *;
        auto stack = Stack<PNode, std::allocator<PNode>, Vector<PNode>>();
        stack.emplace(root);
        while(!stack.empty()) {
            auto top = stack.top();
            if (top) {
                stack.pop();

                stack.emplace(top); // middle
                stack.emplace(nullptr);
                if (top->getRight()) stack.emplace(top->getRight()); // right
                if (top->getLeft()) stack.emplace(top->getLeft()); // left
            } else {
                stack.pop();
                auto cur = stack.top();
                stack.pop();
                if (!call(cur)) {
                    return cur;
                }
            }
        }
        return nullptr;
    }

template<typename NT, typename Format = std::function<std::string(NT *)>>
void PrintTree(std::ostream &out, NT *node, const Format &ft = [](auto ptr){return std::to_string(ptr->val);}, const std::string& pre = "") {
    if (!node) {
        return;
    }

    auto parent = node->getParent();
    if (!parent) {
        PrintTree(out, node->getRight(), ft, pre);
        out << ft(node) << std::endl;
        PrintTree(out, node->getLeft(), ft, pre);
    } else if (parent->getLeft() == node) {
        PrintTree<NT, Format>(out, node->getRight(), ft, pre + " | ");
        out << pre << "  \\" << ft(node) << std::endl;
        PrintTree<NT, Format>(out, node->getLeft(), ft, pre + "   ");
    } else {
        PrintTree<NT, Format>(out, node->getRight(), ft, pre + "   ");
        out << pre << "  /" << ft(node) << std::endl;
        PrintTree<NT, Format>(out, node->getLeft(), ft, pre + " | ");
    }
}
}
#pragma once

#include <utility>
#include <functional>
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
    };

    template<typename T>
    struct Node: public BaseNode<Node<T>> {
        using value_type = T;
        T val;

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
    template <typename T>
    Node<T> * InOrderTravRecur(Node<T> *root, const std::function<bool (Node<T> *)> &call) {
        if (!root) {
            return nullptr;
        }
        auto left = InOrderTravRecur(root->getLeft(), call);
        if (left) {
            return left;
        }
        if (call(root) == false) {
            return root;
        }
        return InOrderTravRecur(root->getRight(), call);
    }

    /**
     * post order traversal of tree
     * @tparam T value type
     * @param root root node
     * @param call call on each node, return true if continue else return false
     * @return return the node which call returns false, otherwise nullptr
     */
    template <typename T>
    Node<T> * PostOrderTravRecur(Node<T> *root, const std::function<bool (Node<T> *)> &call) {
        if (!root) {
            return nullptr;
        }
        auto left = PostOrderTravRecur(root->getLeft(), call);
        if (left) {
            return left;
        }
        auto right = PostOrderTravRecur(root->getRight(), call);
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
    template <typename T>
    Node<T> * PreOrderTravRecur(Node<T> *root, const std::function<bool (Node<T> *)> &call) {
        if (!root) {
            return nullptr;
        }
        if (call(root) == false) {
            return root;
        }
        auto left = PreOrderTravRecur(root->getLeft(), call);
        if (left) {
            return left;
        }
        return PreOrderTravRecur(root->getRight(), call);
    }

    /**
     * in order traverse of tree. non recursively
     * @tparam T value type
     * @param root root of tree
     * @param call call on each node, return true if continue else return false
     * @return return the last node which call return false
     */
    template <typename T>
    Node<T> * InOrderTrav(Node<T> *root, const std::function<bool (Node<T> *)> &call) {
        if (!root) {
            return nullptr;
        }

        using PNode = Node<T> *;
        auto stack = Stack<PNode, std::allocator<PNode>, Vector<PNode>>();
        stack.emplace(root);
        while(!stack.empty()) {
            auto top = stack.top();
            if (top) {
                stack.pop();

                if (top->right) stack.emplace(top->getRight()); // right
                stack.emplace(top); // middle
                stack.emplace(nullptr);
                if (top->left) stack.emplace(top->getLeft()); // left
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
    template <typename T>
    Node<T> * PreOrderTrav(Node<T> *root, const std::function<bool (Node<T> *)> &call) {
        if (!root) {
            return nullptr;
        }

        using PNode = Node<T> *;
        auto stack = Stack<PNode, std::allocator<PNode>, Vector<PNode>>();
        stack.emplace(root);
        while(!stack.empty()) {
            auto top = stack.top();
            if (top) {
                stack.pop();

                if (top->right) stack.emplace(top->getRight()); // right
                if (top->left) stack.emplace(top->getLeft()); // left
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
    template <typename T>
    Node<T> * PostOrderTrav(Node<T> *root, const std::function<bool (Node<T> *)> &call) {
        if (!root) {
            return nullptr;
        }

        using PNode = Node<T> *;
        auto stack = Stack<PNode, std::allocator<PNode>, Vector<PNode>>();
        stack.emplace(root);
        while(!stack.empty()) {
            auto top = stack.top();
            if (top) {
                stack.pop();

                stack.emplace(top); // middle
                stack.emplace(nullptr);
                if (top->right) stack.emplace(top->getRight()); // right
                if (top->left) stack.emplace(top->getLeft()); // left
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
}
#pragma once

#include "vector/vector.hpp"

template<typename T, typename Alloc = std::allocator<T>>
class Deque {
public:
    using value_type = T;
    using allocator_type = Alloc;
    using reference = value_type &;
    using pointer = value_type *;

protected:
public:
};


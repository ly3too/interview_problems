#pragma once

#include <iostream>
#include <type_traits>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

template<typename ...>
using to_void = void; // maps everything to void, used in non-evaluated contexts

//specialize a type for all of the STL containers.
namespace is_stl_container_impl{
    template <typename T>       struct is_stl_container:std::false_type{};
    template <typename T, std::size_t N> struct is_stl_container<std::array    <T,N>>    :std::true_type{};
    template <typename... Args> struct is_stl_container<std::vector            <Args...>>:std::true_type{};
    template <typename... Args> struct is_stl_container<std::deque             <Args...>>:std::true_type{};
    template <typename... Args> struct is_stl_container<std::list              <Args...>>:std::true_type{};
    template <typename... Args> struct is_stl_container<std::forward_list      <Args...>>:std::true_type{};
    template <typename... Args> struct is_stl_container<std::set               <Args...>>:std::true_type{};
    template <typename... Args> struct is_stl_container<std::multiset          <Args...>>:std::true_type{};
    template <typename... Args> struct is_stl_container<std::map               <Args...>>:std::true_type{};
    template <typename... Args> struct is_stl_container<std::multimap          <Args...>>:std::true_type{};
    template <typename... Args> struct is_stl_container<std::unordered_set     <Args...>>:std::true_type{};
    template <typename... Args> struct is_stl_container<std::unordered_multiset<Args...>>:std::true_type{};
    template <typename... Args> struct is_stl_container<std::unordered_map     <Args...>>:std::true_type{};
    template <typename... Args> struct is_stl_container<std::unordered_multimap<Args...>>:std::true_type{};
    template <typename... Args> struct is_stl_container<std::stack             <Args...>>:std::true_type{};
    template <typename... Args> struct is_stl_container<std::queue             <Args...>>:std::true_type{};
    template <typename... Args> struct is_stl_container<std::priority_queue    <Args...>>:std::true_type{};
}

//type trait to utilize the implementation type traits as well as decay the type
template <typename T> struct is_container {
    static constexpr bool const value = is_stl_container_impl::is_stl_container<std::decay_t<T>>::value;
};


//template<typename C>
//class Format {
//public:
//    using container_type = std::remove_reference_t<C>;
//
//protected:
//    const container_type &m_cont;
//    const char *m_deli;
//public:
//    Format(const container_type &c, const char *deli = ", "): m_cont(c), m_deli(deli) {
//    }
//
//    template <typename Cc>
//    friend std::ostream& operator<<(std::ostream &out, Format<Cc> &me);
//
//};

template<typename T, typename U>
std::ostream& operator<<(std::ostream &out, const std::pair<T, U> &p) {
    out << '[' << p.first << ", " << p.second << "]";
    return out;
}

template <class charT, class traits, class T, class... Args, template<typename, typename...> class Tclass>
std::basic_ostream<charT,traits>& operator<< (std::basic_ostream<charT,traits>& out, const Tclass<T, Args...>& c) {
    out << "{";
    for (auto p=c.begin(), end=c.end(); p != end;) {
        out << *p;
        ++p;
        if (p != end) {
            out << ", ";
        }
    }
    out << "}";
    return out;
}



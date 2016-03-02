/*
 * SFINAE & variadic template
 * */
#include <iostream>
#include <string>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <vector>
#include <deque>
#include <list>
#include <type_traits>

template<typename ...>
struct is_associative : public std::false_type {};

template<typename ... Args>
struct is_associative<std::set<Args ...>> : public std::true_type {};

template<typename ... Args>
struct is_associative<std::unordered_set<Args ...>> : public std::true_type {};

template<typename ... Args>
struct is_associative<std::map<Args ...>> : public std::true_type {};

template<typename ... Args>
struct is_associative<std::unordered_map<Args ...>> : public std::true_type {};

template<typename U, typename V>
std::ostream& operator<<(std::ostream &os, const std::pair<U, V> &val) {
    return (os << val.first << ": " << val.second);
};

template<typename ...>
struct is_sequence : public std::false_type {};

template<typename ... Args>
struct is_sequence<std::vector<Args ...>> : public std::true_type {};

template<typename ... Args>
struct is_sequence<std::deque<Args ...>> : public std::true_type {};

template<typename ... Args>
struct is_sequence<std::list<Args ...>> : public std::true_type {};

template<typename T, typename D = typename std::enable_if<is_associative<T>::value>::type>
std::ostream& operator<<(std::ostream &os, const T &xs) {
    os << "{";
    auto it = xs.begin();
    if(it != xs.end()) {
        os << *it++;
    }
    while(it != xs.end()) {
        os << ", " << *it++;
    }
    return (os << "}");
}

template<typename T, typename D = typename std::enable_if<is_sequence<T>::value>::type,
                     typename E = typename std::enable_if<is_sequence<T>::value>::type>
std::ostream& operator<<(std::ostream &os, const T &xs) {
    os << "[";
    auto it = xs.begin();
    if(it != xs.end()) {
        os << *it++;
    }
    while(it != xs.end()) {
        os << ", " << *it++;
    }
    return (os << "]");
}

int main() {
    std::vector<std::unordered_map<std::string, int>> x{
        {{"alpha", 0}, {"beta", 1}, {"gamma", 2}},
        {{"revere", 1}, {"ibes", 2}, {"lionshare", 3}, {"dataexplorers", -1}},
        {{"sklearn", 2}, {"tensorflow", 3}}
    };
    std::cout << x << std::endl;

    std::map<std::string, std::unordered_map<int, std::vector<int>>> y{
        {"alpha", {{0, {1, 2, 3}}, {1, {2, 3}}, {2, {3, 4, 5}}}},
        {"beta", {{-1, {-1, 0, 1}}, {-3, {-7, -8, -9}}}},
        {"gamma", {{9, {5, 5, 5}}, {8, {6, 6, 6}}, {7, {7, 7, 7}}}}
    };
    std::cout << y << std::endl;

    return 0;
}
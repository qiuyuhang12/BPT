//
// Created by qiuyuhang on 24-4-11.
//

#ifndef MAP_MY_SORT_HPP
#define MAP_MY_SORT_HPP

#include <iterator>
#include <algorithm>
#include <type_traits>
#include "map.hpp"
#include <cassert>
// sjtu::map<int, int>::iterator::operator +(int n) const;

namespace sjtu {
    template<typename _Tp>
    static constexpr bool has_rand = requires(const _Tp &x) {
        x + int();
        *x;
    };
    template<typename _Tp>
    static constexpr bool is_my_itr = requires() {
        _Tp::myItr;
    };

    template<typename _Iter>
    void my_sort(_Iter __first, _Iter __last) {
        if constexpr (has_rand<_Iter>) {
            std::sort(__first, __last);
            return;
        }
//        if constexpr (is_my_itr<_Iter>)return;
        else if constexpr (is_my_itr<_Iter>)return;
//        assert(0);
//        if constexpr (true)static_assert(false,"Not a random access iterator.");


//        if constexpr (!has_rand<_Iter>&&!is_my_itr<_Iter>)static_assert(false,"Not a random access iterator.");
        else static_assert(false,"Not a random access iterator.");

        // Your code here.
        // For random access iterator, use std::sort.
        // For sjtu::map iterator, return directly.
        // For others, static_assert(false, "Not a random access iterator.");
        // You may use std::iterator_traits to get the iterator category.
    }

}
#endif //MAP_MY_SORT_HPP

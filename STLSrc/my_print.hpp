//
// Created by qiuyuhang on 24-4-11.
//

#ifndef MAP_MY_PRINT_HPP
#define MAP_MY_PRINT_HPP
#include <iterator>
#include <algorithm>
#include <type_traits>
#include <vector>
#include <iostream>

namespace sjtu {
    template <typename _Tp>
    static constexpr bool is_container=requires(const _Tp&tp){
        std::begin(tp);
        std::end(tp);
    };
    template <typename _Tp>
    void print(const _Tp &__val) {
        if constexpr (is_container<_Tp>){
            std::cout<<std::endl<<"[";
            for(auto i=std::begin(__val);i!=std::end(__val);i++){
                print(*i);
            }
            std::cout<<"]"<<std::endl;
        }
        if constexpr (!is_container<_Tp>){
            std::cout<<__val<<' ';
        }
    }

} // namespace sjtu
void sample() {
    std::vector<int> a[5];
    for (int i = 0 ; i < 5 ; ++i)
        a[i].push_back(i), a[i].push_back(i);
    sjtu::print(a);
}
#endif //MAP_MY_PRINT_HPP

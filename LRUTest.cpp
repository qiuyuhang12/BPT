//
// Created by qiuyuhang on 24-4-26.
//
#include "LRU.hpp"
#include <iostream>
#include <cassert>
#include <cstring>
#include <map>
#include <algorithm>

//size_t hash(int x) {
//    return x%LRU<int, int, std::hash<int>>::TableCapacity;
//}

struct hash {
    size_t operator()(int x) const {
        return x % TableCapacity;
    }
};
int main() {
    LRU<int, int, hash> lru;
    std::map<int, int> mp;
    for (int i = 0; i < 4e4; ++i) {
//        if (i%1000==0)
        std::cout << i << std::endl;
        int k = rand()/1e3;
        int k2= rand()/1e3;
        if (mp.count(k))continue;
        mp[k] = k2;
        if (mp.size() == LinkCapacity+1) {
            int del=lru.tail->prev->key;
            mp.erase(del);
//            break;
        }
        lru.insert(k, k2);
        assert(lru.size == mp.size());
        for (auto item: mp) {
            int val = -1;
            assert(lru.get(item.first, val));
            assert(val!=-1&&val == item.second);
        }
    }
    return 0;
}
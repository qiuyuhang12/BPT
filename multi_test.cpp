#include "BPT.hpp"
#include <cstring>
#include <iostream>
#include <map>
#include <algorithm>

struct Int {
    int val;
    bool isMin;
    int realVal;

    Int() : val(0), isMin(false) {}

    Int(int _val, bool _isMin, int _realVal) : val(_val), isMin(_isMin), realVal(_realVal) {}

    explicit Int(int _val) : val(_val), isMin(false) {}

    Int(int _val, int _realVal) : val(_val), isMin(false), realVal(_realVal) {}

    Int(int val, bool isMin) : val(val), isMin(isMin) {}

    Int(const Int &other) : val(other.val), isMin(other.isMin), realVal(other.realVal) {}

    bool operator<(const Int &other) const {
        return val < other.val;
    }

    bool operator==(const Int &other) const {
        return val == other.val;
    }

    bool operator>(const Int &other) const {
        return val > other.val;
    }

    bool operator<=(const Int &other) const {
        return val <= other.val;
    }

    bool operator>=(const Int &other) const {
        return val >= other.val;
    }

    bool operator!=(const Int &other) const {
        return val != other.val;
    }
//
//    bool operator<(const Int &other) const {
//        if (val != other.val)return val < other.val;
//        return realVal < other.realVal;
//    }
//
//    bool operator==(const Int &other) const {
//        return val == other.val && realVal == other.realVal;
//    }
//
//    bool operator>(const Int &other) const {
//        if (val != other.val)return val > other.val;
//        return realVal > other.realVal;
//    }
//
//    bool operator<=(const Int &other) const {
//        if (val != other.val)return val <= other.val;
//        return realVal <= other.realVal;
//    }
//
//    bool operator>=(const Int &other) const {
//        if (val != other.val)return val >= other.val;
//        return realVal >= other.realVal;
//    }
//
//    bool operator!=(const Int &other) const {
//        return val != other.val || realVal != other.realVal;
//    }
//        return val != other.val;
};

std::ostream &operator<<(std::ostream &os, const Int &i) {
    os << i.val;
    return os;
}

using namespace std;


template<typename K1, typename K2, typename V>
void cmp(BPT<K1, V> &bpt, multimap<K2, V> &mp) {
    assert(bpt.size == mp.size());
    for (auto item: mp) {
        if (mp.lower_bound(item.first)->second != item.second) {
            continue;
        }
        vector<int> inMap;
        for (auto i0 = mp.equal_range(item.first).first; i0 != mp.equal_range(item.first).second; ++i0) {
            inMap.push_back(i0->second);
        }
        sort(inMap.begin(), inMap.end());
        sjtu::vector<int> inBPT;
        int key = item.first;
        inBPT = bpt.find3(Int(key));
        assert(inMap.size() == inBPT.size());
        for (int j = 0; j < inMap.size(); ++j) {
            assert(inMap[j] == inBPT[j]);
        }
//            if(bpt.find2(Int(item.first))!=item.second){
//                cout<<item.first<<' '<<item.second<<' '<<bpt.find2(Int(item.first))<<endl;
//                cout<<"error in insert"<<endl;
//                assert(0);
//                return 0;
//            }
    }
}

int main() {
    clearFile();
    BPT<Int, int> bpt;
    multimap<int, int> mp;
    int n = 2e2;
    int m = 2e1;
    int srandd= time(nullptr);
//    srandd=1715073415;
//1715073415
    srand(srandd);
    cout<<"srand:"<<srandd<<endl;
//    1715071974
//1715072915
    for (int i = 0; i < m * n; ++i) {
        if (i%100==0)cout<<i<<endl;
//        cout << i << endl;
        int k = rand() % n;
        int v = rand() % n;
        auto it = mp.equal_range(k);
        bool flag = false;
        for (auto i0 = it.first; i0 != it.second; ++i0) {
            if (i0->second == v) {
                flag = true;
                break;
            }
        }
        if (flag)continue;
        bpt.insert(Int(k, v), v);
        mp.insert({k, v});
//        continue;
//        if (i<3187)continue;
//        bpt.print();
//        cout<<"k:"<<k<<endl;
//        cout<<"v:"<<v<<endl;
//        if (i<4203)continue;
//        cout<<"k:"<<k<<endl;
//        continue;
        bpt.check();
        cmp(bpt, mp);
    }


    for (int i = 0; i < m * n; ++i) {//5651
        if (i%100==0)cout<<i<<endl;
//        cout << i << endl;
        int k = rand() % n;
        int v;
        auto it = mp.equal_range(k);
        decltype(it.first) it2;
        bool flag = false;
        for (auto i0 = it.first; i0 != it.second; ++i0) {
            if (rand() % 3) {
                flag = true;
                v = i0->second;
                it2 = i0;
                break;
            }
        }
        if (!flag)continue;
        bpt.delete_(Int(k, v), v);
        mp.erase(it2);
//        if (i<4)continue;
//        cout<<"k:"<<k<<endl;
//        cout<<"v:"<<v<<endl;
        assert(bpt.size == mp.size());
//        if (i < 3547)continue;
//        cout << "k:" << k << endl;
//        cout << "v:" << v << endl;
//        cout<<"k:"<<k<<endl;
        bpt.check();
        cmp(bpt, mp);
//        break;
    }
    //如果mp非空，随机删除一个元素
    cout << "delete all...  size:" <<mp.size()<< endl;
    if (!mp.empty()) {
        auto it = mp.begin();
//        it+=rand()%mp.size();
        for (int i = 0; i < rand() % mp.size(); ++i) {
            ++it;
        }
        int k = it->first;
        int v = it->second;
        bpt.delete_(Int(k, v), v);
        mp.erase(it);
        bpt.check();
        cmp(bpt, mp);
    }
    return 0;
}

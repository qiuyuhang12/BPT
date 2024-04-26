#include "BPT.hpp"
#include <cstring>
#include <iostream>
#include <map>

struct Int {
    int val;
    bool isMin;

    Int() : val(0), isMin(false) {}

    explicit Int(int val) : val(val), isMin(false) {}

    Int(int val, bool isMin) : val(val), isMin(isMin) {}

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
};
std::ostream &operator<<(std::ostream &os, const Int &i) {
    os << i.val;
    return os;
}

using namespace std;
int main() {
    clearFile();
    BPT<Int, int> bpt;
    map<int, int> mp;
    for (int i = 0; i < 2e4; ++i) {
        cout<<i<<endl;
//        if (rand()%3)continue;
        int k=rand()%10000;
        if (mp.count(k))continue;
        bpt.insert(Int(k), k);
        mp[k] = k;
//        if (i<4203)continue;
//        cout<<"k:"<<k<<endl;
        bpt.check();
        assert(bpt.size==mp.size());
        for(auto item:mp){
            if(bpt.find2(Int(item.first))!=item.second){
                cout<<item.first<<' '<<item.second<<' '<<bpt.find2(Int(item.first))<<endl;
                cout<<"error in insert"<<endl;
                assert(0);
                return 0;
            }
        }
    }
    for (int i = 0; i < 2e4; ++i) {//5651
        cout<<i<<endl;
        int k=rand()%10000;
        if (mp.count(k)==0)continue;
        bpt.delete_(Int(k), k);
        mp.erase(k);
        assert(bpt.size==mp.size());
//        if (i<5650)continue;

//        cout<<"k:"<<k<<endl;
        bpt.check();
//        continue;
        for(auto item:mp){
            if(bpt.find2(Int(item.first))!=item.second){
//                bpt.find(Int(k));
                std::cout<<"error in delete"<<endl;
                cout<<item.first<<' '<<item.second<<' '<<bpt.find2(Int(item.first))<<endl;
                assert(0);
            }
        }
//        break;
    }
    return 0;
}

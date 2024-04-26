#include "BPT.hpp"
#include <cstring>
#include <iostream>
#include <map>
#include <algorithm>
#include <ctime>
#include <cassert>
#include <vector>
struct Int {
    int val;
    bool isMin;
    int realVal;

    Int() : val(0), isMin(false) {}

    Int(int _val, bool _isMin, int _realVal) : val(_val), isMin(_isMin), realVal(_realVal) {}

    explicit Int(int _val) : val(_val), isMin(false) {}

    Int(int _val, int _realVal) : val(_val), isMin(false), realVal(_realVal) {}

    Int(int val, bool isMin) : val(val), isMin(isMin) {}
    bool operator<(const Int &other) const {
        return val < other.val;
    }
    Int &operator=(const Int &other) {
//        if (val==other.val)return *this;
        val = other.val;
        isMin = other.isMin;
        realVal = other.realVal;
        return *this;
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
//struct Int {
//    int val;
//    bool isMin;
//
//    Int() : val(0), isMin(false) {}
//
//    explicit Int(int val) : val(val), isMin(false) {}
//
//    Int(int val, bool isMin) : val(val), isMin(isMin) {}
//
//    bool operator<(const Int &other) const {
//        return val < other.val;
//    }
//
//    bool operator==(const Int &other) const {
//        return val == other.val;
//    }
//
//    bool operator>(const Int &other) const {
//        return val > other.val;
//    }
//
//    bool operator<=(const Int &other) const {
//        return val <= other.val;
//    }
//
//    bool operator>=(const Int &other) const {
//        return val >= other.val;
//    }
//
//    bool operator!=(const Int &other) const {
//        return val != other.val;
//    }
//};


std::ostream &operator<<(std::ostream &os, const Int &i) {
    os << i.val;
    return os;
}

//std::ostream &operator<<(std::ostream &os)const{
//    os<<val;
//    return os;
//}
void test1() {
    BPT<Int, int> tree;
    std::cout << "insert1..." << std::endl;
    for (int i = 0; i < 10; i++) {
//        std::cout<<i<<std::endl;
        tree.insert(Int(i), i);
        tree.insert(Int(i), i + 1);
    }
    std::cout << "find1..." << std::endl;
    for (int i = 0; i < 10; i++) {
        std::cout << i << std::endl;
        tree.find(Int(i));
    }
    std::cout << "delete1..." << std::endl;
    for (int i = 0; i < 10; i++) {
        std::cout << i << std::endl;
        tree.delete_(Int(i), i);
    }
    std::cout << "delete2..." << std::endl;
    for (int i = 0; i < 10; i++) {
        std::cout << i << std::endl;
        tree.delete_(Int(i), i);
    }
    std::cout << "find2..." << std::endl;
    for (int i = 0; i < 10; i++) {
        std::cout << i << std::endl;
        tree.find(Int(i));
    }
    std::cout << "insert2..." << std::endl;
    for (int i = 0; i < 10; i++) {
//        std::cout<<i<<std::endl;
        tree.insert(Int(i), i + 2);
        tree.insert(Int(i), i + 3);
    }
    std::cout << "find3..." << std::endl;
    for (int i = 0; i < 10; i++) {
        std::cout << i << std::endl;
        tree.find(Int(i));
    }
    std::cout << "delete3..." << std::endl;
    for (int i = 0; i < 10; i++) {
        std::cout << i << std::endl;
        tree.delete_(Int(i), i + 2);
    }
    std::cout << "find4..." << std::endl;
    for (int i = 0; i < 10; i++) {
        std::cout << i << std::endl;
        tree.find(Int(i));
    }
}

void test2() {
    BPT<Int, int> tree;
    std::cout << "insert1..." << std::endl;
    for (int i = 0; i < 40; i++) {
        std::cout << i << std::endl;
        tree.insert(Int(i), i);
        tree.insert(Int(i), i + 1);
    }
    std::cout << "find1..." << std::endl;
    for (int i = 0; i < 40; i++) {
        std::cout << i << std::endl;
        tree.find(Int(i));
    }
    std::cout << "delete1..." << std::endl;
    for (int i = 0; i < 40; i++) {
        std::cout << i << std::endl;
        tree.delete_(Int(i), i);
    }
    std::cout << "find2..." << std::endl;
    for (int i = 0; i < 40; i++) {
        std::cout << i << std::endl;
        tree.find(Int(i));
    }
    std::cout << "delete2..." << std::endl;
    for (int i = 0; i < 40; i++) {
        std::cout << i << std::endl;
        tree.delete_(Int(i), i + 1);
    }
    std::cout << "find3..." << std::endl;
    for (int i = 0; i < 40; i++) {
        std::cout << i << std::endl;
        tree.find(Int(i));
    }
}

void print_root(BPT<Int, int> &tree) {
    BPT<Int, int>::node _node;
    tree.readNode(tree.root, _node);
    _node.print();
}

void print_i(int i, BPT<Int, int> &tree) {
    BPT<Int, int>::node _node;
    BPT<Int, int>::block _block;
    tree.readNode(tree.root, _node);
    tree.readBlock(_node.child[i], _block);
    _block.print();
}

void print(BPT<Int, int> &tree) {
    BPT<Int, int>::node _node;
    BPT<Int, int>::block _block;
    tree.readNode(tree.root, _node);
    for (int i = 0; i < 100; ++i) {
        std::cout << '-';
    }
    std::cout << std::endl;
    for (int j = 0; j <= _node.size; ++j) {
//    for (int j = 0; j <=2; ++j) {
        tree.readBlock(_node.child[j], _block);
        _block.print();
    }
}

void test3() {
    BPT<Int, int> tree;
    int randss = time(nullptr);//1713792523
//    1713675272
    srand(randss);//1713617739
    srand(1713792523);
//    srand(1713616717);
//    srand(1713617739);
    std::cout << "srand" << randss << std::endl;
    std::cout << "insert1..." << std::endl;
    for (int i = 0; i < 4000; i++) {
//        std::cout<<i<<std::endl;
        int k = rand() % 8931;
        if (1)tree.insert(Int(k,k), k);
        if (1)tree.insert(Int(k,k+1), k + 1);
//        tree.print();
//        tree.print();
//        continue;
        tree.check();

//        print(tree);
    }
//    print_root(tree);
//    print(tree);
//    std::cout<<"find1..."<<std::endl;
//    for(int i=0;i<40;i++){
//        std::cout<<i<<std::endl;
//        tree.find(Int(i));
//    }
    std::cout << "delete1..." << std::endl;
    for (int i = 0; i < 4000; i++) {
//        for (int j = 0; j < 100; ++j) {
//            std::cout<<'-';
//        }
//        std::cout<<std::endl;
//        std::cout<<i<<std::endl;
//        print_root(t)
        int k = rand() % 7721;
        if (1)tree.delete_(Int(k,k), k);
//        tree.check();
        if (1)tree.delete_(Int(k,k+1), k + 1);
//        if (i<521)continue;
        tree.check();
//        tree.print();
//        if (rand()%2)tree.delete_(Int(i),i);
//        if (rand()%6)tree.delete_(Int(i),i+1);
//        print_root(tree);
//        print(tree);
    }
}

void test4() {
//    clearFile();
//    test3();
//    test3();
//    BPT<Int,int> tree;
//    std::cout<<"insert1..."<<std::endl;
//    for(int i=0;i<50;i++){
//        for (int j = 0; j < 100; ++j) {
//            std::cout<<'-';
//        }
//        std::cout<<i<<std::endl;
//        tree.insert(Int(i),i);
////        tree.print();
//        //        tree.insert(Int(i),i+1);
//    }
//    std::cout<<"delete1..."<<std::endl;
//    for(int i=0;i<50;i++){
//        for (int j = 0; j < 100; ++j) {
//            std::cout<<'-';
//        }
//        std::cout<<i<<std::endl;
//        tree.delete_(Int(i),i);
//        tree.print();
//        //        tree.delete_(Int(i),i+1);
//    }
////    print(tree);
}

struct String {
    char val[65];
    bool isMin;
    int realVal;

    String() : val(""), isMin(false) {}

    explicit String(std::string val) : isMin(false) {
        strcpy(this->val, val.c_str());
    }

    String(std::string val, bool isMin) : isMin(isMin) {
        strcpy(this->val, val.c_str());
    }

    String(std::string val, int realVal) : isMin(false), realVal(realVal) {
        strcpy(this->val, val.c_str());
    }

    String(std::string val, bool isMin, int realVal) : isMin(isMin), realVal(realVal) {
        strcpy(this->val, val.c_str());
    }

    String &operator=(const String &other) {
        if (val==other.val)return *this;
        strcpy(val, other.val);
        isMin = other.isMin;
        realVal = other.realVal;
        return *this;
    }

    bool operator<(const String &other) const {
        return strcmp(val, other.val) < 0;
    }

    bool operator==(const String &other) const {
        return strcmp(val, other.val) == 0;
    }

    bool operator>(const String &other) const {
        return strcmp(val, other.val) > 0;
    }

    bool operator<=(const String &other) const {
        return strcmp(val, other.val) <= 0;
    }

    bool operator>=(const String &other) const {
        return strcmp(val, other.val) >= 0;
    }

    bool operator!=(const String &other) const {
        return strcmp(val, other.val) != 0;
    }
};
std::ostream &operator<<(std::ostream &os, const String &i) {
    os << i.val;
    return os;
}
void submit(){
//    clearFile();
    BPT<String, int> tree;
    int n = 0;
    std::cin >> n;
    for (int i = 0; i < n; ++i) {
        std::string order;
        std::cin >> order;
        if (order == "insert") {
            std::string key;
            int val;
            std::cin >> key >> val;
            tree.insert(String(key,val), val);
        }
        if (order == "delete") {
            std::string key;
            int val;
            std::cin >> key >> val;
            tree.delete_(String(key,val), val);
        }
        if (order == "find") {
            std::string key;
            std::cin >> key;
            tree.find(String(key));
        }
//        tree.print();
    }
}
template<typename K,typename V,typename K2>
void checkbptmap(BPT<K,V> &tree,std::map<K2,V> &map){
    assert(tree.size==map.size());
    for (auto item:map) {
        assert(tree.find2(K(item.first)) == item.second);
    }
//    for (auto item:map) {
//        assert(bpt.find2(item.first)==item.second);
//    }
}
void test5(){
    clearFile();
    BPT<Int, int> tree;
    std::map<int, int> map;
    int n = time(nullptr);
    srand(1713680945);
//    srand(n);
    std::cout << "srand" << n << std::endl;
    while (1) {
        static int cnt = 0;
        std::cout << "cnt:" << ++cnt << std::endl;
        if (cnt == 1e3)break;
        std::cout << "insert1..." << std::endl;
        for (int i = 0; i < 2000; ++i) {
            int k = rand() % 3000;
            int key=k;
//            std::string key = std::to_string(k);
            if (map.find(key) != map.end()) {
//            i--;
                continue;
            }
            std::cout << i << std::endl;
            int val = rand() % 1000;
            if (cnt==2&&i==113) {
                tree.print_block(5184);
                std::cout << "hhh" << std::endl;
            }
            tree.insert(Int(key), val);
            map[key] = val;
//            checkbptmap(tree,map);
            if (cnt==2&&i==112){
                tree.print();
            }
            if (cnt==2&&i==113){
                tree.find(Int(key));
                tree.print();
                checkbptmap(tree,map);
            }
        }
        std::cout << "find1..." << std::endl;
        for (int i = 0; i < 2000; ++i) {
            int k = rand() % 3000;
//            std::cout << i << std::endl;
//
////            std::string key = std::to_string(k);
////            if (map.count(key)) {
////                assert(tree.find2(String(key)) == map[key]);
//////            tree.delete_(String(key), map[key]);
//////            map.erase(key);
////            } else {
////                assert(tree.find2(String(key)) == 0);
////            }
        }
        std::cout << "delete1..." << std::endl;
        for (int i = 0; i < 2000; ++i) {
            std::cout << i << std::endl;
            int k = rand() % 3000;
            int key=k;
//            std::string key = std::to_string(k);
            if (map.count(key)) {
                tree.delete_(Int(key), map[key]);
                map.erase(key);
            }
//            checkbptmap(tree,map);
        }
//        std::cout << "find2..." << std::endl;
        for (int i = 0; i < 2000; ++i) {
//            std::cout << i << std::endl;
//            assert(tree.size==map.size());
            int k = rand() % 3000;
//            for (auto item:map) {
//                assert(tree.find2(String(item.first)) == item.second);
//            }
//            std::string key = std::to_string(k);
//            if (map.count(key)) {
//                static int cnt2 = 0;
//                ++cnt2;
//                if (cnt==2&&cnt2==855){
//                    std::cout<<"hhh"<<std::endl;
//                }
//                assert(tree.find2(String(key)) == map[key]);
//            } else {
//                assert(tree.find2(String(key)) == 0);
//            }
        }
    }
}
int main() {
    clearFile();
    std::fstream file;
    file.open("test.txt", std::ios::out);
    file.close();
    file.open("test.txt", std::ios::out|std::ios::in|std::ios::binary);
    ll a= time(nullptr);
    int pp;
    int pq;
    char bb[2000];
    for (int i = 0; i < 2000; ++i) {
        bb[i]=rand()%256;
    }
    for (ll i = 0; i < 1e9; ++i) {
        pp%=pq;
    }
    ll b= time(nullptr);
    std::cout<<b-a<<std::endl;
//    test3();
//    BPT<Int, int> tree;
//    for (int i = 0; i < 100; ++i) {
//        for (int j = 0; j < 200; ++j) {
//            tree.insert(Int(i), j);
//        }
//    }
//    for (int i = 0; i < 100; ++i) {
//        std::cout << i << std::endl;
//        tree.find(Int(i));
//        for (int j = 0; j < 153; ++j) {
//            tree.delete_(Int(i), j);
//        }r
////        if (i==3||i==4||i==5)
////        tree.print();
//    }
//    submit();
//    clearFile();
    return 0;
}


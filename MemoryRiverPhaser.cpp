#include "BPT.hpp"
#include <cstring>
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
    }
}
int main() {
//    clearFile();
    submit();
    return 0;
}


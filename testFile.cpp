//
// Created by qiuyuhang on 24-4-26.
//
#include <fstream>
#include <iostream>
#include <cassert>
#include <cstring>
#include <map>
#include <algorithm>
int main() {
    remove("testFile.cpp");
    std::fstream f1("testFile.cpp",std::ios::out|std::ios::binary);
f1.close();
    f1.open("testFile.cpp", std::ios::in|std::ios::out|std::ios::binary);
    std::fstream f2("testFile.cpp", std::ios::in|std::ios::out|std::ios::binary);
    for (int i = 0; i < 13; ++i) {
        f1.seekp(i*16);
        f1.write((char*)&i, sizeof(i));
        f2.seekp(i*16+8);
        f2.write((char*)&i, sizeof(i));
        f2.flush();
    }
    f1.close();
    f2.close();
    return 0;
}
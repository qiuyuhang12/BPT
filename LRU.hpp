//
// Created by qiuyuhang on 24-4-26.
//

#ifndef BPT_LRU_HPP
#define BPT_LRU_HPP

#include <cstddef>
#include <iostream>
#include <cassert>
#include <fstream>
#include <string>

template<class T>
concept HasIsMin = requires(T k) {
    k.isMin;
    k.val;
    k.realVal;
    std::is_same_v<decltype(k.isMin), bool>;
};

template<HasIsMin Key,typename Value>
class BPT;

template<typename Hash, typename Key>
concept Hashable = requires(Key key){
    { Hash()(key) }->std::same_as<size_t>;
};
static const int TableCapacity = 1e4+7;//!2
static const int LinkCapacity = 4e3;//!2
template<typename Key, typename Block, Hashable<Key> Hash>
class LRU {
private:
public:
//    static const int TableCapacity = 199;
//    static const int TableCapacity = 1e6+3;//!1
//    static const int TableCapacity = 9;
//    static const int LinkCapacity = 1999;//!1
//    static const int TableCapacity = 9;
//    static const int LinkCapacity = 7;
    size_t size = 0;
    std::fstream file;
    class HashNode;
    struct DataNode {
        Key key;
        Block* block;
        HashNode *hashNode = nullptr;
        DataNode *prev = nullptr;
        DataNode *next = nullptr;

        DataNode() = default;

        DataNode(Key key, Block *block, DataNode *next = nullptr, DataNode *prev = nullptr) : key(key), block(block),
                                                                                              next(next), prev(prev) {}

                                                                                              ~DataNode(){
                                                                                                  delete block;
                                                                                              }
    };

    class HashNode {
    public:
        Key key;
        DataNode *data = nullptr;
        HashNode *next = nullptr;
        HashNode *prev = nullptr;

        HashNode() = default;

        HashNode(Key key, DataNode *data, HashNode *next = nullptr, HashNode *prev = nullptr) : key(key), data(data),
                                                                                                next(next),
                                                                                                prev(prev) {}
    };

    HashNode *hashTable[TableCapacity] = {nullptr};//todo初始化为头节点
    DataNode *head = nullptr;
    DataNode *tail = nullptr;

    void writeToFile(long long pos,Block *block){
        file.seekp(pos,std::ios::beg);
        file.write(reinterpret_cast<char *>(block),sizeof(Block));
        file.flush();
    }
    void insertToTable(size_t pos,HashNode *hashNode){
        hashNode->next = hashTable[pos]->next;
        hashNode->prev = hashTable[pos];
        hashTable[pos]->next->prev = hashNode;
        hashTable[pos]->next = hashNode;
    }
    void insertToLink(DataNode *dataNode){//插入到头节点后
        dataNode->next = head->next;
        dataNode->prev = head;
        head->next->prev = dataNode;
        head->next = dataNode;
    }
    void separateFromTable(HashNode *hashNode){
        assert(hashNode->prev != nullptr && hashNode->next != nullptr);
        hashNode->prev->next = hashNode->next;
        hashNode->next->prev = hashNode->prev;
    }
    void separateFromLink(DataNode *dataNode){
        assert(dataNode->prev != nullptr && dataNode->next != nullptr);
        dataNode->prev->next = dataNode->next;
        dataNode->next->prev = dataNode->prev;
    }
    void moveToHead(DataNode *dataNode){
        separateFromLink(dataNode);
        insertToLink(dataNode);
    }
    void removeTail(){
        assert(size > 0);
        --size;
        DataNode *p = tail->prev;
        separateFromLink(p);
        HashNode *q = p->hashNode;
//        size_t pos = Hash()(p->key);
//        HashNode *q = hashTable[pos]->next;
//        while (q->key != p->key) {
//            q = q->next;
//            assert(q != nullptr);
//            assert(q->next != nullptr);//q是尾节点
//        }
        separateFromTable(q);
        writeToFile(q->key,p->block);
        delete p;
        delete q;
    }
    DataNode* findAndMoveToHead(Key key){
        size_t pos = Hash()(key);
        HashNode *p = hashTable[pos]->next;
        while (p->key != key&&p->data!= nullptr) {
            p = p->next;
            assert(p != nullptr);
        }
        if (p->data == nullptr) {
            return nullptr;
        }
        moveToHead(p->data);
        return p->data;
    }
public:
    LRU(){
        head = new DataNode();
        tail = new DataNode();
        head->next = tail;
        tail->prev = head;
        for (int i = 0; i < TableCapacity; ++i) {
            hashTable[i] = new HashNode();
            hashTable[i]->next =new HashNode();
            hashTable[i]->next->prev = hashTable[i];
        }
    }

    ~LRU(){
        for (int i = 0; i < TableCapacity; ++i) {
            HashNode *p = hashTable[i];
            while (p->next != nullptr) {
                HashNode *q = p->next;
                delete p;
                p = q;
            }
            delete p;
        }
        while (head!=tail){
            DataNode *p = head->next;
            delete head;
            head = p;
        }
        delete head;
        head = nullptr;
        tail = nullptr;
        file.close();
    }

    void insert(Key key, Block *block) {
        DataNode *p=findAndMoveToHead(key);
        if (p != nullptr) {
            if (p->block==block)return;
            delete p->block;
            p->block = block;
            return;
        }
        size_t pos = Hash()(key);
        DataNode *dataNode = new DataNode(key, block);
        HashNode *hashNode = new HashNode(key, dataNode);
        dataNode->hashNode = hashNode;
        insertToTable(pos,hashNode);
        insertToLink(dataNode);
        size++;
        if (size > LinkCapacity) {
            removeTail();
        }
    }

    bool get(Key key, Block *&block) {
        DataNode *p = findAndMoveToHead(key);
        if (p == nullptr) {
            return false;
        }
        block = p->block;
        return true;
    }
    void enableFile(const std::string &filename){
        file.open(filename,std::ios::in|std::ios::out|std::ios::binary);
        if (!file.is_open()){
            assert(0);
        }
    }
    void disableFile(){
        DataNode *dataNode=head->next;
        while (dataNode!=tail){
            writeToFile(dataNode->key,dataNode->block);
            dataNode = dataNode->next;
        }
        file.close();
    }
};

#endif //BPT_LRU_HPP

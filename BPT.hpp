#ifndef BPT_BPT_HPP
#define BPT_BPT_HPP

#include <iostream>
#include <string>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <filesystem>
#include <cassert>
//#include <vector>//!
//#include <map>//!
#include "STLSrc/map.hpp"
#include "STLSrc/vector.hpp"
#include "LRU.hpp"
#define ll long long



template<class T>
concept HasIsMin = requires(T k) {
    k.isMin;
    k.val;
    k.realVal;
    std::is_same_v<decltype(k.isMin), bool>;
};

//template<typename hasher>
//static constexpr bool isMyHasher = requires() {
//    hasher::isMine;
//};

struct hash{
    size_t operator()(ll& x)const{
        return x%TableCapacity;
    }
};

template<HasIsMin Key,typename Value>
class BPT{
private:
public:
    std::fstream bptNodes;
    std::fstream bptBlocks;
    ll root=1e3;
    ll nodeNum=0;
    ll blockNum=0;
    ll NodesFileEnd=1e3;
    ll BlocksFileEnd=0;
    ll size=0;
    sjtu::map<ll,ll> nodeParent;
    sjtu::map<ll,ll> blockParent;
    ll NParent(ll pos){
        assert(nodeParent.count(pos));
        return nodeParent[pos];
    }
    ll BParent(ll pos){
        assert(blockParent.count(pos));
        return blockParent[pos];
    }
    static const ll cut=1e3;
    static const int M=49;
    static const int L=50+1;
//    static const int M=20+1;
//    static const int L=20+1;
    struct node{
        bool isRoot=false;
        bool isLeaf=false;
        ll size=0;//不含0处
        ll parent=0;
        Key key[M]={};
        ll child[M]={0};
        node()=default;
        node(bool _isRoot,bool _isLeaf,ll _size,ll _parent):isRoot(_isRoot),isLeaf(_isLeaf),size(_size),parent(_parent){
            key[0].isMin=true;
        }
        void print(){
            std::cout<<"isRoot:"<<isRoot<<' '<<"isLeaf:"<<isLeaf<<' '<<"size:"<<size<<' '<<"parent:"<<parent<<std::endl;
            for (int i = 0; i <= size; ++i) {
                std::cout<<i<<":("<<key[i]<<' ';
                std::cout<<std::boolalpha<<key[i].isMin<<") ";
            }
            std::cout<<std::endl;
            for (int i = 0; i <= size; ++i) {
                std::cout<<i<<':'<<child[i]<<' ';
            }
            std::cout<<std::endl;

        }
    };
    struct Pair{
        Key key=Key();
        Value value=Value();
        Pair()=default;
        Pair(Key _key,Value _value):key(_key),value(_value){}
    };
    struct block{
        ll size=0;//含0处
        ll last=-1;
        ll next=-1;
        ll parent=0;
        Pair data[L]={};
        block()=default;
        block(ll _size,ll _last,ll _next,ll _parent):size(_size),last(_last),next(_next),parent(_parent){}
        void print(){
            std::cout<<"size:"<<size<<' '<<"last:"<<last<<' '<<"next:"<<next<<' '<<"parent:"<<parent<<std::endl;
            for (int i = 0; i < size; ++i) {
                std::cout<<data[i].key<<' '<<data[i].value<<std::endl;
            }
        }
    };
    LRU<ll,node,hash> lruNode;
    LRU<ll,block,hash> lruBlock;
    enum Type{
        Last,Next
    };
    bool compK(const Key& a,const Key& b)const{
        if (a.isMin&&b.isMin)assert(false);
//        if (a.isMin&&b.isMin)return false;
        if (a.isMin||b.isMin)return a.isMin;
//        return a.val<b.val;
        return a<b;
    }
    bool equalK(const Key& a,const Key& b)const{
//        if (a.isMin&&b.isMin)assert(false);
        if (a.isMin&&b.isMin)return true;
        if (a.isMin||b.isMin)return false;
//        return a.val==b.val;
        return a==b;
    }
    bool compKey(const Key& a,const Key& b)const{
        if (a.isMin&&b.isMin)assert(false);
//        if (a.isMin&&b.isMin)return false;
        if (a.isMin||b.isMin)return a.isMin;
//        return a<b;
        if (a!=b)
        return a<b;
        else return a.realVal<b.realVal;
    }
    bool equalKey(const Key& a,const Key& b)const{
//        if (a.isMin&&b.isMin)assert(false);
        if (a.isMin&&b.isMin)return true;
        if (a.isMin||b.isMin)return false;
//        return a==b;
        return a==b&&a.realVal==b.realVal;
    }
    bool equalKV(const Pair& a,const Pair& b)const{
        if (a.key.isMin&&b.key.isMin)assert(false);
        if (a.key.isMin||b.key.isMin)return false;
        return equalK(a.key,b.key)&&a.value==b.value;
    }
    bool compKV(const Pair& a,const Pair& b)const{
        if (a.key.isMin&&b.key.isMin)assert(false);
        if (a.key.isMin||b.key.isMin)return a.key.isMin;
        if (!equalK(a.key,b.key))return compK(a.key,b.key);
        return a.value<b.value;
    }

    void readNode(ll pos,node& _node){
        assert(pos>=cut&&pos<NodesFileEnd);//1e3
        if (lruNode.get(pos, _node))return;
        bptNodes.seekg(pos, std::ios::beg);
        bptNodes.read(reinterpret_cast<char*>(&_node),sizeof(node));
    }
    void writeNodeToEnd(node& _node){//没有空间回收//没有++nodeNum
        assert(NodesFileEnd>cut-1);
        lruNode.insert(NodesFileEnd,_node);
//        bptNodes.seekp(NodesFileEnd, std::ios::beg);
        NodesFileEnd+=sizeof(node);
//        bptNodes.write(reinterpret_cast<char*>( &_node),sizeof(node));
        ++nodeNum;
    }
    void writeNode(ll pos,node& _node){//没有空间回收//没有++nodeNum
        assert(NodesFileEnd>cut-1);
        lruNode.insert(pos,_node);
//        bptNodes.seekp(pos, std::ios::beg);
//        bptNodes.write(reinterpret_cast<char*>( &_node),sizeof(node));
    }
    void readBlock(ll pos,block& _block){
        assert(pos>-1&&pos<BlocksFileEnd);
        if (lruBlock.get(pos, _block))return;
        bptBlocks.seekg(pos, std::ios::beg);
        bptBlocks.read(reinterpret_cast<char*>(&_block),sizeof(block));
    }
    void writeBlockToEnd(block& _block){//没有空间回收//没有++blockNum
        assert(BlocksFileEnd>-1);
        lruBlock.insert(BlocksFileEnd,_block);
//        bptBlocks.seekp(BlocksFileEnd, std::ios::beg);
        BlocksFileEnd+=sizeof(block);
//        bptBlocks.write(reinterpret_cast<char*>(&_block),sizeof(block));
        ++blockNum;
    }
    void writeBlock(ll pos,block& _block){//没有空间回收//没有++blockNum
        assert(BlocksFileEnd>-1);
        lruBlock.insert(pos,_block);
//        bptBlocks.seekp(pos, std::ios::beg);
//        bptBlocks.write(reinterpret_cast<char*>(&_block),sizeof(block));
    }

    bool ShouldSplitNode(node& _node){
        assert(_node.isRoot||nodeNum==1||(_node.size<M-1&&_node.size>M/3));
        return _node.size==M-2;
    }
    bool ShouldSplitBlock(block& _block){
        assert(blockNum==1||(_block.size<L-1&&_block.size>L/3));
        return _block.size==L-2;
    }
    bool ShouldMergeNode(node& _node){
        assert(!_node.isRoot);
        assert(nodeNum==1||(_node.size<M-1&&_node.size>=M/3));
        return nodeNum!=1&&_node.size==M/3;
    }
    bool ShouldMergeRoot(node& _node){
        assert(nodeNum==1||_node.size<M-1);
        assert(_node.isRoot);
        assert(_node.size>=0);
        if (_node.isLeaf){
            assert(nodeNum==1);
            return false;
        }
        assert(nodeNum!=1);
        return _node.size==0;
    }
    bool ShouldMergeBlock(block& _block){
        assert(blockNum==1||(_block.size<L-1&&_block.size>=L/3));
        return blockNum!=1&&_block.size==L/3;
    }

    void adjustParentBlock(ll i,block &_block){
        ll son=i;
        ll parent=BParent(i);
        node p;
        while (true){
            readNode(parent,p);
            if (p.child[0]==son){
                if (p.isRoot)break;
                son=parent;
                parent=NParent(parent);
            }
            else{
                for (int k = 1; k <= p.size; ++k) {
                    if (son==p.child[k]){
                        p.key[k]=_block.data[0].key;
                        writeNode(parent,p);
                        return;
                    }
                }
                assert(0);
            }
        }
    }

    enum TType{
        first,last,normal
    };
    void splitBlock(ll pos){
        block _block;
        readBlock(pos,_block);
        block newBlock;
        newBlock.size=_block.size/2;
        _block.size-=newBlock.size;
        newBlock.next=_block.next;
        newBlock.last=pos;
        _block.next=BlocksFileEnd;
        writeBlock(pos,_block);
        block nextBlock;
        if (newBlock.next!=-1){
            readBlock(newBlock.next,nextBlock);
            nextBlock.last=_block.next;
            writeBlock(newBlock.next,nextBlock);
        }
        for (int i = 0; i < newBlock.size; ++i) {
            newBlock.data[i]=_block.data[_block.size+i];
        }
        writeBlockToEnd(newBlock);
        node parent;
        readNode(BParent(pos),parent);
        Key key=newBlock.data[0].key;
        bool flag=false;
        for (int i = 0; i <= parent.size; ++i) {
            if (pos==parent.child[i]){
                for (int j = parent.size+1; j > i+1; --j) {
                    parent.key[j]=parent.key[j-1];
                    parent.child[j]=parent.child[j-1];
                }
                parent.key[i+1]=key;
                parent.child[i+1]=_block.next;
                ++parent.size;
                writeNode(BParent(pos),parent);
                flag=true;
                break;
            }
        }
        assert(flag);
        if (ShouldSplitNode(parent)){
            splitNode(BParent(pos));
        }
    }

    void splitNode(ll pos){
        node _node;
        readNode(pos,_node);
        node newNode(false,_node.isLeaf,_node.size/2,_node.parent);
        _node.size-=newNode.size+1;
        for (int i = 0; i <= newNode.size; ++i) {//头是ismin,如果改成反之，则要改assert
            newNode.key[i]=_node.key[_node.size+i+1];
            newNode.child[i]=_node.child[_node.size+i+1];
        }
        newNode.key[0].isMin=true;
        if (!_node.isRoot) {
            writeNode(pos, _node);
            writeNodeToEnd(newNode);
            node parent;
            readNode(NParent(pos),parent);
            Key key = _node.key[_node.size+1];
            bool flag = false;
            for (int i = 0; i <= parent.size; ++i) {
                if (pos == parent.child[i]) {
                    for (int j = parent.size + 1; j > i + 1; --j) {
                        parent.key[j] = parent.key[j - 1];
                        parent.child[j] = parent.child[j - 1];
                    }
                    parent.key[i + 1] = key;
                    parent.child[i + 1] = NodesFileEnd - sizeof(node);
                    ++parent.size;
                    writeNode(NParent(pos),parent);
                    flag = true;
                    break;
                }
            }
            assert(flag);
            if (ShouldSplitNode(parent)) {
                splitNode(NParent(pos));
            }
        } else{
            _node.isRoot= false;
            newNode.isRoot= false;
            node newRoot(true,false,1,0);
            newRoot.key[1]=_node.key[_node.size+1];
            newRoot.child[0]=pos;
            newRoot.child[1]=NodesFileEnd;//因为没写入root呢
            writeNode(pos,_node);
            writeNodeToEnd(newNode);
            writeNodeToEnd(newRoot);
            root=NodesFileEnd-sizeof(node);
        }
    }

    TType TType__(ll pos,node &_node,node &parent){
        if (parent.child[0] == pos) {
            return first;
        } else if (parent.child[parent.size] == pos) {
            return last;
        } else {
            return normal;
        }
    }
    void mergeRoot(node &son, node &parent){
        assert(parent.isRoot);
        assert(parent.size==0);
        son.isRoot=true;
        writeNode(parent.child[0],son);
        root=parent.child[0];
        --nodeNum;
    }
    void mergeNode(ll pos){//主分支函数
        assert(pos!=root);
        node _node, lastNode, nextNode;
        node parent;
        readNode(pos, _node);
        readNode(NParent(pos),parent);
        ll nextP=0,lastP=0;
        if (borrowNode(pos, _node, lastNode, nextNode, parent,lastP,nextP)){//在里边读
            return;
        }
        int type=0;
        switch (TType__(pos, _node, parent)) {
            case first:
                _mergeNode(pos,nextP,_node,nextNode,parent);
                type=1;
                break;
            case last:
                _mergeNode(lastP,pos,lastNode,_node,parent);
                type=0;
                break;
            case normal:
                assert(_node.size==M/3);
                _mergeNode(pos,nextP,_node,nextNode,parent);
                type=1;
                break;
        }
        if (parent.isRoot) {
            if (ShouldMergeRoot(parent)) {
                if (type==1)mergeRoot(_node, parent);
                else mergeRoot(lastNode, parent);
            }
        } else {
            if (ShouldMergeNode(parent)) {
                mergeNode(NParent(pos));
            }
        }
    }//主分支函数
    bool borrowNode(ll pos,node &_node,node &lastNode,node &nextNode,node &parent,ll &lastP,ll &nextP) {//还没读,在这个函数读
        if (parent.child[0] == pos) {
            readNode(parent.child[1], nextNode);
            nodeParent[parent.child[1]]=NParent(pos);
            nextP=parent.child[1];
            if (_borrowNode(pos, _node, Next, parent, 0, lastNode, nextNode)) {
                return true;
            }
        } else if (parent.child[parent.size] == pos) {
            readNode(parent.child[parent.size - 1], lastNode);
            nodeParent[parent.child[parent.size-1]]=NParent(pos);
            lastP=parent.child[parent.size-1];
            if (_borrowNode(pos, _node, Last, parent, parent.size, lastNode, nextNode)) {
                return true;
            }
        } else{
            for (int i = 1; i < parent.size; ++i) {
                if (parent.child[i] == pos) {
                    readNode(parent.child[i - 1], lastNode);
                    readNode(parent.child[i + 1], nextNode);
                    nodeParent[parent.child[i+1]]=NParent(pos);
                    nodeParent[parent.child[i-1]]=NParent(pos);
                    lastP=parent.child[i-1];
                    nextP=parent.child[i+1];
                    if (_borrowNode(pos, _node, Last, parent, i, lastNode, nextNode)) {
                        return true;
                    }
                    if (_borrowNode(pos, _node, Next, parent, i, lastNode, nextNode)) {
                        return true;
                    }
                }
            }
        }
        return false;
    }//分支函数
    bool _borrowNode(ll pos,node &_node,Type t,node &parent,ll pInSonArray,node &lastNode,node &nextNode){
        if (t==Last){
            if (lastNode.size>M/3+1){
                for (int i = _node.size+1; i > 0; --i) {//注意是size+1
                    _node.child[i]=_node.child[i-1];
                    _node.key[i]=_node.key[i-1];
                }
                _node.key[1]=parent.key[pInSonArray];
                _node.child[0]=lastNode.child[lastNode.size];
                ++_node.size;
                parent.key[pInSonArray]=lastNode.key[lastNode.size];
                --lastNode.size;
                writeNode(parent.child[pInSonArray-1],lastNode);
                writeNode(pos,_node);
                writeNode(NParent(pos),parent);
                return true;
            }
            assert(lastNode.size==M/3+1);
        }else{
            if (nextNode.size>M/3+1){
                _node.key[_node.size+1]=parent.key[pInSonArray+1];
                _node.child[_node.size+1]=nextNode.child[0];
                ++_node.size;
                for (int i = 0; i < nextNode.size; ++i) {//注意是size+1
                    nextNode.child[i]=nextNode.child[i+1];
                    nextNode.key[i]=nextNode.key[i+1];
                }
                --nextNode.size;
                parent.key[pInSonArray+1]=nextNode.key[0];
                nextNode.key[0].isMin=true;
                writeNode(parent.child[pInSonArray+1],nextNode);
                writeNode(pos,_node);
                writeNode(NParent(pos),parent);
                return true;
            }
            assert(nextNode.size==M/3+1);
        }
        return false;
    }//真正借元素函数
    void _mergeNode(ll pos1,ll pos2,node&node1,node&node2,node&parent){//真正并块函数，出现pos2的文件空隙
        assert((node1.size==M/3&&node2.size==M/3+1)||(node1.size==M/3+1&&node2.size==M/3));
        assert(NParent(pos1)== NParent(pos2));
        for (int i = 0; i <= node2.size; ++i) {
            node1.key[node1.size+i+1]=node2.key[i];
            node1.child[node1.size+i+1]=node2.child[i];
        }
        bool flag=false;
        for (int i = 1; i <= parent.size; ++i) {
            if (parent.child[i]==pos2){
                node1.key[node1.size+1]=parent.key[i];
                for (int j = i; j < parent.size; ++j) {
                    parent.key[j]=parent.key[j+1];
                    parent.child[j]=parent.child[j+1];
                }
                --parent.size;
                flag=true;
                break;
            }
        }
        assert(flag);
        node1.size+=node2.size+1;
        writeNode(pos1,node1);
        writeNode(NParent(pos1),parent);
        --nodeNum;
    }//真正并块函数，出现pos2的文件空隙


    TType TType_(ll pos,block &_block,node &parent){
        if (parent.child[0] == pos) {
            return first;
        } else if (parent.child[parent.size] == pos) {
            return last;
        } else {
            return normal;
        }
    }
    void mergeBlock(ll pos){//主分支函数
        block _block, lastBlock, nextBlock;
        node parent;
        readBlock(pos, _block);
        readNode(BParent(pos), parent);
        if(_block.next!=-1){
            readBlock(_block.next, nextBlock);
            blockParent[_block.next]=BParent(pos);
        }
        if(_block.last!=-1){
            readBlock(_block.last, lastBlock);
            blockParent[_block.last]=BParent(pos);
        }
        if (borrowBlock(pos, _block, lastBlock, nextBlock, parent)){
            return;
        }
        switch (TType_(pos, _block, parent)) {
            case first:
                _mergeBlock(pos,_block.next,_block,nextBlock,parent);
                break;
            case last:
                _mergeBlock(_block.last,pos,lastBlock,_block,parent);
                break;
            case normal:
                assert(_block.last!=-1&&_block.next!=-1);
                assert(_block.size==L/3);
                _mergeBlock(pos,_block.next,_block,nextBlock,parent);
                break;
        }
        if (parent.isRoot) {
            return;
        }else {
            if (ShouldMergeNode(parent)) {
                mergeNode(BParent(pos));
            }
        }
    }
    bool borrowBlock(ll pos,block &_block,block &lastBlock,block &nextBlock,node &parent) {
        if (parent.child[0] == pos) {
            if (_borrowBlock(pos, _block, Next, parent, 0, lastBlock, nextBlock)) {
                return true;
            }
        } else if (parent.child[parent.size] == pos) {
            if (_borrowBlock(pos, _block, Last, parent, parent.size, lastBlock, nextBlock)) {
                return true;
            }
        } else{
            for (int i = 1; i < parent.size; ++i) {
                if (parent.child[i] == pos) {
                    if (_borrowBlock(pos, _block, Last, parent, i, lastBlock, nextBlock)) {
                        return true;
                    }
                    if (_borrowBlock(pos, _block, Next, parent, i, lastBlock, nextBlock)) {
                        return true;
                    }
                }
            }
        }
        return false;
    }//分支函数
    bool _borrowBlock(ll pos,block &_block,Type t,node &parent,ll pInSonArray,block &lastBlock,block &nextBlock){
        if (t==Last){
            if (lastBlock.size>L/3+1){
                for (int i = _block.size; i > 0; --i) {
                    _block.data[i]=_block.data[i-1];
                }
                _block.data[0]=lastBlock.data[lastBlock.size-1];
                ++_block.size;
                --lastBlock.size;
                writeBlock(_block.last,lastBlock);
                writeBlock(pos,_block);
                parent.key[pInSonArray]=_block.data[0].key;
                writeNode(BParent(pos),parent);
                return true;
            }
            assert(lastBlock.size==L/3+1);
        }else{
            if (nextBlock.size>L/3+1){
                _block.data[_block.size]=nextBlock.data[0];
                ++_block.size;
                for (int i = 0; i < nextBlock.size-1; ++i) {
                    nextBlock.data[i]=nextBlock.data[i+1];
                }
                --nextBlock.size;
                writeBlock(_block.next,nextBlock);
                writeBlock(pos,_block);
                parent.key[pInSonArray+1]=nextBlock.data[0].key;
                writeNode(BParent(pos),parent);
                return true;
            }
            assert(nextBlock.size==L/3+1);
        }
        return false;
    }//真正借元素函数
    void _mergeBlock(ll pos1,ll pos2,block&block1,block&block2,node&parent){//真正并块函数，出现pos2的文件空隙
        assert((block1.size==L/3&&block2.size==L/3+1)||(block1.size==L/3+1&&block2.size==L/3));
        assert(BParent(pos1)==BParent(pos2));
        assert(block1.next==pos2);
        assert(block2.last==pos1);
        if (block2.next!=-1){
            block blockAfter;
            readBlock(block2.next,blockAfter);
            blockAfter.last=pos1;
            writeBlock(block2.next,blockAfter);
        }
        for (int i = 0; i < block2.size; ++i) {
            block1.data[block1.size+i]=block2.data[i];
        }
        bool flag=false;
        for (int i = 1; i <= parent.size; ++i) {
            if (parent.child[i]==pos2){
                for (int j = i; j < parent.size; ++j) {
                    parent.key[j]=parent.key[j+1];
                    parent.child[j]=parent.child[j+1];
                }
                --parent.size;
                flag=true;
                break;
            }
        }
        assert(flag);
        block1.next=block2.next;
        block1.size+=block2.size;
        writeBlock(pos1,block1);
        writeNode(BParent(pos1),parent);
        --blockNum;
    }//真正并块函数，出现pos2的文件空隙

    sjtu::vector<ll> findKey(Key &key,const sjtu::vector<ll>& vec){//返回的是node/block的pos
        assert(!vec.empty());
        sjtu::vector<ll> res;
        node nd;
        for (ll i:vec){
            readNode(i,nd);
            for (int j = 0; j < nd.size; ++j) {
                if (!compK(key,nd.key[j])&&!compK(nd.key[j+1],key)){
                    res.push_back(nd.child[j]);
                    if (!nd.isLeaf)nodeParent[nd.child[j]]=i;
                    else blockParent[nd.child[j]]=i;
                }
            }
            if (!compK(key,nd.key[nd.size])){
                res.push_back(nd.child[nd.size]);
                if (!nd.isLeaf)nodeParent[nd.child[nd.size]]=i;
                else blockParent[nd.child[nd.size]]=i;
            }
        }
        if (nd.isLeaf){
            return res;
        }
        return findKey(key,res);
    }
    sjtu::vector<ll> findKeyOnlyOne(Key &key,const sjtu::vector<ll>& vec){//返回的是node/block的pos
        assert(!vec.empty());
        sjtu::vector<ll> res;
        node nd;
        for (ll i:vec){
            readNode(i,nd);
            for (int j = 0; j < nd.size; ++j) {
//                if (!compK(key,nd.key[j])&&!compK(nd.key[j+1],key)){
                if (!compKey(key,nd.key[j])&&!compKey(nd.key[j+1],key)){
                    res.push_back(nd.child[j]);
                    if (!nd.isLeaf)nodeParent[nd.child[j]]=i;
                    else blockParent[nd.child[j]]=i;
                }
            }
//            if (!compK(key,nd.key[nd.size])){
            if (!compKey(key,nd.key[nd.size])){
                res.push_back(nd.child[nd.size]);
                if (!nd.isLeaf)nodeParent[nd.child[nd.size]]=i;
                else blockParent[nd.child[nd.size]]=i;
            }
        }
        if (nd.isLeaf){
            return res;
        }
        return findKeyOnlyOne(key,res);
    }
    ll delKV(Key &key,Value &value){//删除成功返回block的pos，不成功返回-1,不处理合并块
        sjtu::vector<ll> rt;
        rt.push_back(root);
        sjtu::vector<ll> res=findKeyOnlyOne(key,rt);
        Pair p(key,value);
        for (ll i:res){
            block _block;
            readBlock(i,_block);
            if (compKV(p,_block.data[0])){
                assert(compKV(p,_block.data[_block.size-1]));
                break;
            }
            if (compKV(_block.data[_block.size-1],p)){
                assert(compKV(_block.data[0],p));
                continue;
            }
            for (int j = 0; j < _block.size; ++j) {
                if (equalKV(_block.data[j],p)){
                    for (int k = j; k < _block.size-1; ++k) {
                        _block.data[k]=_block.data[k+1];
                    }
                    --_block.size;
                    writeBlock(i,_block);
                    --size;
                    if (j!=0)return i;
                    adjustParentBlock(i,_block);
                    return i;
                }
            }
        }
        return -1;
    }
    ll insertKV(Key &key,Value &value){//插入成功返回block的pos，不成功返回-1,不处理分裂块
        sjtu::vector<ll> rt;
        rt.push_back(root);
        sjtu::vector<ll> res=findKeyOnlyOne(key,rt);
        assert(res.size()>=1);
        Pair p(key,value);
        block _block,nextBlock;
        readBlock(res[0],_block);
        ll i=res[0];
        for (int kk=1;kk<=res.size();++kk,_block=nextBlock){
            if (kk!=res.size()){
                if (res.size()==1)assert(0);
                i=res[kk];
                readBlock(i,nextBlock);
                if (compKV(nextBlock.data[nextBlock.size-1],p)){
                    assert(compKV(nextBlock.data[0],p));
                    continue;
                }
            }
            if (compKV(p,_block.data[0])){
                assert(compKV(p,_block.data[_block.size-1]));
                break;
            }
            for (int j = 0; j < _block.size; ++j) {//插到中间
                if (equalKV(_block.data[j],p))return -1;
                if (compKV(p,_block.data[j])){
                    assert(j!=0);
                    assert(compKV(_block.data[j-1],p));
                    for (int k = _block.size; k > j; --k) {
                        _block.data[k]=_block.data[k-1];
                    }
                    _block.data[j]=p;
                    ++_block.size;
                    writeBlock(res[kk-1],_block);
                    ++size;
                    return res[kk-1];
                }
            }
            _block.data[_block.size]=p;//插到最后
            ++_block.size;
            writeBlock(res[kk-1],_block);
            ++size;
            return res[kk-1];
        }
        return -1;
    }


    void print_child(ll pos){
        for (int i = 0; i < 30; ++i) {
            std::cout<<'-';
        }
        std::cout<<std::endl;
        node _node;
        readNode(pos,_node);
        std::cout<<"pos:"<<pos;
        _node.print();
        if (_node.isLeaf){
            block _block;
            for (int i = 0; i <= _node.size; ++i) {
                for (int j = 0; j < 30; ++j) {
                    std::cout<<'-';
                }
                std::cout<<std::endl;
                readBlock(_node.child[i],_block);
                _block.print();
            }
            return;
        }
        for (int i = 0; i <= _node.size; ++i) {
            print_child(_node.child[i]);
        }
    }
    void print_block(ll pos){
        block _block;
        readBlock(pos,_block);
        _block.print();
    }
    void print(){
        std::cout<<"PRINT START\n";
        std::cout<<"root:"<<root<<std::endl;
        std::cout<<"nodeNum:"<<nodeNum<<std::endl;
        std::cout<<"blockNum:"<<blockNum<<std::endl;
        std::cout<<"NodesFileEnd:"<<NodesFileEnd<<std::endl;
        std::cout<<"BlocksFileEnd:"<<BlocksFileEnd<<std::endl;
        std::cout<<"size:"<<size<<std::endl;
        print_child(root);
    }

    void check_block(ll pos,Key &key,Key &key2){
        block _block;
        readBlock(pos,_block);
        assert(equalK(_block.data[0].key,key));
        if (blockNum==1){
            assert(_block.size>0&&_block.size<L-1);
        } else{
            assert(_block.size>L/3&&_block.size<L-1);
        }
        for (int i = 0; i < _block.size; ++i) {
            if (i==0){
            }else{
                assert(!compKV(_block.data[i],_block.data[i-1]));
            }
        }
        if (!key2.isMin){
            assert(!compK(key2,_block.data[_block.size-1].key));
        }
    }
    void checkNode(ll pos,Key &key,Key &key2){
        node _node;
        readNode(pos,_node);
        if (_node.isRoot){
            assert(_node.size>=0&&_node.size<M-1);
        } else{
            assert(_node.size>M/3&&_node.size<M-1);
        }
        assert(_node.key[0].isMin);
        assert(!compK(_node.key[1],key));
        for (int i = 0; i < _node.size; ++i) {
            assert(!compK(_node.key[i+1],_node.key[i]));
        }
        if (!key2.isMin){
            assert(!compK(key2,_node.key[_node.size]));
        }
        if (_node.isLeaf){
            if (_node.size>=1)check_block(_node.child[0],key,_node.key[1]);
            for (int i = 1; i < _node.size; ++i) {
                check_block(_node.child[i],_node.key[i],_node.key[i+1]);
            }
            check_block(_node.child[_node.size],_node.key[_node.size],key2);
            return;
        } else {
            if (_node.size>=1)checkNode(_node.child[0],key,_node.key[1]);
            for (int i = 1; i < _node.size; ++i) {
                checkNode(_node.child[i],_node.key[i],_node.key[i+1]);
            }
            checkNode(_node.child[_node.size],_node.key[_node.size],key2);
        }
    }
    void check(){
        Key key;
        key.isMin=true;
        checkNode(root,key,key);
    }
public:
    BPT(){
        bool flag=false;
        bptNodes.open("bptNodes",std::ios::in|std::ios::out|std::ios::binary);
        bptBlocks.open("bptBlocks",std::ios::in|std::ios::out|std::ios::binary);
        if (!bptNodes){
            bptNodes.open("bptNodes",std::ios::out|std::ios::binary);
            bptNodes.close();
            bptNodes.open("bptNodes",std::ios::in|std::ios::out|std::ios::binary);
            node _Root(true,true,0,0);
            writeNodeToEnd(_Root);
            nodeNum=1;
            flag=true;
            assert(!bptBlocks);
        }
        if (!bptBlocks){
            bptBlocks.open("bptBlocks",std::ios::out|std::ios::binary);
            bptBlocks.close();
            bptBlocks.open("bptBlocks",std::ios::in|std::ios::out|std::ios::binary);
            block _block(1,-1,-1,NodesFileEnd-sizeof(node));
            _block.data[0].key.isMin=true;
            writeBlockToEnd(_block);
            blockNum=1;
        }
        if (!bptNodes||!bptBlocks){
            std::cerr<<"Error: Cannot open file.\n";
            exit(1);
        }
        if (flag){
            return;
        }
        bptNodes.seekg(0);
        bptNodes.read(reinterpret_cast<char*>(&root),sizeof(ll));
        bptNodes.read(reinterpret_cast<char*>(&nodeNum),sizeof(ll));
        bptNodes.read(reinterpret_cast<char*>(&blockNum),sizeof(ll));
        bptNodes.read(reinterpret_cast<char*>(&NodesFileEnd),sizeof(ll));
        bptNodes.read(reinterpret_cast<char*>(&BlocksFileEnd),sizeof(ll));
        bptNodes.read(reinterpret_cast<char*>(&size),sizeof(ll));
        lruNode.enableFile("bptNodes");
        lruBlock.enableFile("bptBlocks");
    }
    ~BPT(){
        lruNode.disableFile();
        lruBlock.disableFile();
        bptNodes.seekp(0);
        bptNodes.write(reinterpret_cast<char*>(&root),sizeof(ll));
        bptNodes.write(reinterpret_cast<char*>(&nodeNum),sizeof(ll));
        bptNodes.write(reinterpret_cast<char*>(&blockNum),sizeof(ll));
        bptNodes.write(reinterpret_cast<char*>(&NodesFileEnd),sizeof(ll));
        bptNodes.write(reinterpret_cast<char*>(&BlocksFileEnd),sizeof(ll));
        bptNodes.write(reinterpret_cast<char*>(&size),sizeof(ll));
        bptNodes.close();
        bptBlocks.close();
    }
    //TODO使用引用
    void insert(Key key,Value value){//优化！！
        nodeParent.clear();
        blockParent.clear();
        ll i=insertKV(key,value);
        if (i==-1)return;
        //todo
        block _block;
        readBlock(i,_block);
        if (ShouldSplitBlock(_block)){
            splitBlock(i);
        }
    }
    void delete_(Key key,Value value){//优化！！
        nodeParent.clear();
        blockParent.clear();
        ll i=delKV(key,value);
        if (i==-1)return;
        //todo
        block _block;
        readBlock(i,_block);
        if (ShouldMergeBlock(_block)){
            mergeBlock(i);
        }
    }
    Value find2(Key key){
        nodeParent.clear();
        blockParent.clear();
        sjtu::vector<ll> rt;
        rt.push_back(root);
        sjtu::vector<ll> res=findKey(key,rt);
        bool flag=false;
        for (ll i:res){
            block _block;
            readBlock(i,_block);
            for (int j = 0; j < _block.size; ++j) {
                if (equalK(_block.data[j].key,key)){
                    flag=true;
                    return _block.data[j].value;
                    std::cout<<_block.data[j].value<<' ';
                }
            }
        }
        if (!flag){
            return -1;
            std::cout<<"null";
        }
        return Value();
        std::cout<<'\n';
    }
    void find(Key key){
        nodeParent.clear();
        blockParent.clear();
        sjtu::vector<ll> rt;
        rt.push_back(root);
        sjtu::vector<ll> res=findKey(key,rt);
        bool flag=false;
        for (ll i:res){
            block _block;
            readBlock(i,_block);
            for (int j = 0; j < _block.size; ++j) {
                if (equalK(_block.data[j].key,key)){
                    flag=true;
                    std::cout<<_block.data[j].value<<' ';
                }
            }
        }
        if (!flag){
            std::cout<<"null";
        }
        std::cout<<'\n';
    }
    sjtu::vector<Value> find3(Key key){
        sjtu::vector<Value> ans;
        nodeParent.clear();
        blockParent.clear();
        sjtu::vector<ll> rt;
        rt.push_back(root);
        sjtu::vector<ll> res=findKey(key,rt);
        bool flag=false;
        for (ll i:res){
            block _block;
            readBlock(i,_block);
            for (int j = 0; j < _block.size; ++j) {
                if (equalK(_block.data[j].key,key)){
                    flag=true;
                    ans.push_back(_block.data[j].value);
                }
            }
        }
        if (!flag){
            return ans;
        }
        return ans;
    }
};
void clearFile(){
    std::filesystem::remove("bptNodes");
    std::filesystem::remove("bptBlocks");
}
#endif //BPT_BPT_HPP

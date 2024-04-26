//
// Created by qiuyuhang on 24-4-26.
//

#ifndef BPT_MAPANDVECTORFORSUBMIT_HPP
#define BPT_MAPANDVECTORFORSUBMIT_HPP


#include <iostream>
#include <string>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <filesystem>
#include <cassert>
//#include <vector>//!
//#include <map>//!
//#include "STLSrc/map.hpp"
//#include "STLSrc/vector.hpp"
//#ifndef SJTU_UTILITY_HPP
//#define SJTU_UTILITY_HPP
//
//#include <utility>
//
//namespace sjtu {
//
//template<class T1, class T2>
//class pair {
//public:
//	T1 first;
//	T2 second;
//	constexpr pair() : first(), second() {}
//	pair(const pair &other) = default;
//	pair(pair &&other) = default;
//	pair(const T1 &x, const T2 &y) : first(x), second(y) {}
//	template<class U1, class U2>
//	pair(U1 &&x, U2 &&y) : first(x), second(y) {}
//	template<class U1, class U2>
//	pair(const pair<U1, U2> &other) : first(other.first), second(other.second) {}
//	template<class U1, class U2>
//	pair(pair<U1, U2> &&other) : first(other.first), second(other.second) {}
//};
//
//}
//
//#endif
#ifndef SJTU_UTILITY_HPP
#define SJTU_UTILITY_HPP

#include <utility>

namespace sjtu {

    template<class T1, class T2>
    class pair {
    public:
        T1 first;   // first    element
        T2 second;  // second   element

        constexpr pair() = default;
        constexpr pair(const pair &other) = default;
        constexpr pair(pair &&other) = default;

        template<class U1 = T1, class U2 = T2>
        constexpr pair(U1 &&x, U2 &&y)
                : first(std::forward<U1>(x)), second(std::forward<U2>(y)) {}

        template<class U1, class U2>
        constexpr pair(const pair<U1, U2> &other)
                : first(other.first), second(other.second) {}

        template<class U1, class U2>
        constexpr pair(pair<U1, U2> &&other)
                : first(std::move(other.first))
                ,second(std::move(other.second)) {}
    };

    template<class T1, class T2>
    pair(T1, T2) -> pair<T1, T2>;

}

#endif
#ifndef SJTU_EXCEPTIONS_HPP
#define SJTU_EXCEPTIONS_HPP

#include <cstddef>
#include <cstring>
#include <string>

namespace sjtu {

    class exception {
    protected:
        const std::string variant = "";
        std::string detail = "";
    public:
        exception() {}
        exception(const exception &ec) : variant(ec.variant), detail(ec.detail) {}
        virtual std::string what() {
            return variant + " " + detail;
        }
    };

    class index_out_of_bound : public exception {
        /* __________________________ */
    };

    class runtime_error : public exception {
        /* __________________________ */
    };

    class invalid_iterator : public exception {
        /* __________________________ */
    };

    class container_is_empty : public exception {
        /* __________________________ */
    };
}

#endif



#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP


#include <climits>
#include <cstddef>

namespace sjtu {
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
    template<typename T>
    class vector {
    private:
        T *data = nullptr;
        std::allocator<T> allocator;
        size_t _size = 0, max_size = 1;

        void _destroy(int l, int r) {
            for (int i = l; i < r + 1; ++i) {
                std::destroy_at(data + i);
            }
        }

        bool should_half() {
            return 2 * 4 * _size < 3 * max_size;
        }

        void double_size() {
            std::allocator<T> tmp;
            T *p = tmp.allocate(max_size * 2);
            for (int i = 0; i < _size; ++i) {
                std::construct_at(p + i, data[i]);
            }
            _destroy(0, _size - 1);
            allocator.deallocate(data, max_size);
            data = p;
            max_size *= 2;
        }

        void half_size() {
            std::allocator<T> tmp;
            T *p = tmp.allocate(max_size / 2);
            for (int i = 0; i < _size; ++i) {
                std::construct_at(p + i, data[i]);
            }
            _destroy(0, _size - 1);
            allocator.deallocate(data, max_size);
            data = p;
            max_size /= 2;
        }

        void forward(size_t pos_index) {
            if (pos_index == _size) {
                return;
            }
            for (size_t i = _size; i > pos_index; --i) {
                std::construct_at(data + i, data[i - 1]);
                std::destroy_at(data + i - 1);
            }
        }

        void backward(size_t pos_index) {
            for (size_t i = pos_index; i < _size; ++i) {
                std::destroy_at(data + i);
                std::construct_at(data + i, data[i + 1]);
            }
            _destroy(_size - 1, _size - 1);
        }

    public:
        class const_iterator;

        class iterator {
            friend sjtu::vector<T>;
            // The following code is written for the C++ type_traits library.
            // Type traits is a C++ feature for describing certain properties of a type.
            // For instance, for an iterator, iterator::value_type is the type that the
            // iterator points to.
            // STL algorithms and containers may use these type_traits (e.g. the following
            // typedef) to work properly. In particular, without the following code,
            // @code{std::sort(iter, iter1);} would not compile.
            // See these websites for more information:
            // https://en.cppreference.com/w/cpp/header/type_traits
            // About value_type: https://blog.csdn.net/u014299153/article/details/72419713
            // About iterator_category: https://en.cppreference.com/w/cpp/iterator
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T *;
            using reference = T &;
            using iterator_category = std::output_iterator_tag;
        private:
            pointer iter = nullptr;
            T *p = nullptr;
            using Iter = sjtu::vector<value_type>::iterator;

            bool in(Iter l, Iter r) {
                return (this->iter - l.iter >= 0) && (this->iter - r.iter <= 0);
            }

        public:
            /**
             * return a new iterator which pointer n-next elements
             * as well as operator-
             */

            iterator operator+(const int &n) const {
                Iter tmp;
                tmp.iter = iter + n;
                tmp.p = p;
                return tmp;
            }

            iterator operator-(const int &n) const {
                Iter tmp;
                tmp.iter = iter - n;
                tmp.p = p;
                return tmp;
            }

            // return the distance between two iterators,
            // if these two iterators point to different vectors, throw invaild_iterator.
            int operator-(const iterator &rhs) const {
                if (p != rhs.p)throw invalid_iterator();
                return iter - rhs.iter;
            }

            iterator &operator+=(const int &n) {
                iter += n;
                return *this;
            }

            iterator &operator-=(const int &n) {
                iter -= n;
                return *this;
            }

            iterator operator++(int) {
                Iter tmp = *this;
                tmp.p = p;
                iter++;
                return tmp;
            }


            iterator &operator++() {
                iter++;
                return *this;
            }

            iterator operator--(int) {
                Iter tmp = *this;
                iter--;
                tmp.p = p;

                return tmp;
            }

            iterator &operator--() {
                iter--;
                return *this;
            }

            T &operator*() const {
                return *iter;
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory address).
             */
            bool operator==(const iterator &rhs) const {
                return iter == rhs.iter;
            }

            bool operator==(const const_iterator &rhs) const {
                return iter == rhs.iter;
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return iter != rhs.iter;
            }

            bool operator!=(const const_iterator &rhs) const {
                return iter != rhs.iter;
            }
        };

        class const_iterator {
            friend sjtu::vector<T>;
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T *;
            using reference = T &;
            using iterator_category = std::output_iterator_tag;
        private:
            pointer iter = nullptr;
            T *p = nullptr;

            using Iter = sjtu::vector<value_type>::const_iterator;

            bool in(Iter l, Iter r) {
                return (this->iter - l.iter >= 0) && (this->iter - r.iter <= 0);
            }

        public:
            /**
             * return a new const_iterator which pointer n-next elements
             * as well as operator-
             */
            const_iterator operator+(const int &n) const {

                Iter tmp;
                tmp.p = p;
                tmp.iter = iter + n;
                return tmp;
            }

            const_iterator operator-(const int &n) const {

                Iter tmp;
                tmp.p = p;
                tmp.iter = iter - n;
                return tmp;
            }

            // return the distance between two iterators,
            // if these two iterators point to different vectors, throw invaild_iterator.
            int operator-(const const_iterator &rhs) const {
                if (p != rhs.p)throw invalid_iterator();

                return iter - rhs.iter;
            }

            const_iterator &operator+=(const int &n) {

                iter += n;
                return *this;
            }

            const_iterator &operator-=(const int &n) {

                iter -= n;
                return *this;
            }


            const_iterator operator++(int) {
                Iter tmp = *this;
                iter++;
                tmp.p = p;

                return tmp;
            }

            const_iterator &operator++() {
                iter++;
                return *this;
            }

            const_iterator operator--(int) {
                Iter tmp = *this;
                iter--;
                tmp.p = p;

                return tmp;
            }

            const_iterator &operator--() {
                iter--;
                return *this;
            }


            const T &operator*() const {
                return *iter;
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory address).
             */
            bool operator==(const const_iterator &rhs) const {
                return iter == rhs.iter;
            }

            bool operator==(const iterator &rhs) const {
                return iter == rhs.iter;
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return iter != rhs.iter;
            }

            bool operator!=(const const_iterator &rhs) const {
                return iter != rhs.iter;
            }

        };

        vector() {
            data = allocator.allocate(1);
        }

        vector(const vector<T> &other) {
            if (other.data == this->data) {
                return;
            }
            _destroy(0, _size - 1);
            allocator.deallocate(data, max_size);
            _size = other._size;
            max_size = other.max_size;
            std::allocator<T> tmp;
            data = tmp.allocate(max_size);
            for (int i = 0; i < _size; ++i) {
                std::construct_at(data + i, other.data[i]);
            }
        }

        ~vector() {
            _destroy(0, _size - 1);
            allocator.deallocate(data, max_size);
        }

        vector &operator=(const vector &other) {
            if (this->data == other.data) {
                return *this;
            }
            _destroy(0, _size - 1);
            allocator.deallocate(data, max_size);
            _size = other._size;
            max_size = other.max_size;
            data = allocator.allocate(max_size);
            for (int i = 0; i < _size; ++i) {
                std::construct_at(data + i, other.data[i]);
            }
            return *this;
        }

        /**
         * assigns specified element with bounds checking
         * throw index_out_of_bound if pos is not in [0, size)
         */
        T &at(const size_t &pos) {
            if (pos >= _size || pos < 0) {
                throw index_out_of_bound();
            }
            return data[pos];
        }

        const T &at(const size_t &pos) const {
            if (pos >= _size || pos < 0) {
                throw index_out_of_bound();
            }
            return data[pos];
        }

        /**
         * assigns specified element with bounds checking
         * throw index_out_of_bound if pos is not in [0, size)
         * !!! Pay attentions
         *   In STL this operator does not check the boundary but I want you to do.
         */
        T &operator[](const size_t &pos) {
            return at(pos);
        }

        const T &operator[](const size_t &pos) const {
            return at(pos);
        }

        /**
         * access the first element.
         * throw container_is_empty if size == 0
         */
        const T &front() const {
            if (empty())throw container_is_empty();
            return at(0);
        }

        /**
         * access the last element.
         * throw container_is_empty if size == 0
         */
        const T &back() const {
            if (empty())throw container_is_empty();
            return at(_size - 1);
        }

        /**
         * returns an iterator to the beginning.
         */
        iterator begin() const{
            iterator tmp;
            tmp.iter = &data[0];
            tmp.p = data;
            return tmp;
        }

        const_iterator cbegin() const {
            const_iterator tmp;
            tmp.iter = &data[0];
            return tmp;
        }

        /**
         * returns an iterator to the end.
         */
        iterator end() const{
            iterator tmp;
            tmp.p = data;
            tmp.iter = &data[_size];
            return tmp;
        }

        const_iterator cend() const {
            const_iterator tmp;
            tmp.iter = &data[_size];
            return tmp;
        }

        /**
         * checks whether the container is empty
         */
        bool empty() const {
            return _size == 0;
        }

        /**
         * returns the number of elements
         */
        size_t size() const {
            return _size;
        }

        /**
         * clears the contents
         */
        void clear() {
            _destroy(0, _size - 1);
            allocator.deallocate(data, max_size);
            _size = 0;
            max_size = 1;
            data = allocator.allocate(1);
        }

        /**
         * inserts value before pos
         * returns an iterator pointing to the inserted value.
         */
        iterator insert(iterator pos, const T &value) {
            if (!pos.in(begin(), end()))throw invalid_iterator();
            size_t index = pos - begin();
            if (_size == max_size - 1)double_size();
            forward(index);
            std::construct_at(data + index, value);
            _size++;
            return begin() + index;
        }

        /**
         * inserts value at index ind.
         * after inserting, this->at(ind) == value
         * returns an iterator pointing to the inserted value.
         * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
         */
        iterator insert(const size_t &ind, const T &value) {
            return insert(begin() + ind, value);
        }

        /**
         * removes the element at pos.
         * return an iterator pointing to the following element.
         * If the iterator pos refers the last element, the end() iterator is returned.
         */
        iterator erase(iterator pos) {
            if (!pos.in(begin(), end() - 1))throw invalid_iterator();
            size_t index = pos - begin();
            backward(index);
            _size--;
            if (should_half())half_size();
            return begin() + index;
        }

        /**
         * removes the element with index ind.
         * return an iterator pointing to the following element.
         * throw index_out_of_bound if ind >= size
         */
        iterator erase(const size_t &ind) {
            if (ind >= _size || ind < 0) {
                throw index_out_of_bound();
            }
            return erase(begin() + ind);
        }

        /**
         * adds an element to the end.
         */
        void push_back(const T &value) {
            if (_size >= max_size - 1)double_size();
            std::construct_at(data + _size, value);
            _size++;
        }

        /**
         * remove the last element from the end.
         * throw container_is_empty if size() == 0
         */
        void pop_back() {
            if (_size == 0)throw container_is_empty();
            _destroy(_size - 1, _size - 1);
            _size--;
            if (should_half())half_size();
        }
    };


}

#endif

/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

#include <cstdio>
#include <cstring>
#include <iostream>
#include <cmath>
#include <string>
#include <functional>
#include <cstddef>


namespace sjtu {

    template<
            class Key,
            class T,
            class Compare = std::less<Key>
    >
    class map {
    public:
        /**
         * the internal type of data.
         * it should have a default constructor, a copy constructor.
         * You can use sjtu::map as value_type by typedef.
         */
        typedef pair<Key, T> value_type;

    private:
        enum subTree {
            left, right
        };

        struct node {
            node *father = nullptr;
            node *left = nullptr;
            node *right = nullptr;
            value_type val;
            int height = 0;

            node(Key k, T v, node *f = NULL, node *l = NULL, node *r = NULL) : val(value_type(k, v)), father(f),
                                                                               left(l), right(r) {}

            node(value_type valueType, node *f = NULL, node *l = NULL, node *r = NULL) : val(valueType), father(f),
                                                                                         left(l), right(r) {}

            node(const node &other) : val(other.val), height(other.height) {
            }


            Key &get_key() {
                return val.first;
            }

            T &get_t() {
                return val.second;
            }

            node *find(const Key &_key) {
                if (eq(_key, get_key())) return this;
                if (Compare()(_key, get_key())) {
                    if (left == nullptr) return nullptr;
                    return left->find(_key);
                } else {
                    if (right == nullptr) return nullptr;
                    return right->find(_key);
                }
            }
        };
    private:
        node *root = nullptr;
        size_t _size = 0;
        int *posi = nullptr;
        static bool eq(const Key &l, const Key &r) {
            return !(Compare()(l, r) || Compare()(r, l));
        }

        void exchange(node *higher, node *lower) {
            if (higher == root)root = lower;
            int tmp = lower->height;
            lower->height = higher->height;
            higher->height = tmp;
            node *hf = higher->father;
            node *hl = higher->left;
            node *hr = higher->right;
            node *lf = lower->father;
            node *ll = lower->left;
            node *lr = lower->right;
            if (lower->father != higher) {
                higher->father = lf;
                higher->left = ll;
                higher->right = lr;
                lower->father = hf;
                lower->left = hl;
                lower->right = hr;
                if (hf != nullptr) {
                    if (hf->left == higher)hf->left = lower;
                    else hf->right = lower;
                }
                if (lf != nullptr) {
                    if (lf->left == lower)lf->left = higher;
                    else lf->right = higher;
                }
                if (ll != nullptr)ll->father = higher;
                if (lr != nullptr)lr->father = higher;
                if (hl != nullptr)hl->father = lower;
                if (hr != nullptr)hr->father = lower;
            } else {
                if (ll != nullptr)ll->father = higher;
                if (lr != nullptr)lr->father = higher;
                higher->left = ll;
                higher->right = lr;
                lower->father = higher->father;
                if (hf != nullptr) {
                    if (hf->left == higher)hf->left = lower;
                    else hf->right = lower;
                }
                higher->father = lower;
                if (hl == lower) {
                    lower->left = higher;
                    lower->right = hr;
                    if (hr != nullptr)hr->father = lower;
                } else {
                    lower->left = hl;
                    lower->right = higher;
                    if (hl != nullptr)hl->father = lower;
                }
            }
        }

        int height(node *nd) {
            if (nd == nullptr)return -1;
            return nd->height;
        }

        void back_height(node *nd) {
            if (nd == nullptr)return;
            if (nd->father == nullptr)return;
            int old_height = nd->father->height;
            nd->father->height = max_height(nd->father->left, nd->father->right) + 1;
        }

        size_t max_height(node *l, node *r) {
            if (l == nullptr && r == nullptr) return -1;
            if (l == nullptr) return r->height;
            if (r == nullptr) return l->height;
            return l->height > r->height ? l->height : r->height;
        }

        void clear(node *&nd) {
            if (nd == nullptr) return;
            clear(nd->left);
            clear(nd->right);
            delete nd;
            nd = nullptr;
        }

        node *copy(node *&nd, const node *other) {
            if (other == nullptr)return nullptr;
            nd = new node(*other);
            node *l = copy(nd->left, other->left);
            node *r = copy(nd->right, other->right);
            nd->right = r;
            nd->left = l;
            if (r != nullptr)r->father = nd;
            if (l != nullptr)l->father = nd;
            return nd;
        }

        size_t ll(node *nd) {
            node *f = nd->father, *s = nd, *ls = nd->left, *lsrs = ls->right;
            if (f != nullptr) {
                if (f->left == s)f->left = ls;
                else f->right = ls;
            } else {
                root = ls;
            }
            nd = ls;
            nd->father = f;
            nd->right = s;
            s->father = nd;
            s->left = lsrs;
            if (lsrs != nullptr)lsrs->father = s;
            s->height = max_height(s->left, s->right) + 1;
            nd->height = max_height(nd->left, nd->right) + 1;
            return nd->height;
        }

        size_t rr(node *nd) {
            node *f = nd->father, *s = nd, *rs = nd->right, *rsrl = rs->left;
            node *tmp = nd->left;
            if (f != nullptr) {
                if (f->right == s)f->right = rs;
                else f->left = rs;
            } else {
                root = rs;//?
            }
            nd = rs;
            nd->father = f;
            nd->left = s;
            s->father = nd;
            s->right = rsrl;
            if (rsrl != nullptr)rsrl->father = s;
            s->height = max_height(s->left, s->right) + 1;
            nd->height = max_height(nd->left, nd->right) + 1;
            return nd->height;
        }

        size_t lr(node *&nd) {
            rr(nd->left);
            ll(nd);
            return nd->height;
        }

        size_t rl(node *&nd) {
            ll(nd->right);
            rr(nd);
            return nd->height;
        }

        node *_insert(node *&nd, node *f, const Key &key, const T &t, subTree sub = left) {
            if (nd == nullptr) {
                nd = new node(key, t, f, nullptr, nullptr);
                if (f != nullptr) {
                    if (sub == left)f->left = nd;
                    else f->right = nd;
                }
                if (nd->father != nullptr) nd->father->height = max_height(nd->father->left, nd->father->right) + 1;
                return nd;
            }
            node *tmp = nullptr;
            if (Compare()(key, nd->get_key())) {
                tmp = _insert(nd->left, nd, key, t, left);
                if (height(nd->left) - height(nd->right) >= 2) {
                    if (height(nd->left->left) >= height(nd->left->right))ll(nd);
                    else lr(nd);
                }
            } else {
                tmp = _insert(nd->right, nd, key, t, right);
                if (height(nd->right) - height(nd->left) >= 2) {
                    if (height(nd->right->right) >= height(nd->right->left))rr(nd);
                    else rl(nd);
                }
            }
            nd->height = max_height(nd->left, nd->right) + 1;
            return tmp;
        }

        void remove(node *&nd, node *_rt) {
            if (nd->left == nullptr && nd->right == nullptr) {
                nd->height = -1;
                if (nd == root) {
                    delete nd;
                    nd = nullptr;
                    root = nullptr;
                    return;
                }
                if (nd->father->left == nd) {
                    nd->father->height = max_height(nd, nd->father->right) + 1;
                    nd->father->left = nullptr;
                    if (nd != root) adjust(nd->father, left);
                    delete nd;
                    nd = nullptr;
                } else {
                    nd->father->height = max_height(nd, nd->father->left) + 1;
                    nd->father->right = nullptr;
                    if (nd != root)
                        adjust(nd->father, right);
                    delete nd;
                    nd = nullptr;
                }

            } else if (nd->right == nullptr) {
                if (nd == root) {
                    node *tmp = nd->left;
                    delete nd;
                    nd = nullptr;
                    root = tmp;
                    root->father = nullptr;
                    return;
                }
                if (nd->father->left == nd) {
                    nd->father->left = nd->left;
                    nd->left->father = nd->father;
                    nd->father->height = max_height(nd->father->left, nd->father->right) + 1;
                    if (nd != root)
                        adjust(nd->father, left);
                    delete nd;
                    nd = nullptr;
                } else {
                    nd->father->right = nd->left;
                    nd->left->father = nd->father;
                    nd->father->height = max_height(nd->father->left, nd->father->right) + 1;
                    if (nd != root)
                        adjust(nd->father, right);
                    delete nd;
                    nd = nullptr;
                }
            } else if (nd->left == nullptr) {
                if (nd == root) {
                    node *tmp = nd->right;
                    delete nd;
                    nd = nullptr;
                    root = tmp;
                    root->father = nullptr;
                    return;
                }
                if (nd->father->left == nd) {
                    nd->father->left = nd->right;
                    nd->right->father = nd->father;
                    if (nd != root) {
                        nd->father->height = max_height(nd->father->left, nd->father->right) + 1;
                        adjust(nd->father, left);
                    }
                    delete nd;
                    nd = nullptr;
                } else {
                    nd->father->right = nd->right;
                    nd->right->father = nd->father;
                    if (nd != root) {
                        nd->father->height = max_height(nd->father->left, nd->father->right) + 1;
                        adjust(nd->father, right);
                    }
                    delete nd;
                    nd = nullptr;
                }
            } else {
                node *tmp = getll(nd->right);
                exchange(nd, tmp);
                remove(nd, _rt);
            }
        }

        void adjust(node *&t, subTree sub) {
            if (t == nullptr)return;
            if (!(height(t->right) - height(t->left) == 1 || height(t->right) - height(t->left) == 0 ||
                  (height(t->right) - height(t->left) == 2 &&
                   (height(t->right->left) - height(t->right->right) == 1 ||
                    height(t->right->left) - height(t->right->right) == 0 ||
                    height(t->right->left) - height(t->right->right) == -1))) &&
                !(height(t->left) - height(t->right) == 1 || height(t->left) - height(t->right) == 0 ||
                  (height(t->left) - height(t->right) == 2 &&
                   (height(t->left->right) - height(t->left->left) == 1 ||
                    height(t->left->right) - height(t->left->left) == 0 ||
                    height(t->left->right) - height(t->left->left) == -1))))
                return;
            if (sub == left) {
                if (height(t->right) - height(t->left) == 1) {
                    back_height(t);
                    return;
                } else if (height(t->right) == height(t->left)) {
                    if (t != root) {
                        t->father->height = max_height(t->father->left, t->father->right) + 1;
                        adjust(t->father, t->father->left == t ? left : right);
                    }
                    back_height(t);
                } else {
                    bool flag = height(t->right->left) != height(t->right->right);
                    if (height(t->right->left) > height(t->right->right)) {
                        rl(t);
                        if (t != root)
                            adjust(t->father, t->father->left == t ? left : right);
                        back_height(t);
                        return;
                    }
                    rr(t);
                    if (t != root)
                        t->father->height = max_height(t->father->left, t->father->right) + 1;
                    if (flag) {
                        if (t != root)
                            adjust(t->father, t->father->left == t ? left : right);
                    }
                    back_height(t);
                }
            } else {
                if (height(t->left) - height(t->right) == 1) {
                    back_height(t);
                    return;
                } else if (height(t->left) == height(t->right)) {
                    if (t != root) {
                        t->father->height = max_height(t->father->left, t->father->right) + 1;
                        adjust(t->father, t->father->left == t ? left : right);
                    }
                    back_height(t);
                } else {
                    bool flag = height(t->left->right) != height(t->left->left);
                    if (height(t->left->right) > height(t->left->left)) {
                        lr(t);
                        if (t != root)
                            adjust(t->father, t->father->left == t ? left : right);
                        back_height(t);
                        return;
                    }
                    ll(t);
                    if (t != root)
                        t->father->height = max_height(t->father->left, t->father->right) + 1;
                    if (flag && t != root) {
                        adjust(t->father, t->father->left == t ? left : right);
                    }
                    back_height(t);
                }
            }
            t->height = max_height(t->left, t->right) + 1;
        }

        static node *getll(node *_nd) {//得到最左的孩子
//            if (_nd== nullptr)return _nd;
            if (_nd->left == nullptr)return _nd;
            return getll(_nd->left);
        }

        node *getll_() const{//得到最左的孩子
            return getll(root);
        }

        static node *getrr(node *_nd) {//得到最右的孩子
//            if (_nd== nullptr)return _nd;
            if (_nd->right == nullptr)return _nd;
            return getrr(_nd->right);
        }

        node *getrr_() const{//得到最右的孩子
            return getrr(root);
        }


    public:
        class iterator;
//    private:
    public:
        bool good(node *nd) {
            if (nd == nullptr)return true;
            if (nd->father == nullptr && nd != root)return false;
            return (height(nd->left) - height(nd->right) < 2 && height(nd->right) - height(nd->left) < 2 &&
                    nd->height == max_height(nd->left, nd->right) + 1);
        }

        void print() {
            pt(root);
            std::cout << std::endl;
        }

        void pt(node *nd) {
            if (nd == nullptr)return;
            std::cout << nd->val.first << '(' << nd->height << ')' << " ";
            std::cout << "[";
            if (nd->father != nullptr)std::cout << 'f' << nd->father->val.first << ' ';
            if (nd->left != nullptr)std::cout << 'l' << nd->left->val.first << ' ';
            if (nd->right != nullptr)std::cout << 'r' << nd->right->val.first << ' ';
            std::cout << "] ";
            pt(nd->left);
            pt(nd->right);
        }

        void pt(iterator nnd) {
            if (nnd == end())return;
            node *nd = nnd.nd;
            if (nd == nullptr)return;
            std::cout << nd->val.first << '(' << nd->height << ')' << " ";
            pt(nd->left);
            pt(nd->right);
        }

        void ck(node *nd) {
            if (nd == nullptr)return;
            ck(nd->left);
            ck(nd->right);
//            assert(good(nd));
//            if (nd->left != nullptr)assert(nd->left->father == nd);
//            if (nd->right != nullptr)assert(nd->right->father == nd);
        }

        void check() {
            ck(root);
        }

    public:

        /**
         * see BidirectionalIterator at CppReference for help.
         *
         * if there is anything wrong throw invalid_iterator.
         *     like it = map.begin(); --it;
         *       or it = map.end(); ++end();
         */
        class const_iterator;

        class iterator {
            friend map;
        private:

            map<Key, T, Compare> *rt = nullptr;
            node *nd = nullptr;
            // int *rt = nullptr;

            iterator(node *_nd, map<Key, T, Compare> *mp_) {
                nd = _nd;
                rt = mp_;
//                mp = mp_;
                // rt_real=_rr;
//                posi = p;
            }
            /**
             * TODO add data members
             *   just add whatever you want.
             */
        public:
            struct my_itr {
            };
            my_itr myItr;

            iterator() {}

            iterator(const iterator &other) {
                rt = other.rt;
                nd = other.nd;
            }

            iterator operator++(int) {
                if (nd == nullptr) {
                    throw invalid_iterator();
                }
                node *tmp = nd;
                if (nd->right != nullptr) {
                    nd = getll(nd->right);
                    return iterator(tmp, rt);
                }
                while (nd->father != nullptr && nd->father->right == nd) {
                    nd = nd->father;
                }
                nd = nd->father;
                return iterator(tmp, rt);
            }

            iterator &operator++() {
                if (nd == nullptr) {
                    throw invalid_iterator();
                }
                if (nd->right != nullptr) {
                    nd = getll(nd->right);
                    return *this;
                }
                while (nd->father != nullptr && nd->father->right == nd) {
                    nd = nd->father;
                }
                nd = nd->father;
                return *this;
            }

            iterator operator--(int) {
                node *tmp = nd;
                if (nd == nullptr) {
                    if (rt == nullptr || rt->root == nullptr)throw invalid_iterator();
                    nd = rt->getrr_();
                    return iterator(tmp, rt);
                }
                if (nd->left != nullptr) {
                    nd = getrr(nd->left);
                    return iterator(tmp, rt);
                }
                while (nd->father != nullptr && nd->father->left == nd) {
                    nd = nd->father;
                }
                if (nd == rt->root) {//?
                    throw invalid_iterator();
                }
                nd = nd->father;
                return iterator(tmp, rt);
            }

            iterator &operator--() {
                if (nd == nullptr) {
                    if (rt == nullptr||rt->root== nullptr)throw invalid_iterator();
                    nd = rt->getrr_();
                    return *this;
                }
                if (nd->left != nullptr) {
                    nd = getrr(nd->left);
                    return *this;
                }
                while (nd->father != nullptr && nd->father->left == nd) {
                    nd = nd->father;
                }
                if (nd == rt->root) {
                    throw invalid_iterator();
                }
                nd = nd->father;
                return *this;
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            value_type &operator*() const {
                return nd->val;
            }

            bool operator==(const iterator &rhs) const {
                return nd == rhs.nd && rt == rhs.rt;
            }

            bool operator==(const const_iterator &rhs) const {
                return nd == rhs.nd && rt == rhs.rt;
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return nd != rhs.nd || rt != rhs.rt;
            }

            bool operator!=(const const_iterator &rhs) const {
                return nd != rhs.nd || rt != rhs.rt;
            }

            /**
             * for the support of it->first.
             * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
             */
            value_type *operator->() const noexcept {
                if (nd == nullptr)throw invalid_iterator();
                return &(nd->val);
            }
        };

        class const_iterator {
            friend map;
        private:

            const map<Key, T, Compare> *rt = nullptr;
            node *nd = nullptr;
            // int *rt = nullptr;

            const_iterator(node *_nd, const map<Key, T, Compare> *mp_) {
                nd = _nd;
                rt = mp_;
//                mp = mp_;
                // rt_real=_rr;
//                posi = p;
            }
            /**
             * TODO add data members
             *   just add whatever you want.
             */
        public:
            struct my_itr {
            };
            my_itr myItr;

            const_iterator() {}

            const_iterator(const const_iterator &other) {
                rt = other.rt;
                nd = other.nd;
            }

            const_iterator(const iterator &other) {
                rt = other.rt;
                nd = other.nd;
            }

            const_iterator operator++(int) {
                if (nd == nullptr) {
                    throw invalid_iterator();
                }
                node *tmp = nd;
                if (nd->right != nullptr) {
                    nd = getll(nd->right);
                    return const_iterator(tmp, rt);
                }
                while (nd->father != nullptr && nd->father->right == nd) {
                    nd = nd->father;
                }
                nd = nd->father;
                return const_iterator(tmp, rt);
            }

            const_iterator &operator++() {
                if (nd == nullptr) {
                    throw invalid_iterator();
                }
                if (nd->right != nullptr) {
                    nd = getll(nd->right);
                    return *this;
                }
                while (nd->father != nullptr && nd->father->right == nd) {
                    nd = nd->father;
                }
                nd = nd->father;
                return *this;
            }

            const_iterator operator--(int) {
                node *tmp = nd;
                if (nd == nullptr) {
                    if (rt == nullptr || rt->root == nullptr)throw invalid_iterator();
                    nd = rt->getrr_();
                    return const_iterator(tmp, rt);
                }
                if (nd->left != nullptr) {
                    nd = getrr(nd->left);
                    return const_iterator(tmp, rt);
                }
                while (nd->father != nullptr && nd->father->left == nd) {
                    nd = nd->father;
                }
                if (nd == rt->root) {//?
                    throw invalid_iterator();
                }
                nd = nd->father;
                return const_iterator(tmp, rt);
            }

            const_iterator &operator--() {
                if (nd == nullptr) {
                    if (rt == nullptr||rt->root== nullptr)throw invalid_iterator();
                    nd = rt->getrr_();
                    return *this;
                }
                if (nd->left != nullptr) {
                    nd = getrr(nd->left);
                    return *this;
                }
                while (nd->father != nullptr && nd->father->left == nd) {
                    nd = nd->father;
                }
                if (nd == rt->root) {
                    throw invalid_iterator();
                }
                nd = nd->father;
                return *this;
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            value_type &operator*() const {
                return nd->val;
            }

            bool operator==(const iterator &rhs) const {
                return nd == rhs.nd && rt == rhs.rt;
            }

            bool operator==(const const_iterator &rhs) const {
                return nd == rhs.nd && rt == rhs.rt;
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return nd != rhs.nd || rt != rhs.rt;
            }

            bool operator!=(const const_iterator &rhs) const {
                return nd != rhs.nd || rt != rhs.rt;
            }

            /**
             * for the support of it->first.
             * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
             */
            value_type *operator->() const noexcept {
                if (nd == nullptr)throw invalid_iterator();
                return &(nd->val);
            }
        };

        map() {
            posi = new int;
        }

        map(const map &other) : map() {
            if (root == other.root)return;
            clear(root);
            delete posi;
            posi = new int;
            _size = other._size;
            copy(root, other.root);
        }

        map &operator=(const map &other) {
            if (root == other.root)return *this;
            clear(root);
            delete posi;
            posi = new int;
            _size = other._size;
            copy(root, other.root);
            return *this;
        }

        ~map() {
            delete posi;
            clear(root);
        }

        /**
         * access specified element with bounds checking
         * Returns a reference to the mapped value of the element with key equivalent to key.
         * If no such element exists, an exception of type `index_out_of_bound'
         */
        T &at(const Key &key) {
            if (root == nullptr)throw index_out_of_bound();
            node *tmp = root->find(key);
            if (tmp == nullptr)throw index_out_of_bound();
            return tmp->get_t();
        }

        const T &at(const Key &key) const {
            if (root == nullptr)throw index_out_of_bound();
            node *tmp = root->find(key);
            if (tmp == nullptr)throw index_out_of_bound();
            return tmp->get_t();
        }

        /**
         * access specified element
         * Returns a reference to the value that is mapped to a key equivalent to key,
         *   performing an insertion if such key does not already exist.
         */
        T &operator[](const Key &key) {
            if (root == nullptr) {
                _size++;
                return _insert(root, nullptr, key, T())->get_t();
            }
            node *tmp = root->find(key);
            if (tmp == nullptr) {
                _size++;
                return _insert(root, nullptr, key, T())->get_t();
            } else return tmp->get_t();
        }

        /**
         * behave like at() throw index_out_of_bound if such key does not exist.
         */
        const T &operator[](const Key &key) const {
            return at(key);
        }

        /**
         * return a iterator to the beginning
         */
        iterator begin() {
            if (root == nullptr)return iterator(root, this);
            node *nd = getll(root);
            return iterator(nd, this);
        }

        const_iterator cbegin() const {
            if (root == nullptr)return const_iterator(root, this);
            node *nd = getll(root);
            return const_iterator(nd, this);
        }

        /**
         * return a iterator to the end
         * in fact, it returns past-the-end.
         */
        iterator end() {
            return iterator(nullptr, this);
        }

        const_iterator cend() const {
            return const_iterator(nullptr, this);
        }

        /**
         * checks whether the container is empty
         * return true if empty, otherwise false.
         */
        bool empty() const {
            return _size == 0;
        }

        /**
         * returns the number of elements.
         */

        size_t size() const {
            return _size;
        }

        /**
         * clears the contents
         */
        void clear() {
            clear(root);
            _size = 0;
        }

        /**
         * insert an element.
         * return a pair, the first of the pair is
         *   the iterator to the new element (or the element that prevented the insertion),
         *   the second one is true if insert successfully, or false.
         */
        pair<iterator, bool> insert(const value_type &value) {
            node *no = nullptr;
            node *tmp = nullptr;
            if (root != nullptr)tmp = root->find(value.first);
            if (tmp != nullptr)return pair<iterator, bool>(iterator(tmp, this), false);
            _size++;
            return pair<iterator, bool>(iterator(_insert(root, no, value.first, value.second), this), true);
        }

        /**
         * erase the element at pos.
         *
         * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
         */
        void erase(iterator pos) {
            if (pos.rt != this || pos.nd == nullptr)throw invalid_iterator();
            remove(pos.nd, root);
            _size--;
        }

        bool erase(const Key &key) {
            if (root == nullptr)return false;
            node *tmp = root->find(key);
            if (tmp == nullptr)return false;
            remove(tmp, root);
            _size--;
            return true;
        }

        /**
         * Returns the number of elements with key
         *   that compares equivalent to the specified argument,
         *   which is either 1 or 0
         *     since this container does not allow duplicates.
         * The default method of check the equivalence is !(a < b || b > a)
         */
        size_t count(const Key &key) const {
            if (root == nullptr)return 0;
            node *nd = root->find(key);
            if (nd == nullptr)return 0;
            return 1;
        }

        /**
         * Finds an element with key equivalent to key.
         * key value of the element to search for.
         * Iterator to an element with key equivalent to key.
         *   If no such element is found, past-the-end (see end()) iterator is returned.
         */
        iterator find(const Key &key) {
            if (root == nullptr)return iterator(nullptr, this);
            node *nd = root->find(key);
            return iterator(nd, this);
        }

        const_iterator find(const Key &key) const {
            if (root == nullptr)return const_iterator(nullptr, this);
            node *nd = root->find(key);
            return const_iterator(nd, this);
        }
    };

}

#endif
#endif //BPT_MAPANDVECTORFORSUBMIT_HPP
//#include "STLSrc/MapAndVectorForSubmit.hpp"
//
// Created by qiuyuhang on 24-4-26.
//

#ifndef BPT_LRU_HPP
#define BPT_LRU_HPP

#include <cstddef>
#include <iostream>
#include <cassert>


template<typename Hash, typename Key>
concept Hashable = requires(Key key){
    { Hash()(key) }->std::same_as<size_t>;
};
static const int TableCapacity = 1e2+3;//!2
static const int LinkCapacity = 1e2;//!2
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
    class HashNode;
    struct DataNode {
        Key key;
        Block block;
        HashNode *hashNode = nullptr;
        DataNode *prev = nullptr;
        DataNode *next = nullptr;

        DataNode() = default;

        DataNode(Key key, Block block, DataNode *next = nullptr, DataNode *prev = nullptr) : key(key), block(block),
                                                                                             next(next), prev(prev) {}
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
    }

    void insert(Key key, Block &block) {
        DataNode *p=findAndMoveToHead(key);
        if (p != nullptr) {
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

    bool get(Key key, Block &block) {
        DataNode *p = findAndMoveToHead(key);
        if (p == nullptr) {
            return false;
        }
        block = p->block;
        return true;
    }
};

#endif //BPT_LRU_HPP
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
//#include "STLSrc/map.hpp"
//#include "STLSrc/vector.hpp"
//#include "LRU.hpp"
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
//    static const int M=200+1;
//    static const int L=200+1;
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
        bptNodes.seekp(NodesFileEnd, std::ios::beg);
        NodesFileEnd+=sizeof(node);
        bptNodes.write(reinterpret_cast<char*>( &_node),sizeof(node));
        ++nodeNum;
    }
    void writeNode(ll pos,node& _node){//没有空间回收//没有++nodeNum
        assert(NodesFileEnd>cut-1);
        lruNode.insert(pos,_node);
        bptNodes.seekp(pos, std::ios::beg);
        bptNodes.write(reinterpret_cast<char*>( &_node),sizeof(node));
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
        bptBlocks.seekp(BlocksFileEnd, std::ios::beg);
        BlocksFileEnd+=sizeof(block);
        bptBlocks.write(reinterpret_cast<char*>(&_block),sizeof(block));
        ++blockNum;
    }
    void writeBlock(ll pos,block& _block){//没有空间回收//没有++blockNum
        assert(BlocksFileEnd>-1);
        lruBlock.insert(pos,_block);
        bptBlocks.seekp(pos, std::ios::beg);
        bptBlocks.write(reinterpret_cast<char*>(&_block),sizeof(block));
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
    }
    ~BPT(){
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
//#include "BPT.hpp"
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


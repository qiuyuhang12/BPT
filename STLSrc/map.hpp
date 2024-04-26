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
#include "utility.hpp"
#include "exceptions.hpp"

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

    private:
        node *root = nullptr;
        size_t _size = 0;
        int *posi = nullptr;
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

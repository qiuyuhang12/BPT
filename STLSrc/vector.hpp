#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

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

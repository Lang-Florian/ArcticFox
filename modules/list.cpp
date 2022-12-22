#pragma once

#include <algorithm>
#include <numeric>

template <typename T, int MAX_SIZE>
class List {
private:
  T array[MAX_SIZE];
  int _size_;

public:
  List() {
    this->_size_ = 0;
  };

  T* begin() {
    return this->array;
  };
  
  T* end() {
    return this->array + this->_size_;
  };

  T& operator[](unsigned int index) {
    return this->array[index];
  };

  void push(T value) {
    this->array[this->_size_] = value;
    ++this->_size_;
  };

  T pop() {
    return this->array[--this->_size_];
  };

  int size() {
    return this->_size_;
  };

  bool empty() {
    return this->_size_ == 0;
  };

  void clear() {
    this->_size_ = 0;
  };

  bool contains(T value) {
    return std::find(this->array, this->array + this->_size_, value) != this->array + this->_size_;
  };

  int count(auto condition) {
    return std::count_if(this->array, this->array + this->_size_, condition);
  };

  void sort() {
    std::sort(this->array, this->array + this->_size_);
  };

  void sort(auto comparator) {
    std::sort(this->array, this->array + this->_size_, comparator);
  };

  void reverse() {
    std::reverse(this->array, this->array + this->_size_);
  };

  template <int OTHER_MAX_SIZE>
  void append(List<T, OTHER_MAX_SIZE> other) {
    std::copy(other.begin(), other.end(), this->array + this->_size_);
    this->_size_ += other.size();
  };

  List<T, MAX_SIZE> copy() {
    List<T, MAX_SIZE> copy;
    std::copy(this->array, this->array + this->_size_, copy.array);
    copy._size_ = this->_size_;
    return copy;
  };
};
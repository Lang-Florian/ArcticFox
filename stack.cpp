#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#ifndef __STACK__MODULE__ 
#define __STACK__MODULE__


#include <algorithm>


/*
  
  a class for a stack of elements
  up to a maximum size of MAX_SIZE

  the stack is implemented as an array
  and has important functions for manipulating the stack

*/
template <typename T, int MAX_SIZE>
class Stack {
  private:
    T array[MAX_SIZE];
    int _size_;

  public:
    Stack() {
      this->_size_ = 0;
    };

    T& operator[](unsigned int index) {
      return this->array[index];
    };

    T* begin() {
      return this->array;
    };
    
    T* end() {
      return this->array + this->_size_;
    };

    void push(T value) {
      this->array[this->_size_++] = value;
    };

    T pop() {
      return this->array[--this->_size_];
    };

    T pop(bool check) {
      return this->array[this->_size_ -= check];
    };

    int size() {
      return this->_size_;
    };

    void clear() {
      this->_size_ = 0;
    };

    int index(T value) {
      return std::find(this->array, this->array + this->_size_, value) - this->array;
    };

    int index(bool (check)(T)) {
      return std::find_if(this->array, this->array + this->_size_, check) - this->array;
    };

    int index(auto check) {
      return std::find_if(this->array, this->array + this->_size_, check) - this->array;
    };

    bool contains(T value) {
      return std::search(this->array, this->array + this->_size_, &value, &value + 1) != this->array + this->_size_;
    };

    bool contains(bool (check)(T)) {
      return std::find_if(this->array, this->array + this->_size_, check) != this->array + this->_size_;
    };

    bool contains(auto check) {
      return std::find_if(this->array, this->array + this->_size_, check) != this->array + this->_size_;
    };

    int count(T value) {
      return std::count(this->array, this->array + this->_size_, value);
    };

    int count(bool (check)(T)) {
      return std::count_if(this->array, this->array + this->_size_, check);
    };

    int count(auto check) {
      return std::count_if(this->array, this->array + this->_size_, check);
    };

    void sort() {
      std::stable_sort(this->array, this->array + this->_size_);
    };
    
    void sort(bool (comparison)(T, T)) {
      std::stable_sort(this->array, this->array + this->_size_, comparison);
    };

    void sort(auto comparison) {
      std::stable_sort(this->array, this->array + this->_size_, comparison);
    };

    void reverse() {
      std::reverse(this->array, this->array + this->_size_);
    };

    Stack<T, MAX_SIZE> copy() {
      Stack<T, MAX_SIZE> copy;
      std::copy(this->array, this->array + this->_size_, copy.array);
      copy._size_ = this->_size_;
      return copy;
    };
};


#endif // __STACK__MODULE__
#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#ifndef __STACK__MODULE__ 
#define __STACK__MODULE__


#include <algorithm>
#include <numeric>


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

    T* begin() {
      return this->array;
    };
    
    T* end() {
      return this->array + this->_size_;
    };

    // get an element from the stack
    T& operator[](unsigned int index) {
      return this->array[index];
    };

    // push an element onto the stack
    void push(T value) {
      this->array[this->_size_++] = value;
    };

    // pop an element from the stack
    T pop() {
      return this->array[--this->_size_];
    };

    // pop an element from the stack if an condition is met
    T pop(auto condition) {
      return this->array[this->_size_ -= condition];
    };

    // get the size of the stack
    int size() {
      return this->_size_;
    };

    // check if the stack is empty
    bool empty() {
      return this->_size_ == 0;
    };

    // clear the stack
    void clear() {
      this->_size_ = 0;
    };

    // check if the stack contains a value
    bool contains(T value) {
      return std::find(this->array, this->array + this->_size_, value) != this->array + this->_size_;
    };

    // count the number of elements in the stack that satisfy a condition
    int count(auto condition) {
      return std::count_if(this->array, this->array + this->_size_, condition);
    };

    // sort the stack
    void sort() {
      std::sort(this->array, this->array + this->_size_);
    };

    // sort the stack with a custom comparator
    void sort(auto comparator) {
      std::sort(this->array, this->array + this->_size_, comparator);
    };

    // reverse the stack
    void reverse() {
      std::reverse(this->array, this->array + this->_size_);
    };

    // append another stack to this stack
    template <int OTHER_MAX_SIZE>
    void append(Stack<T, OTHER_MAX_SIZE> other) {
      std::copy(other.begin(), other.end(), this->array + this->_size_);
      this->_size_ += other.size();
    };

    // create a copy of the stack
    Stack<T, MAX_SIZE> copy() {
      Stack<T, MAX_SIZE> copy;
      std::copy(this->array, this->array + this->_size_, copy.array);
      copy._size_ = this->_size_;
      return copy;
    };
};


#endif // __STACK__MODULE__
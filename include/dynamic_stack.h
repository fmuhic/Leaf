#pragma once

#include "types.h"
#include <cassert>
#include <cstring>

template <typename T>
class DynamicStack {
    public:
    DynamicStack();
    DynamicStack(i32 initCapacity);
    ~DynamicStack();

    T pop();
    void push(T);
    inline void reset() { size = 0; }
    inline i32 getSize() { return size; }
    inline bool isEmpty() { return size == 0; }
    // Remove this once set is implemented
    bool contains(T value);

    private:
    void expandElements();

    T* elements = nullptr;
    i32 capacity;
    i32 size = 0;
};

template <typename T>
DynamicStack<T>::DynamicStack(i32 initCapacity): capacity(initCapacity) {
    elements = new T[capacity];
}

template <typename T>
DynamicStack<T>::DynamicStack(): DynamicStack(16) {}

template <typename T>
DynamicStack<T>::~DynamicStack() {
    delete [] elements;
}

template <typename T>
T DynamicStack<T>::pop() {
    if (size > 0) {
        return elements[--size];
    }
    else {
        assert(false && "Pop from empty stack");
    }
}

template <typename T>
bool DynamicStack<T>::contains(T value) {
    for (i32 i = 0; i < size; ++i) {
        if (elements[i] == value)
            return true;
    }
    return false;
}

template <typename T>
void DynamicStack<T>::push(T value) {
    if (size > capacity) {
        expandElements();
    }
    elements[size++] = value;
}

template <typename T>
void DynamicStack<T>::expandElements() {
    assert(size == capacity);
    capacity *= 2;
    T* old = elements; 
    elements = new T[capacity];
    memcpy(elements, old, size * sizeof(T));
    delete [] old;
}

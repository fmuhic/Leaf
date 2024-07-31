#pragma once

#include "types.h"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

template <typename T>
class ListPool {
    public:

    inline static i32 END = -1;

    struct Node {
        T value;
        i32 next;
    };

    ListPool(i32 initCapacity = 16);
    ~ListPool();

    i32 reserve(i32 amount);
    void free(i32 id);
    inline i32 next(i32 prev) {
        assert(prev != END);
        return nodes[prev].next;
    }
    // O(n) for now
    void removeElement(i32 listId, T elem);

    T& operator [] (i32 index) {
        assert(index >= 0 && index < capacity);
        return nodes[index].value;
    }

    void debug(std::string name);

    private:

    void expandPool(i32 minCapacity);

    Node* nodes = nullptr;
    i32 capacity;
    i32 size = 0;
    i32 freeList = 0;
};


template <typename T>
ListPool<T>::ListPool(i32 initCapacity): capacity(initCapacity) {
    assert(capacity > 0);
    nodes = new Node[capacity];

    for (i32 i = 0; i < capacity - 1; ++i) {
        nodes[i].next = i + 1;
    }

    nodes[capacity - 1].next = END;
}

template <typename T>
ListPool<T>::~ListPool() {
    delete [] nodes;
}

template <typename T>
i32 ListPool<T>::reserve(i32 amount) {
    if (size + amount > capacity) {
        expandPool(size + amount);
        assert(freeList != END);
    }

    i32 lastNode = freeList;
    for (i32 i = 0; i < amount - 1; ++i) {
        lastNode = nodes[lastNode].next;
    }

    i32 nodeId = freeList;
    freeList = nodes[lastNode].next;
    nodes[lastNode].next = END;
    size += amount;

    return nodeId;
}

template <typename T>
void ListPool<T>::expandPool(i32 minCapacity) {
    capacity *= 2;
    if (capacity < minCapacity)
        capacity = minCapacity;

    Node* old = nodes; 
    nodes = new Node[capacity];
    memcpy(nodes, old, size * sizeof(Node));
    delete [] old;

    for (i32 i = size; i < capacity - 1; ++i) {
        nodes[i].next = i + 1;
    }

    nodes[capacity - 1].next = END;
    freeList = size;
}

template <typename T>
void ListPool<T>::free(i32 id) {
    assert(id >= 0 && id < capacity);
	assert(size > 0);

    i32 lastNode = id;
    i32 amount = 1;
    while (nodes[lastNode].next != END) {
        lastNode = nodes[lastNode].next;
        ++amount;
    }

    nodes[lastNode].next = freeList;
    freeList = id;
    size -= amount;
    assert(size >= 0);
}

template <typename T>
void ListPool<T>::debug(std::string name) {
    std::cout << name << ": Capacity(" << capacity << "), Size(" << size << ")\n";
    i32 freeSize = 0;
    i32 iter = freeList;
    while (iter != END) {
        iter = nodes[iter].next;
        ++freeSize;
    }

    std::cout << name << ": Free(" << freeSize << ")\n";
    std::cout << "______________________________________________\n";
}

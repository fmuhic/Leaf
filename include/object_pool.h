#pragma once

#include "types.h"
#include <cassert>

template <typename T>
class ObjectPool {
    public:

    inline static i32 END = -1;

    struct Node {
        T value;
        i32 next;
    };

    ObjectPool(i32 initCapacity = 16);
    ~ObjectPool();

    i32 reserve();
    void free(i32 id);
    inline i32 next(i32 prev) {
        assert(prev != END);
        return nodes[prev].next;
    }

    T operator [] (i32 index) {
        assert(index >= 0 && index < capacity);
        return nodes[index];
    }

    private:

    void expandPool();

    Node* nodes = nullptr;
    i32 capacity;
    i32 size = 0;
    i32 freeList = 0;
    i32 elements = END;
};


template <typename T>
ObjectPool<T>::ObjectPool(i32 initCapacity): capacity(initCapacity) {
    assert(capacity > 0);
    nodes = new Node[capacity];

    for (i32 i = 0; i < capacity - 1; ++i) {
        nodes[i].next = i + 1;
    }

    nodes[capacity - 1].next = END;
}

template <typename T>
ObjectPool<T>::~ObjectPool() {
    delete nodes;
}

template <typename T>
i32 ObjectPool<T>::reserve() {
    if (freeList == END) {
        expandPool();
        assert(freeList != END);
    }

    i32 nodeId = freeList;
    freeList = nodes[freeList].next;
    nodes[nodeId].next = elements;
    elements = nodeId;
    ++size;

    return nodeId;
}

template <typename T>
void ObjectPool<T>::expandPool() {
    assert(size == capacity);

    capacity *= 2;
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
void ObjectPool<T>::free(i32 id) {
    assert(id >= 0 && id < capacity);
	assert(size > 0);

    elements = nodes[id].next;
	nodes[id].next = freeList;
	freeList = id;
	--size;
}

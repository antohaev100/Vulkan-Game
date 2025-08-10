#include "vulkan_game/shared_buffer.h"

// Generic manager functions for any dynamic array
void vectorAdd(vec *m, void* newElem) {
    if (m->n >= m->c) {
        m->c *= 2;
        m->array = realloc(m->array, m->c * m->elemSize);
    }
    memcpy((char*)m->array + (m->n * m->elemSize), newElem, m->elemSize);
    m->n++;
}

void vectorRem(vec* m,int index) {
    if (index >= m->n) return;
    
    // Shift elements
    char* base = m->array;
    memmove(base + (index * m->elemSize), 
            base + ((index + 1) * m->elemSize), 
            (m->n - index - 1) * m->elemSize);
    
    m->n--;
    if (m->n < m->c / 2 && m->c / 2 >= m->minc) {
        m->c /= 2;
        m->array = realloc(m->array, m->c * m->elemSize);
    }
}

void vectorCheckCapacity(vec *m) {
    if (m->n < m->c / 2 && m->c / 2 >= m->minc) {
        m->c /= 2;
        m->array = realloc(m->array, m->c * m->elemSize);
    }
}

void initVector(vec *m, int elemSize, int capacity, int minCapacity) { 
    m->array = malloc(elemSize * capacity);
    m->elemSize = elemSize;
    m->n = 0;
    m->c = capacity;
    m->minc = minCapacity;
}
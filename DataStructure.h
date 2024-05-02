#ifndef _DATASTRUCTURE_H_
#define _DATASTRUCTURE_H_

#define MAX_ELEMENT 20

typedef struct {
	int key;
}element;

typedef struct {
	element heap[MAX_ELEMENT];
	int heap_size;
}HeapType;


HeapType* create();
void init(HeapType* h);
void insert_max_heap(HeapType* h, element item);
element delete_max_heap(HeapType* h);
void heap_sort(element a[], int n);


#endif


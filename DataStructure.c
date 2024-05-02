//heap ���� �ڵ��?���� �� �κ�, ����
#include<stdio.h>
#include<stdlib.h>
#include "DataStructure.h"

#if 0
HeapType* create() {
	return (HeapType*)malloc(sizeof(HeapType));
}

void init(HeapType* h) {
	h->heap_size = 0;
}

void insert_max_heap(HeapType* h, element item) {
	int i;
	i = ++(h->heap_size);
	while ((item.key > h->heap[i / 2].key) && (i != 1)) {
		h->heap[i] = h->heap[i / 2];
		i = i / 2;
	}
	h->heap[i] = item;
}

element delete_max_heap(HeapType* h) {
	element item, temp;
	item = h->heap[1];
	temp = h->heap[h->heap_size];

	h->heap_size = h->heap_size - 1;

	int parent, child;
	parent = 1;
	child = 2;
	while (child <= h->heap_size) {
		if ((child < h->heap_size) && (h->heap[child].key < h->heap[child + 1].key))
			child++;
		if (temp.key >= h->heap[child].key)
			break;
		h->heap[parent] = h->heap[child];
		parent = child;
		child *= 2;
	}
	h->heap[parent] = temp;
	return item;
}
//heap ���� �ڵ��?���� �� �κ�, ��


//heap ���� �ڵ�
void heap_sort(element a[], int n) {
	int i;
	HeapType* h;
	h = create();
	init(h);

	//���� ������ �ְ�
	for (int i = 0; i < n; i++)
		insert_max_heap(h, a[i]);

	//�ִ񰪺��� �迭 ���ʿ� �ִ´�
	for (i = n - 1; i >= 0; i--) {
		a[i] = delete_max_heap(h);
	}
	free(h);
}
#endif
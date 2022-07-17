#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "huffmanEncoding.h"
#include "minHeap.h"

static void heapifyBotTop(struct Heap* h, int index){
    struct HuffmanTree* temp;
    int parent_node = (index-1)/2;
    if(((h->arr)[parent_node])->value > ((h->arr)[index])->value){
        //swap and recursive call
        temp = h->arr[parent_node];
        h->arr[parent_node] = h->arr[index];
        h->arr[index] = temp;
        
        heapifyBotTop(h,parent_node);
    }
}

static void insert(struct Heap* h, struct HuffmanTree* node){
    if( h->count < h->capacity){
        h->arr[h->count] = node;
        heapifyBotTop(h, h->count);
        h->count++;
    }
}

static struct Heap* newHeap(uint16_t nb_chars, struct HuffmanTree* nodes[nb_chars]){
    struct Heap* h = malloc(sizeof(struct Heap));
    //check if memory allocation is fails
    if(h == NULL){
        printf("Memory Error!");
        return NULL;
    }
    h->count = 0;
    h->capacity = (2*nb_chars) - 1;
    h->arr = malloc(h->capacity * sizeof(struct HuffmanTree)); //size in bytes

    //check if allocation succeed
    if ( h->arr == NULL){
        printf("Memory Error!");
        return NULL;
    }
    for (uint16_t i = 0; i < nb_chars; i++) {
        insert(h, nodes[i]);
    }
    return h;
}

static void heapifyTopBot(struct Heap* h, int parent_node){
    int left = parent_node*2+1;
    int right = parent_node*2+2;
    int min;
    struct HuffmanTree* temp = NULL;

    if(left >= h->count || left <0)
        left = -1;
    if(right >= h->count || right <0)
        right = -1;

    if(left != -1 && h->arr[left]->value < h->arr[parent_node]->value)
        min=left;
    else
        min =parent_node;
    if(right != -1 && h->arr[right]->value < h->arr[min]->value)
        min = right;

    if(min != parent_node){
        temp = h->arr[min];
        h->arr[min] = h->arr[parent_node];
        h->arr[parent_node] = temp;

        // recursive  call
        heapifyTopBot(h, min);
    }
}

static struct HuffmanTree* extractMin(struct Heap* h){
    struct HuffmanTree* pop;
    if(h->count==0){
        printf("\n__Heap is Empty__\n");
        return NULL;
    }
    // replace first node by last and delete last
    pop = h->arr[0];
    h->arr[0] = h->arr[h->count-1];
    h->count--;
    heapifyTopBot(h, 0);
    return pop;
}

struct HuffmanTree* buildTree(uint16_t nb_chars, struct HuffmanTree* nodes[nb_chars]) {
    struct Heap* heap = newHeap(nb_chars, nodes);
    struct HuffmanTree *left, *right, *parent;
    while (heap->count != 1) {
        left = extractMin(heap);
        right = extractMin(heap);
        parent = malloc(sizeof(struct HuffmanTree));
        parent->value = left->value + right->value;
        parent->left = left;
        parent->right = right;
        insert(heap, parent);
    }

    return extractMin(heap);
}
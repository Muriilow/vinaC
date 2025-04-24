#ifndef QUEUE_H
#define QUEUE_H

#include "vinaC.h"

//struct Node
//{
//    struct Node* next;
//    struct Node* prev;
//    struct Member* arquive;
//};

struct Queue
{
    struct Node* first;
    struct Node* last;
    int size;
};

void enqueue(struct Queue* q, struct Member* node);
struct Member* dequeue(struct Queue* q);
struct Node* createNode(struct Member* item);
struct Queue* createQueue();

#endif

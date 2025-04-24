#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

//struct Queue* createQueue()
//{
//    struct Queue* q = (struct Queue*) malloc(sizeof(struct Queue));
//
//    if(q == NULL)
//    {
//        printf("Error: Memory alocation\n");
//        return NULL;
//    }
//
//    q->size = 0;
//    q->last = NULL;
//    q->first = NULL;
//
//    return q;
//}
//
//struct Node* createNode(struct Member* item)
//{
//    struct Node* n = (struct Node*) malloc(sizeof(struct Node));
//    
//    if(n == NULL)
//    {
//        printf("Error: Memory alocation\n");
//        return NULL;
//    }
//
//    n->arquive = item;
//    n->prev = NULL;
//    n->next = NULL;
//
//    return n;
//}
//
//void enqueue(struct Queue* q, struct Member* item)
//{
//    struct Node* n = createNode(item);
//
//    if(q->last == NULL || q->first == NULL)
//    {
//        q->last = n;
//        q->first = n;
//        return;
//    }
//
//    n->next = q->last;
//    q->last->prev = n;
//    q->last = n;
//
//    q->size += 1;
//}
//
//struct Member* dequeue(struct Queue* q)
//{
//    struct Node* tmp;
//    struct Member* arquive;
//
//    if(q->last == NULL || q->first == NULL)
//        return NULL;
//
//    q->size -= 1;
//    arquive = q->first->arquive;
//
//    if(q->last == q->first)
//    {
//        free(q->last);
//
//        q->last = NULL;
//        q->first = NULL;
//
//        return arquive;
//    }
//    
//    tmp = q->first;
//
//    q->first = q->first->prev;
//    q->first->next = NULL;
//
//    free(tmp);
//    return arquive;
//}
//
//void destroyQueue(struct Queue* q)
//{
//    
//}

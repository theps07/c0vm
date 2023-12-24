#include <stdlib.h>        // Standard C library: free(), NULL...
#include <stdbool.h>       // Standard true, false, and bool type
#include "lib/contracts.h" // Our contracts library
#include "lib/xalloc.h"    // Our allocation library
#include "queue.h"         // The queue interface

typedef struct list_node list;
struct list_node
{
    void *data;
    list *next;
};

typedef struct queue_header queue;
struct queue_header
{
    list *front;
    list *back;
    size_t size;
};

bool is_inclusive_segment(list *start, list *end, size_t i)
{
    if (start == NULL && i == 0)
        return true;
    if (start == end && i != 1)
        return false;
    return is_inclusive_segment(start->next, end, i - 1);
}

bool is_acyclic(list *start)
{
    if (start == NULL)
        return true;

    list *slow = start;
    list *fast = start->next;

    while (fast != slow)
    {
        if (fast == NULL || fast->next == NULL)
            return true;
        fast = fast->next->next;
        //@assert slow != NULL;
        slow = slow->next;
    }

    //@assert fast == slow;

    return false;
}

bool is_queue(queue *Q)
{
    /* FIX ME */
    return Q != NULL && is_inclusive_segment(Q->front, Q->back, Q->size) && is_acyclic(Q->front);
}

void print_queue(queue *Q, print_data_fn *print_data)
//@requires is_queue(Q) && print_data != NULL;
{
    REQUIRES(is_queue(Q) && print_data != NULL);

    list *TMP = Q->front;

    for (size_t i = 0; i < Q->size; i++)
    {
        if (TMP == Q->front)
            printf("| FRONT |\n");

        printf("idx: %ld | data: ", i);

        (*print_data)(TMP->data);

        printf("\n");

        if (TMP == Q->back)
            printf("| BACK |\n");

        TMP = TMP->next;
    }

    return;
}

queue *queue_new()
//@ensures is_queue(\result);
{
    queue *new = xmalloc(sizeof(queue)); // MEMORY ALLOCATED; Free during Queue Free?
    new->front = NULL;
    new->back = NULL;
    new->size = 0;

    ENSURES(is_queue(new));

    return new;
}

size_t queue_size(queue *Q)
//@requires is_queue(Q);
//@ensures \result >= 0;
{
    REQUIRES(is_queue(Q));
    return Q->size;
}

void enq(queue *Q, void *x)
//@requires is_queue(Q);
//@ensures is_queue(Q);
{
    REQUIRES(is_queue(Q));

    list *newElem = xmalloc(sizeof(list)); // MEMORY ALLOCATED;
    newElem->data = x;
    newElem->next = NULL;

    if (Q->size == 0)
    {
        Q->front = newElem;
        Q->back = newElem;
    }
    else
    {

        Q->back->next = newElem;
        Q->back = newElem;
    }

    Q->size++;

    ENSURES(is_queue(Q));
}

/* O(1) -- removes an item from the front of the queue */
void *deq(queue *Q) // DEQ element must be freed?
//@requires is_queue(Q) && queue_size(Q) > 0;
//@ensures is_queue(Q) && queue_size(Q) >= 0;
{
    REQUIRES(is_queue(Q) && queue_size(Q) > 0);

    list *nodeToBeFreed = Q->front;
    void *res = nodeToBeFreed->data;

    if (Q->size == 1)
    {
        Q->front = NULL;
        Q->back = NULL;
    }
    else
    {
        Q->front = Q->front->next;
    }

    Q->size--;

    REQUIRES(is_queue(Q));

    free(nodeToBeFreed);

    return res;
}

void *queue_peek(queue *Q, size_t i)
//@requires is_queue(Q) && 0 <= i && i < queue_size(Q);
{
    REQUIRES(is_queue(Q) && i < queue_size(Q));

    list *TMP = Q->front;
    size_t n = i;

    while (n != 0)
    {
        TMP = TMP->next;
        n--;
    }

    return TMP->data;
}

void queue_reverse(queue *Q)
//@requires is_queue(Q);
//@ensures is_queue(Q);
{
    REQUIRES(is_queue(Q));

    list *loPtr = NULL;
    list *midPtr = Q->front;
    if (midPtr == NULL || midPtr->next == NULL)
        return;
    list *hiPtr = NULL;

    size_t i = 0;

    while (midPtr != NULL)
    //@loop_invariant is_inclusive_segment(loPtr, Q->front, i);
    {
        hiPtr = midPtr->next;
        midPtr->next = loPtr;
        loPtr = midPtr;
        midPtr = hiPtr;
        i++;
    }
    //@assert loPtr == Q->back;
    ASSERT(loPtr == Q->back);

    Q->back = Q->front;
    Q->front = loPtr;

    ENSURES(is_queue(Q));

    return;
}

bool queue_all(queue *Q, check_property_fn *P)
//@requires is_queue(Q) && P != NULL;
{
    REQUIRES(is_queue(Q) && P != NULL);

    if (Q->size == 0)
        return true;

    list *TMP = Q->front;

    for (size_t i = 0; i < Q->size; i++)
    {
        if (!(*P)(TMP->data))
            return false;
        TMP = TMP->next;
    }
    return true;
}

void *queue_iterate(queue *Q, void *base, iterate_fn *F)
//@requires is_queue(Q) != NULL && F != NULL;
{
    REQUIRES(is_queue(Q) && F != NULL);

    if (Q->size == 0)
        return base;
    void *res = base;
    list *TMP = Q->front;

    for (size_t i = 0; i < Q->size; i++)
    {
        res = (*F)(res, TMP->data);
        TMP = TMP->next;
    }

    return res;
}

void queue_free(queue *Q, free_fn *F)
{
    REQUIRES(is_queue(Q));

    list *freePtr = Q->front;

    while (freePtr != NULL)
    {
        list *toBeFreed = freePtr;
        freePtr = freePtr->next;
        if (F != NULL)
            (*F)(toBeFreed->data);
        free(toBeFreed);
    }

    free(Q);
}
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h> // for testing

#include "queue.h"

struct node {
    void* data;
    struct node *next;
};

struct queue {
    int size;
    struct node *head;
    struct node *tail;
};

queue_t queue_create(void)
{
    queue_t q = malloc(sizeof(*q));
    if (!q){
        return NULL;
    }

    q->size = 0;
    q->head = q->tail = NULL;

    return q;
}

int queue_destroy(queue_t queue)
{
    if (!queue || queue->size != 0){
        return -1;
    }

    free(queue);
    return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
    if (!queue || !data){
        return -1;
    }

	// Make a new node
    struct node *new_node = malloc(sizeof(struct node));
    if (!new_node){
        return -1;
    }

	// Configure the data and explicitly define pointer so we can properly use ->next logic later.
    new_node->data = data;
    new_node->next = NULL;

    if (queue->size == 0){
        // First element case
        queue->head = queue->tail = new_node;
    } else {
        queue->tail->next = new_node;
        queue->tail = new_node;
    }

    queue->size++;

    return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
    if (!queue || !data || queue->size == 0){
        return -1;
    }

    struct node *old_head = queue->head; // Tracking this cause we need to free node (otherwise no use)
    *data = old_head->data;

    queue->head = old_head->next; // Bridge nodes
    free(old_head);

    queue->size--;
    if (queue->size == 0){ // Special case if size is 0 now. Tail should be NULL.
        queue->tail = NULL;
    }

    return 0;
}

int queue_delete(queue_t queue, void *data)
{
    if (!queue || !data || queue->size == 0){
        return -1;
    }

	// Keep track of current and previous nodes in case delete in middle
    struct node *current = queue->head;
    struct node *prev = NULL;

    while (current){
		// Detect data and handle edge cases.
        if (current->data == data){
            if (prev){ // If prev means either found middle or tail
                prev->next = current->next; // Bridge gap
            } else {
                queue->head = current->next; // Otherwise you're head
            }
			// Confirms two cases: single node (both head and tail) or actually is last element.
            if (current == queue->tail){
                queue->tail = prev; // Prev becomes tail since current was deleted
				// If prev was NULL then that's fine. Now empty.
            }

            free(current);
            queue->size--;
            return 0;
        }
        prev = current;
        current = current->next;
    }

    return -1; // data not found
}

int queue_iterate(queue_t queue, queue_func_t func)
{
    if (!queue || !func || queue->size == 0) {
        return -1;
    }
	
	// Create a memory safe copy that allows delete
    int size = queue->size;
    void **copy = malloc(size * sizeof(void *));
    if (!copy) return -1;

	// Iterate over linked list queue to get copies
    struct node *n = queue->head;
    for (int i = 0; i < size && n; i++, n = n->next) {
        copy[i] = n->data;
    }

	// Do the func calling on each "safe" element
    for (int i = 0; i < size; i++) {
        func(queue, copy[i]);
    }

    free(copy);
    return 0;
}

// Trivial...
int queue_length(queue_t queue)
{
    if (!queue){
        return -1;
    }
    return queue->size;
}

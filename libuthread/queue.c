#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h> // for testing

#include "queue.h"

struct queue {
	int length; // maximum number of items this queue can support
	int size; // size of queue
	void** data; // item array
};

queue_t queue_create(void)
{
	queue_t q = malloc(sizeof(queue_t));

	if (!q){
		return NULL;
	}

	q->length = 10; // default queue supports 10 elements
	q->size = 0;
    q->data = malloc(q->length * sizeof(void*));

    if (!q->data) {
		free(q);
        return NULL;
    }
    return q;
}

int queue_destroy(queue_t queue)
{
	// Return -1 if queue is NULL or not empty
	if (!queue || queue->size != 0){
		return -1;
	}

	// Unreverse queue memory
	free(queue);

	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	// Return -1 if queue or data is NULL
	if (!queue || !data){
		return -1;
	}

	// Increment queue size for new item
	queue->size++;
	if (queue->size > queue->length){
		// TODO: revamp queue->data to be twice original size
	}else{
		queue->data[queue->size - 1] = data;
	}
	
	return 0;
}

int main(){ // for snippet testing.
	const queue_t q = queue_create();
	char* c = "hello!!";
	queue_enqueue(q,c);
	int status = queue_destroy(q);
	printf("%d\n",status);
	printf("%s\n",(char*)q->data[0]);
	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	/* TODO Phase 1 */
}

int queue_delete(queue_t queue, void *data)
{
	/* TODO Phase 1 */
}

int queue_iterate(queue_t queue, queue_func_t func)
{
	/* TODO Phase 1 */
}

int queue_length(queue_t queue)
{
	/* TODO Phase 1 */
}


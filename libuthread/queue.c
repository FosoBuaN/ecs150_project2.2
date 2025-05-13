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
	// Un-reserve queue data memory
	free(queue->data);

	// Un-reserve queue memory
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

		void** tmp = queue->data; // temporary backup pointer before dupe
		int old_length = queue->length;

		// double length and reserve more memory
		queue->length *= 2;
		queue->data = malloc(2 * queue->length * sizeof(void*));
		if (!queue->data){
			return -1;
		}
		/* copy old data to cover new bottom half. data size can use either length or size - 1
		   as they're the same value in this particular resize scope.
		*/
		memcpy(queue->data,tmp, queue->length * sizeof(void*));
		free(tmp); // release old memory buffer
	}
	
	queue->data[queue->size - 1] = data; // minus 1 for 0-index
	
	
	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	if (!queue || !data || queue -> size == 0){
		return -1;
	}

	queue->size--; // decrement queue size while remove item
	*data = queue->data[0]; // assign data the oldest item from queue (located at index 0)
	// pop queue
	memcpy(queue->data,queue->data + 1,queue->length * sizeof(void*)); 

	return 0;
}

int queue_delete(queue_t queue, void *data)
{
	if (!queue || !data){
		return -1;
	}

	int idx = 0;
	while (idx < queue->size){
		// check for same pointer
		if (queue->data[idx] == data){
			// decrement size
			queue->size--;
			// last element edge case:
			if (idx == queue->length - 1){
				queue->data[idx] = NULL;
			}else{
				int remainItemCt = queue->length - idx - 1;
				memcpy(queue->data + idx, queue->data + idx + 1, remainItemCt * sizeof(void*));
			}
			return 0;
		}
		idx++;
	}

	return -1; // not found data
}

int queue_iterate(queue_t queue, queue_func_t func)
{
	if (!queue || !func){
		return -1;
	}

	int size = queue->size;
	void** safe_copy = malloc(size * sizeof(void*));

	// create a copy of all elements for safe iteration
	for (int i = 0; i < size; ++i){
		safe_copy[i] = queue->data[i];
	}

	for (int i = 0; i < size; ++i){
		func(queue,safe_copy[i]);
	}
	

	return 0;
	
}

int queue_length(queue_t queue)
{
	if (!queue){
		return -1;
	}

	return queue->size;
}

int main(){ // for snippet testing.
	const queue_t q = queue_create();
	char* c = "hello!!";
	char* d = "sleepysleepysleepy!!!";
	char* e = "dog";
	char* f;
	queue_enqueue(q,c);
	queue_enqueue(q,d);
	queue_enqueue(q,c);
	queue_enqueue(q,c);
	queue_enqueue(q,c);
	queue_enqueue(q,c);
	queue_enqueue(q,c);
	queue_enqueue(q,c);
	queue_enqueue(q,c);
	queue_enqueue(q,c);
	queue_enqueue(q,c);
	queue_enqueue(q,e);
	int status = queue_destroy(q);
	printf("%d\n",status);
	printf("Expected: dog, Got: %s\n",(char*)q->data[11]);
	queue_dequeue(q,((void*) &f));
	printf("Expected: hello!!, Deque output: %s\n",f);
	printf("Expected: sleepysleepysleepy!!!, actual new_data[0]:%s\n",(char *)q->data[0]);
	queue_delete(q,d);
	printf("Expected: hello!!, actual new_data[0]:%s\n",(char*)q->data[0]);

	void callback(queue_t queue, void* data){
		printf("%s ",(char*)data);
	}
	queue_iterate(q,callback);
	printf("\n");
	void callback2(queue_t queue, void* data){
		queue_delete(queue,data);
		printf("Deleted %s\n",(char*) data);
	}
	queue_iterate(q,callback2);
	printf("Expected: (null), got data[0]: %s, Element Count: %d\n",(char*)q->data[0],q->size);
	printf("Expected: 0, got: %d\n",queue_length(q));
	return 0;
}


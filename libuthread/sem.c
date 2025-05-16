#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "private.h"
#include "sem.h"
#include "uthread.h"
#include <stdio.h> //for testing

struct semaphore {
	/* TODO Phase 3 */
	size_t count; //the number of resources we have available right now
	queue_t blocked_queue; //queue of threads that are waiting 
	
};

sem_t sem_create(size_t count)
{
	/* TODO Phase 3 */
	sem_t sem = (sem_t)malloc(sizeof(*sem)); //reserving memory for a new semaphore
	if(sem == NULL){ //if we can't get memory it failed so we return NULL
		return NULL; 
	}
	sem->count = count; //starting number of resources is the value user gives 
	sem->blocked_queue = queue_create(); //creating an empty queue to hold the threads that might need to wait at some point
	return sem; 
}

int sem_destroy(sem_t sem) //function to get rid of a semaphore and free up its memory
{
	/* TODO Phase 3 */
	if(sem == NULL){ //we can't destroy a NULL semaphore, so error happens here
		return(-1); 
	}
	queue_destroy(sem->blocked_queue); //destroying the queue of waiting threads 
	free(sem); //freeing the memory semaphore used 
	
	return 0;

}

int sem_down(sem_t sem)
{
	if(sem == NULL){ //if the semaphore doesn't exist we have an error 
		return(-1);
	}

	
	preempt_disable(); //preemption has to be disabled/turned off to check and change the count
	if(sem->count > 0){ 
		
		sem->count--;
		preempt_enable(); 
		return 0;
	}
	

	struct uthread_tcb * current_uthread = uthread_current(); //getting the current thread
	queue_enqueue(sem->blocked_queue, current_uthread); //placing it into the blocked queue
	uthread_block(); //blocking it so scheduler doesn't run it
	// uthread_yield(); //yielding control so another thread can run (uthread_block() already yields)
	preempt_enable(); // prevent issue where sem->count <= 0 and never reenable.
	
	return 0; // there's a chance threads dont ever reach this line, but adding so compiler don't complain

}

int sem_up(sem_t sem)
{
	/* TODO Phase 3 */
	if(sem == NULL){ 
		return(-1);
	}
	preempt_disable();
	if(queue_length(sem->blocked_queue) > 0){ //checking if there are any threads waiting 
		struct uthread_tcb *next_uthread = NULL;
		if(!queue_dequeue(sem->blocked_queue, (void**) &next_uthread)){//now we take one thread out of the blocked queue
			uthread_unblock(next_uthread); //marking it ready so it can run again
		}
	}else{
		sem->count++; //only increment if blocked queue empty because that's when we actually have resources.
	}
	preempt_enable();

	return 0; 
	
}


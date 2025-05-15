#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "private.h"
#include "sem.h"

struct semaphore {
	/* TODO Phase 3 */
	size_t count;
	queue_t blocked_queue; 
	
};

sem_t sem_create(size_t count)
{
	/* TODO Phase 3 */
	sem_t sem = (sem_t *)malloc(sizeof(struct semaphore));
	if(sem == NULL){
		return NULL; 
	}
	sem->count = count; 
	sem->blocked_queue = queue_create(); 
	return sem; 
}

int sem_destroy(sem_t sem)
{
	/* TODO Phase 3 */
	if(sem == NULL){
		return(-1); 
	}
	queue_destroy(sem->blocked_queue);

}

int sem_down(sem_t sem)
{
	if(sem == NULL){
		return(-1);
	}

	preempt_disable();

	if(sem->count > 0){
		sem->count--;
		preempt_enable(); 
		return 0;
	}
}

int sem_up(sem_t sem)
{
	/* TODO Phase 3 */
}


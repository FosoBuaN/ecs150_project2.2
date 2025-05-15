#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"

typedef enum{
	RUNNING,
	READY, 
	IDLE, 
	EXITED
} uthread_state;

struct uthread_tcb {
	/* TODO Phase 2 */
	uthread_ctx_t *context; 
	void *stack; 
	uthread_state state; 
};

static struct uthread_tcb *current_uthread = NULL; 

static queue_t thread_queue = NULL; 

struct uthread_tcb *uthread_current(void)
{
	/* TODO Phase 2/3 */
	return(current_uthread);
}

void uthread_yield(void)
{
	/* TODO Phase 2 */
	struct uthread_tcb *next_uthread, *prev_uthread;
	int length = queue_length(thread_queue);
	
	for(int i = 0; i < length; i++){
		if(queue_dequeue(thread_queue, &next_uthread) != 0){
			return;
		}
		if(next_uthread == NULL){
			return; 
		}
		if(next_uthread->state == READY){
			break; 
		}
		if(queue_enqueue(thread_queue, next_uthread) != 0){
			free(next_uthread);
			return;
		}

	}
	if(next_uthread == NULL){
		return;
	}
	if(next_uthread->state == IDLE){
		next_uthread->state == READY;
	}
	
	if(current_uthread->state == RUNNING){
		current_uthread->state = READY; 
	}
	queue_enqueue(thread_queue, current_uthread);
	prev_uthread = current_uthread;
	current_uthread = next_uthread;
	current_uthread->state = RUNNING; 

	uthread_ctx_switch(prev_uthread->context, next_uthread->context);
}

void uthread_exit(void)
{
	/* TODO Phase 2 */
	struct uthread_tcb *current = uthread_current();
	current->state = EXITED; 
	uthread_ctx_destroy_stack(current->stack);
	free(current);
	uthread_yield();
}

int uthread_create(uthread_func_t func, void *arg)
{
	/* TODO Phase 2 */
	struct uthread_tcb *new_thread = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
	if(new_thread == NULL){
		return(-1);
	}
	new_thread->stack = uthread_ctx_alloc_stack();
	if(new_thread->stack == NULL){
		free(new_thread);
		return(-1);
	}
	if(uthread_ctx_init(new_thread->context, new_thread->stack, func, arg) == -1){
		free(new_thread->stack);
		free(new_thread);
		return(-1);
	}

	new_thread->state = READY;
	if(queue_enqueue(thread_queue, new_thread) == -1){
		free(new_thread->context);
		free(new_thread->stack);
		free(new_thread);
		return(-1);
	}

	return 0; 

}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	/* TODO Phase 2 */
	thread_queue = queue_create(); 
	if(thread_queue == NULL){
		return(-1);
	}
	current_uthread = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
	if(current_uthread == NULL){
		queue_destroy(thread_queue);
		return(-1);
	}
	current_uthread->state = IDLE;

	if(uthread_create(func, arg) == -1){
		queue_destroy(thread_queue);
		return(-1);
	}
	while(queue_length(thread_queue) > 0){
		uthread_yield();
	}
	queue_destroy(thread_queue);
	free(current_uthread);
	
	return 0;



}

void uthread_block(void)
{
	/* TODO Phase 3 */
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	/* TODO Phase 3 */
}


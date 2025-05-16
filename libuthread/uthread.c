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
	BLOCKED, 
	EXITED //not sure if this needs to be here 
} uthread_state;

struct uthread_tcb {
	/* TODO Phase 2 */
	uthread_ctx_t *context; 
	void *stack; 
	uthread_state state; 
};

// This the thing that helps iterate
static struct uthread_tcb *current_uthread = NULL; 

// This is our main thread
static struct uthread_tcb *idle_thread = NULL;

// Let's have a queue for each state instead... less work lol
static queue_t ready_queue = NULL; 
static queue_t block_queue = NULL;
static queue_t exit_queue = NULL;

static bool loaded = false; // flag to assure all dependencies (queues) loaded

// Helper function to load required stuff
static void uthread_init(void){
	// Idle thread will be whoever first invokes init().
	idle_thread = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
	if (!idle_thread){
		exit(1);
	}
	// Configure idle context for backup
	idle_thread->context = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));
	if (!idle_thread->context){
		free(idle_thread);
		exit(1);
	}
	if (getcontext(idle_thread->context)){
		free(idle_thread->context);
		free(idle_thread);
		exit(1);
	}
	idle_thread->state = RUNNING; // Doesn't really need a state tbh.
	ready_queue = queue_create();
	block_queue = queue_create();
	exit_queue = queue_create();
	if (!ready_queue || !block_queue || !exit_queue){
		// since free(NULL) is safe...
		free(idle_thread);
		free(ready_queue);
		free(block_queue);
		free(exit_queue);
		exit(1);
	}
	current_uthread = idle_thread;

	loaded = true;
}

struct uthread_tcb *uthread_current(void)
{
	if (!loaded){
		exit(1); // y would user call if not loaded
	}
	/* TODO Phase 2/3 */
	return(current_uthread);
}

// callback function to clean EXIT and get stuff

void uthread_yield(void)
{
	if (!loaded){
		exit(1);
	}
	/* TODO Phase 2 */
	struct uthread_tcb *next_uthread, *prev_uthread;
	// int length = queue_length(thread_queue);
	prev_uthread = current_uthread;
	next_uthread = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
	// Check malloc error for next thread

	queue_dequeue(ready_queue,(void**)&next_uthread);
	// Check error for dequeue

	
	queue_enqueue(ready_queue,current_uthread); // Store current: a thread that can yield must be ready

	current_uthread = next_uthread; // Update current
	uthread_ctx_switch(prev_uthread->context,next_uthread->context);


	/*
		No longer need following logic since we have a separate queue for each state.
	*/
	// for(int i = 0; i < length; i++){
	// 	if(queue_dequeue(thread_queue, (void**) &next_uthread) != 0){
	// 		return;
	// 	}
	// 	if(next_uthread == NULL){
	// 		return; 
	// 	}
	// 	// if(next_uthread->state == READY){
	// 	// 	break; 
	// 	// }
	// 	if(queue_enqueue(thread_queue, next_uthread) != 0){
	// 		free(next_uthread);
	// 		return;
	// 	}

	// }
	// if(next_uthread == NULL){
	// 	return;
	// }
	// if(next_uthread->state == BLOCKED){
	// 	next_uthread->state = READY;
	// }
	
	// if(current_uthread->state == RUNNING){
	// 	current_uthread->state = READY; 
	// }
	// queue_enqueue(thread_queue, current_uthread);
	// prev_uthread = current_uthread;
	// current_uthread = next_uthread;
	// current_uthread->state = RUNNING; 

	// uthread_ctx_switch(prev_uthread->context, next_uthread->context);
}

void uthread_exit(void)
{
	if (!loaded){
		exit(1); // crash
	}
	/* TODO Phase 2 */
	struct uthread_tcb *current = uthread_current();
	current->state = EXITED; 
	/* Hmm... I think if you free current then there's really no need to mark EXITED even.
	In theory, you cannot do this here because you need current to help you swap context back
	to idle.
	*/
	uthread_ctx_destroy_stack(current->stack); // this might be fine. we're not coming back to this ctx.
	// free(current);
	// Go back to scheduler, not yield
	// uthread_yield();
	queue_enqueue(exit_queue,current); // add to exit queue
	uthread_ctx_switch(current->context,idle_thread->context);
}

int uthread_create(uthread_func_t func, void *arg)
{
	if (!loaded){
		uthread_init();
	}
	// Declares and reserves space for a new thread
	struct uthread_tcb *new_thread = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
	if(new_thread == NULL){
		return(-1);
	}
	// Grant a stack 
	new_thread->stack = uthread_ctx_alloc_stack();
	if(new_thread->stack == NULL){
		free(new_thread);
		return(-1);
	}
	// Grant some context (can do error check later for malloc)
	new_thread->context = (uthread_ctx_t*) malloc(sizeof(uthread_ctx_t));
	// Error check malloc
	if(uthread_ctx_init(new_thread->context, new_thread->stack, func, arg) == -1){
		free(new_thread->stack);
		free(new_thread);
		return(-1);
	}
	// Declares thread as ready
	new_thread->state = READY;

	// Puts thread on ready queue
	if(queue_enqueue(ready_queue, new_thread) == -1){
		free(new_thread->context);
		free(new_thread->stack);
		free(new_thread);
		return(-1);
	}

	return 0; 

}

/*static void uthread_idle(void *arg){
	while(1){
		uthread_yield();
	}
}*/


int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
    (void)preempt; // Skip error;

	if (!loaded){
		uthread_init();
	}
    printf("run here\n");
    // There are cases where users will call create() before run().
    // Cannot just initialize queue_create() here. Need more dynamic approach.
    // Doesn't matter atm for our testing.

    // Creates a new thread
    if(uthread_create(func, arg) == -1){
        queue_destroy(ready_queue);
        return(-1);
    }

    // While queue not empty yield thread.
    // Problem? Yeah, api should not forcefully yield user threads.
    /* Potential solution:
        Idle (current) -> new_thread right here.
        new_thread -> keep going in queue (user's problem we don't care too much)
        thread yield -> auto go next in queue
        thread exit -> come back to scheduler
    */
    while(queue_length(ready_queue) > 0){
        // uthread_yield();
        struct uthread_tcb *prev_thread = current_uthread;
        struct uthread_tcb *next_thread = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
        // Error check here TODO

        queue_dequeue(ready_queue,(void**)&next_thread);
        // Error check dequeue here

		// Before context switch need to make sure thread is ready

        current_uthread = next_thread;
        uthread_ctx_switch(prev_thread->context,next_thread->context);
        // Error check context switch here
    }
    
    queue_destroy(ready_queue);
	// Have some helper function here destroy everything.
    free(current_uthread);
    
    return 0;



}

void uthread_block(void)
{
	/* TODO Phase 3 */
	current_uthread->state = BLOCKED;
	uthread_yield();
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	/* TODO Phase 3 */
	if(uthread->state == BLOCKED){
		uthread->state = READY; 
		queue_enqueue(ready_queue, uthread);
	}
}
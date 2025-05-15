#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

/* @preempt_flag decides if preempt feature in-use.
   @preempt_state true if preempt enabled else false. NA if preempt not in use.
*/
static bool preempt_flag, preempt_state;

static struct sigaction old_action;
static struct itimerval old_timer;

static void preempt_signal_handler(int signum) {
    (void)signum;

    // Force current thread to yield
    uthread_yield();
}

void preempt_disable(void)
{
    // If preemption is already off, don't do anything
    if (!preempt_flag || !preempt_state) return;

    // Backup current signal handler for SIGVTALRM
    struct sigaction tmp_sa;
    memset(&tmp_sa, 0, sizeof(tmp_sa));   // Zero out the struct
    sigaction(SIGVTALRM, NULL, &tmp_sa);   // Get the current handler
    sigaction(SIGVTALRM, &old_action, NULL); // Restore the previous one (or normal one)
    old_action = tmp_sa;                  // Store current as old

    // Backup current virtual timer configuration
    struct itimerval tmp_tm;
    memset(&tmp_tm, 0, sizeof(tmp_tm));   
    getitimer(ITIMER_VIRTUAL, &tmp_tm);  
    setitimer(ITIMER_VIRTUAL, &old_timer, NULL);  
    old_timer = tmp_tm;                   

    preempt_state = false;
}


void preempt_enable(void)
{
	if (!preempt_flag || preempt_state) return;

	// Backup current signal handler for SIGVTALRM
    struct sigaction tmp_sa;
    memset(&tmp_sa, 0, sizeof(tmp_sa));   // Zero out the struct
    sigaction(SIGVTALRM, NULL, &tmp_sa);   // Get the current handler
    sigaction(SIGVTALRM, &old_action, NULL); // Restore the previous one (or handler one)
    old_action = tmp_sa;                  // Store current as old

    // Backup current virtual timer configuration
    struct itimerval tmp_tm;
    memset(&tmp_tm, 0, sizeof(tmp_tm));   
    getitimer(ITIMER_VIRTUAL, &tmp_tm);  
    setitimer(ITIMER_VIRTUAL, &old_timer, NULL);  
    old_timer = tmp_tm; 

	preempt_state = true;
}

void preempt_start(bool preempt)
{
	// Set universal feature enable flag
	preempt_flag = preempt;

	if (!preempt_flag) return;

	preempt_state = true;

	// ---- Set up timer and signals ----

	// Configure new sigaction
	struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = preempt_signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

	// Backup current action
	sigaction(SIGVTALRM, NULL, &old_action);

	// Set new action
    sigaction(SIGVTALRM, &sa, NULL);

	// actual timer setup
	struct itimerval timer;
	memset(&timer, 0, sizeof(timer));
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = HZ * 100;
    timer.it_value = timer.it_interval;

	// Backup original timer
    getitimer(ITIMER_VIRTUAL, &old_timer);
	// Set actual timer with HZ delay
    setitimer(ITIMER_VIRTUAL, &timer, NULL);
}

void preempt_stop(void)
{
	if (!preempt_flag) return;

	// Restore systems + shutdown preemption.
	sigaction(SIGVTALRM, &old_action, NULL);

    setitimer(ITIMER_VIRTUAL, &old_timer, NULL);

	preempt_flag = false;

}

// void fake_sleep(int seconds) {
//     struct timeval start, now;
    
//     // Get the current time
//     gettimeofday(&start, NULL);

//     // Keep running until the desired time has passed
//     do {
//         gettimeofday(&now, NULL);
//     } while (now.tv_sec - start.tv_sec < seconds); // Wait until the target time has passed
// }

// int main(void) {
//     // Start preemption (timer-based context switching)
//     preempt_start(true);

//     // Run the main loop (simulating the "running" state)
//     for (int i = 0; i < 5; i++) {
//         // Fake sleep for 1 second
//         printf("Main loop iteration %d\n", i + 1);
//         fake_sleep(1); // Simulate 1 second sleep
//     }

// 	preempt_disable();
// 	printf("Disabled preempt. Shouldn't see results for next 5 seconds\n");
// 	for (int i = 0; i < 5; i++) {
//         // Fake sleep for 1 second
//         printf("Main loop iteration %d\n", i + 1);
//         fake_sleep(1); // Simulate 1 second sleep
//     }

// 	preempt_enable();
// 	printf("Enabled preempt!\n");
// 	for (int i = 0; i < 5; i++) {
//         // Fake sleep for 1 second
//         printf("Main loop iteration %d\n", i + 1);
//         fake_sleep(1); // Simulate 1 second sleep
//     }
//     // Stop preemption after the test
//     preempt_stop();

//     return 0;
// }

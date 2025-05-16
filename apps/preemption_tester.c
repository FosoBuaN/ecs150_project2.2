#include <stdio.h>
#include <stdlib.h>
#include <uthread.h>
#include <sem.h>

#define N 5

sem_t sems[N];

void spin_thread(void *arg) {
	int id = (int)(long)arg;

	while (1) {
		printf("thread %d running\n", id);
	}
}

int main(void) {
	for (int i = 0; i < N; i++)
		sems[i] = sem_create(0);

	for (int i = 0; i < N; i++)
		uthread_create(spin_thread, (void*)(long)i);

	uthread_run(true, NULL, NULL); 

	for (int i = 0; i < N; i++)
		sem_destroy(sems[i]);

	return 0;
}

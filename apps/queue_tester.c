#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue.h>

#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)

/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
}

/* Destroy queue normal */
void test_queue_destroy(void){

    queue_t q;
    int status;

    fprintf(stderr, "*** TEST queue_destroy ***\n");

    q = queue_create();
    status = queue_destroy(q);

    TEST_ASSERT(status == 0);
}

/* Destroy queue fail (not empty) */
void test_queue_destroy_not_empty(void){

    queue_t q;
    int data = 3;
    int status;

    fprintf(stderr, "*** TEST queue_destroy_not_empty ***\n");

    q = queue_create();
    queue_enqueue(q, &data);
    status = queue_destroy(q);
    

    TEST_ASSERT(status == -1);
}

/* Destroy queue fail (queue null) */
void test_queue_destroy_null(void){

    queue_t q = NULL;
    int status;

    fprintf(stderr, "*** TEST queue_destroy_null ***\n");

    status = queue_destroy(q);
    

    TEST_ASSERT(status == -1);
}

int main(void)
{
	test_create();
	test_queue_simple();
    test_queue_destroy();
    test_queue_destroy_not_empty();
    test_queue_destroy_null();

	return 0;
}

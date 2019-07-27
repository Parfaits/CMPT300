// Test application for the process_ancestors syscall.

#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include "process_ancestors.h"

// Sys-call number:
#define _PROCESS_ANCESTORS_ 342

/**
 * Prototypes
 */
void test_positive_few(void);
void test_negative_few(void);
void test_many(void);
void test_bad_addr(void);
static void do_syscall_working(long size, long *num_filled);
static void do_syscall_failing(struct process_info info_array[], long size, long *num_filled, long ret_code);
static void test_internal(_Bool success, int lineNum, char* argStr);
static void test_print_summary(void);


/***********************************************************
 * main()
 ***********************************************************/
int main(int argc, char *argv[])
{
	printf("TESTING POSITIVE FEW\n");
	test_positive_few();
	// test_negative_few();
	// test_many();

	printf("TESTING BAD ADDR\n");
	test_bad_addr();

	test_print_summary();
	return 0;
}


/***********************************************************
 * Testing routines for specific test
 ***********************************************************/
void test_positive_few()
{
	do_syscall_working(1, (long*)0); // nothing filled
	do_syscall_working(1, (long*)1); // everything filled
	do_syscall_working(2, (long*)1); //partially filled
	do_syscall_working(56, (long*)10);
	do_syscall_working(765, (long*)765);
}
void test_negative_few()
{
	// do_syscall_working((long[]){-1}, 1);
	// do_syscall_working((long[]){-1, -2}, 2);
	// do_syscall_working((long[]){-1, 2, 3}, 3);
	// do_syscall_working((long[]){0, -2, 4, -6}, 4);
}

void fill(long data[], long size)
{
	for (int i = 0; i < size; i++) {
		data[i] = rand();
		if (i % 2 == 0) {
			data[i] *= -1;
		}
	}
}
#define MEG (1024*1024)
void test_many()
{
	for (int i = 1; i <= 5; i++) {
		long size = MEG * i;
		long *data = malloc(sizeof(data[0]) * size);
		fill(data, size);
		// do_syscall_working(data, size);
		free(data);
	}
}

void test_bad_addr()
{
	struct process_info info_array[0];
	// num_filled > size
	do_syscall_failing(info_array, 1, (long*)3, EFAULT);
	do_syscall_failing(info_array, 2, (long*)3, EFAULT);
	do_syscall_failing(info_array, 1, (long*)2147483647, EFAULT);

	// negative size 
	do_syscall_failing(info_array, -1, (long*)1,EINVAL);
	do_syscall_failing(info_array, -2385315, (long*)5,EINVAL);
	do_syscall_failing(info_array, 0, (long*)0,EINVAL);

	// Bad data pointers
	do_syscall_failing(NULL, 10, (long*)5,EFAULT);
	do_syscall_failing((struct process_info[]){1LL}, 5, (long*)2,EFAULT);
	do_syscall_failing((struct process_info[]){123456789012345689LL}, 5, (long*)2,EFAULT);
}



/***********************************************************
 * Custom testing framework
 ***********************************************************/
// Track results:
static int numTests = 0;
static int numTestPassed = 0;

static int current_syscall_test_num = 0;
static int last_syscall_test_num_failed = -1;
static int num_syscall_tests_failed = 0;

// Macro to allow us to get the line number, and argument's text:
#define TEST(arg) test_internal((arg), __LINE__, #arg)

// Actual function used to check success/failure:
static void test_internal(_Bool success, int lineNum, char* argStr)
{
	numTests++;
	if (!success) {
		if (current_syscall_test_num != last_syscall_test_num_failed) {
			last_syscall_test_num_failed = current_syscall_test_num;
			num_syscall_tests_failed++;
		}
		printf("-------> ERROR %4d: test on line %d failed: %s\n",
				numTestPassed, lineNum, argStr);
	} else {
		numTestPassed++;
	}
}

static void test_print_summary(void)
{
	printf("\nExecution finished.\n");
	printf("%4d/%d tests passed.\n", numTestPassed, numTests);
	printf("%4d/%d tests FAILED.\n", numTests - numTestPassed, numTests);
	printf("%4d/%d unique sys-call testing configurations FAILED.\n", num_syscall_tests_failed, current_syscall_test_num);
}


/***********************************************************
 * Routines to double check array answers
 ***********************************************************/
static long find_max(long data[], long size)
{
	long max = data[0];
	for (int i = 0; i < size; i++) {
		if (data[i] > max) {
			max = data[i];
		}
	}
	return max;
}
static long find_min(long data[], long size)
{
	long min = data[0];
	for (int i = 0; i < size; i++) {
		if (data[i] < min) {
			min = data[i];
		}
	}
	return min;
}
static long find_sum(long data[], long size)
{
	long sum = 0;
	for (int i = 0; i < size; i++) {
		sum += data[i];
	}
	return sum;
}

/***********************************************************
 * Functions to actually make the sys-call and test results
 ***********************************************************/
static int do_syscall(struct process_info info_array[], long size, long *num_filled)
{
	current_syscall_test_num++;
	printf("\nTest %d: ..Diving to kernel level\n", current_syscall_test_num);
	int result = syscall(_PROCESS_ANCESTORS_,info_array,size,num_filled);
	int my_errno = errno;
	printf("..Rising to user level w/ result = %d", result);
	if (result < 0) {
		printf(", errno = %d", my_errno);
	} else {
		my_errno = 0;
	}
	printf("\n");
	return my_errno;

}
static void do_syscall_working(long size, long *num_filled)
{
	struct process_info info_array[5];
	int result = do_syscall(info_array, size, num_filled);

	TEST(result == 0);
	// TEST(stats.min == find_min(data, size));
	// TEST(stats.max == find_max(data, size));
	// TEST(stats.sum == find_sum(data, size));
}
static void do_syscall_failing(struct process_info info_array[], long size, long *num_filled, long ret_code)
{
	int result = do_syscall(info_array, size, num_filled);
	TEST(result == ret_code);
}

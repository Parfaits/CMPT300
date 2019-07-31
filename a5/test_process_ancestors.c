// Test application for the process_ancestors syscall.

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include "process_ancestors.h"

#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/wait.h>

// Sys-call number:
#define _PROCESS_ANCESTORS_ 342


/**
 * Prototypes
 */
void test_GOOD(void);
void test_BAD(void);
void UID_TEST(void);
static void check_pid_name(struct process_info info_array[],long num_filled);
static void print(struct process_info info_array[],long num_filled);
static void do_syscall_working(long size);
static void do_syscall_failing(struct process_info info_array[], long size, long *num_filled, long ret_code);
static void test_internal(_Bool success, int lineNum, char* argStr);
static void test_print_summary(void);


/***********************************************************
 * main()
 ***********************************************************/
int main(int argc, char *argv[])
{
	// printf("TESTING POSITIVE FEW\n");
	
	pid_t pid = fork();
	if(pid == 0)
	{
		test_GOOD();
		test_BAD();
	}
	else
	{
		waitpid(-1,NULL,0);
		test_GOOD();
		test_BAD();

	}


	test_print_summary();
	return 0;
}


/***********************************************************
 * Testing routines for specific test
 ***********************************************************/
void test_GOOD()
{
	do_syscall_working(1);
	do_syscall_working(2);
	do_syscall_working(3);
	do_syscall_working(4);
	do_syscall_working(5);
	do_syscall_working(6);
}

void test_BAD()
{

	struct process_info *info_array = malloc(10*sizeof(struct process_info));
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

	// Test size bigger than allocated memory
	do_syscall_failing(info_array, 1000000, NULL, EFAULT);

	free(info_array);
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
	printf("===============================================================\n");
	printf("\nExecution finished.\n");
	printf("%4d/%d tests passed.\n", numTestPassed, numTests);
	printf("%4d/%d tests FAILED.\n", numTests - numTestPassed, numTests);
	printf("%4d/%d unique sys-call testing configurations FAILED.\n", num_syscall_tests_failed, current_syscall_test_num);
	printf("===============================================================\n");
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
static void do_syscall_working(long size)
{
	struct process_info info_array[size];
	long num_filled = -69420;
	if (setuid(69420) != 0) 
	{
	    exit(1);
	}
	int result = do_syscall(info_array, size, &num_filled);
	//test error code is 0
	TEST(result == 0);
	//test num filled > 0 and num filled <= size
	TEST(num_filled > 0);
	TEST(num_filled <= size);
	//Checking the pid values of the info array
	check_pid_name(info_array,num_filled);
	//Checking the uid values of the info array

	if(num_filled >= 1)
	{
		TEST(info_array[0].uid == 69420);
	}
	// Checking the name of process

	print(info_array,num_filled);
}
static void do_syscall_failing(struct process_info info_array[], long size, long *num_filled, long ret_code)
{
	int result = do_syscall(info_array, size, num_filled);
	TEST(result == ret_code);
}

static void check_pid_name(struct process_info info_array[],long num_filled)
{
	long swapper = 0;
	long pid = getpid();
	char temp[ANCESTOR_NAME_LEN];
	int i,j;
	// check if pid matches with info_array[0].pid
	TEST(pid == info_array[0].pid);
	// make sure other pid values in info array do not match with pid
	for (int i = 1; i < num_filled; ++i)
	{
		TEST(pid != info_array[i].pid);
	}
	//checking the name 
	for (j = 0; j < num_filled; ++j)
	{
		for (i = 0; i < ANCESTOR_NAME_LEN; ++i)
		{
			temp[i] = info_array[j].name[i];
		}
	}
	if(strcmp(temp,"swapper/0") == 0)
	{
		TEST(info_array[j-1].pid == swapper);
	}
}



/***********************************************************
 * Printing info array 
 ***********************************************************/

static void print(struct process_info info_array[],long num_filled)
{
	printf("%5s\t %5s\t %20s\t %5s\t %5s\t %5s\t %5s\t %10s\t %10s\t\n", "i", "pid", "name", "state", "uid", "nvcsw" , "nivcsw", "num_children", "num_siblings");
	for (int i = 0; i <  num_filled; ++i)
	{
		printf("%5d\t %5ld\t %20s\t %5ld\t %5ld\t %5ld\t %5ld\t %10ld\t %10ld\t\n", i,info_array[i].pid,info_array[i].name,info_array[i].state,info_array[i].uid,info_array[i].nvcsw,info_array[i].nivcsw,info_array[i].num_children,info_array[i].num_siblings);
	}
}

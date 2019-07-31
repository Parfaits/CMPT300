all: test_process_ancestors array_stats_test transfer

test_process_ancestors: test_process_ancestors.c
	gcc -std=c99 -D _GNU_SOURCE -static test_process_ancestors.c -o test_process_ancestors

array_stats_test: array_stats_test.c
	gcc -std=c99 -D _GNU_SOURCE -static array_stats_test.c -o array_stats_test

transfer:
	scp -P 2222 test_process_ancestors array_stats_test root@localhost:~

clean:
	rm -f *.o test_process_ancestors array_stats_test
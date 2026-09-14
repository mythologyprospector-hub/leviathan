#include <stdio.h>
#include <unistd.h>

__attribute__((noinline)) void
target_func(void)
{
	printf("target_func: running normally (breakpoint did NOT fire)\n");
}

int
main(void)
{
	printf("test_target: pid=%d, calling target_func()\n", getpid());
	target_func();
	printf("test_target: returned from target_func(), exiting normally\n");
	return 0;
}

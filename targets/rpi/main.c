#include <sys/resource.h>
#include <stdio.h>
#include <string.h>
#include "../../quake/quakedef.h"

extern int quake_main (int argc, char **argv, size_t memsize);

#define STACK_SIZE         (4 * 1024 * 1024)
#define HEAP_SIZE          (8 * 1024 * 1024)

static struct rlimit rl;

int main(int argc, char **argv)
{
    if (getrlimit(RLIMIT_STACK, &rl) == 0) {
        rl.rlim_cur = STACK_SIZE;
        if (setrlimit(RLIMIT_STACK, &rl) != 0) {
            printf("Could not resize stack\n");
            return -1;
        }
    }
    else {
    	printf("Could not get current stack stats\n");
    	return -1;
    }

	quake_main(argc, argv, HEAP_SIZE);
	return 0;
}

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>

pid_t make_fork(void)
{
	pid_t pid = fork();
	assert(pid != -1);
	return pid;
}
int main(int argc, char *argv[])
{
	char *path_i= argv[1], *path_o= argv[2], *path_k= argv[3];
	
	int i_pipe[2], k_pipe[2];

	assert(pipe(i_pipe) == 0);
	assert(pipe(k_pipe) == 0);

	if (make_fork() == 0) {
		assert(close(1) == 0);
		assert(dup(i_pipe[1]) == 1);
		assert(close(i_pipe[0]) == 0);
		assert(close(i_pipe[1]) == 0);
		assert(close(k_pipe[0]) == 0);
		assert(close(k_pipe[1]) == 0);
		assert(execlp("cat", "cat", path_i, NULL) != -1);
		exit(EXIT_SUCCESS);
	}

	if (make_fork() == 0) {
		assert(close(1) == 0);
		assert(dup(k_pipe[1]) == 1);
		assert(close(i_pipe[0]) == 0);
		assert(close(i_pipe[1]) == 0);
		assert(close(k_pipe[0]) == 0);
		assert(close(k_pipe[1]) == 0);
		assert(execlp("cat", "cat", path_k, NULL) != -1);
		exit(EXIT_SUCCESS);
	}

	assert(close(i_pipe[1]) == 0);
	assert(close(k_pipe[1]) == 0);

	int file = creat(path_o, 0700);
	assert(file != -1);

	while (1) {
		char i, k;

		if (1 != read(i_pipe[0], &i, 1))
			break;
		if (1 != read(k_pipe[0], &k, 1))
			break;
		i ^= k;
		assert(write(file, &i, 1) == 1);
	}
	assert(close(file) == 0);
	assert(close(i_pipe[0]) == 0);
	assert(close(k_pipe[0]) == 0);
	exit(EXIT_SUCCESS);
}
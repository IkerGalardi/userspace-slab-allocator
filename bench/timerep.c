#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/resource.h>

#define REPETITION_COUNT 10

static float timeval_to_sec(struct timeval time) {
    return (float)(time.tv_sec) + (float)(time.tv_usec) / 1000000;
}

float fork_wait_and_return_time(char** argv) {
    pid_t pid = fork();
    if(pid == 0) {
        char** child_argv = argv + 1;

        // Redirect stdout to /dev/null
        close(1);
        open("/dev/null", O_WRONLY);
        close(2);
        open("/dev/null", O_WRONLY);

        execvp(child_argv[0], child_argv);
        exit(1);
    }

    int child_status;
    struct rusage child_rusage;
    wait4(pid, &child_status, 0, &child_rusage);

    return timeval_to_sec(child_rusage.ru_utime) + timeval_to_sec(child_rusage.ru_stime);
}

int main(int argc, char** argv) {
    for(int i = 0; i < REPETITION_COUNT; i++) {
        float time = fork_wait_and_return_time(argv);
        printf("%f\n", time);
    }
}

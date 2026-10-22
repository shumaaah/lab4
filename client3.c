#include "net.h"
#include <sys/wait.h>
int main(int argc, char **argv) {
   if (argc != 2) {
       fprintf(stderr, "Usage: %s SERVER_IP\n", argv[0]);
       return 1;
   }
   signal(SIGPIPE, SIG_IGN);
   const long values[3] = {10, 20, 30};
   pid_t children[3];
   int count = 0, failed = 0;
   fflush(NULL);
   for (int i = 0; i < 3; ++i) {
       pid_t pid = fork();
       if (pid < 0) { perror("fork"); failed = 1; break; }
       if (pid == 0) {
           int rc = request(argv[1], values[i]);
           fflush(stdout);
           _exit(rc);
       }
       children[count++] = pid;
   }
   for (int i = 0; i < count; ++i) {
       int status;
       pid_t r;
       do { r = waitpid(children[i], &status, 0); }
       while (r < 0 && errno == EINTR);
       if (r < 0 || !WIFEXITED(status) ||
           WEXITSTATUS(status) != 0) failed = 1;
   }
   printf("Parent: %d children collected\n", count);
   return failed;
}

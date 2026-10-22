#include "net.h"
static void *worker(void *arg) {
   int fd = *(int *)arg;
   serve_one(fd);
   return NULL;
}
int main(void) {
   signal(SIGPIPE, SIG_IGN);
   setvbuf(stdout, NULL, _IONBF, 0);
   int listener = open_listener();
   printf("Server listening on port %d\n", PORT);
   for (;;) {
      int fd = accept(listener, NULL, NULL);
      if (fd < 0) {
          if (errno == EINTR) continue;
          die("accept");
      }
      pthread_t t;
      int rc = pthread_create(&t, NULL, worker, &fd);
      if (rc) { close(fd); pcheck(rc); }
      pcheck(pthread_join(t, NULL));
   }
}

#include "net.h"
#include <semaphore.h>
#define WORKERS 3
#define QSIZE 3
static int queue[QSIZE], in_pos, out_pos;
static sem_t empty_slots, ready_items;
static pthread_mutex_t qlock = PTHREAD_MUTEX_INITIALIZER;
static void swait(sem_t *s) {
   while (sem_wait(s) < 0) {
       if (errno != EINTR) die("sem_wait");
   }
}
static void spost(sem_t *s) {
   if (sem_post(s) < 0) die("sem_post");
}
static void *worker(void *arg) {
   int id = *(int *)arg;
   for (;;) {
       swait(&ready_items);
       pcheck(pthread_mutex_lock(&qlock));
       int fd = queue[out_pos];
       out_pos = (out_pos + 1) % QSIZE;
       pcheck(pthread_mutex_unlock(&qlock));
       spost(&empty_slots);
       printf("Worker %d tid=%lu START\n", id,
              (unsigned long)pthread_self());
       sleep(2); /* Demo delay: makes overlap visible. */
       serve_one(fd);
       printf("Worker %d END\n", id);
   }
   return NULL;
}
int main(void) {
   signal(SIGPIPE, SIG_IGN);
   setvbuf(stdout, NULL, _IONBF, 0);
   if (sem_init(&empty_slots, 0, QSIZE) < 0 ||
       sem_init(&ready_items, 0, 0) < 0) die("sem_init");
   int listener = open_listener();
   pthread_t threads[WORKERS];
   int ids[WORKERS] = {1, 2, 3};
   for (int i = 0; i < WORKERS; ++i)
       pcheck(pthread_create(&threads[i], NULL,
                        worker, &ids[i]));
   printf("3 workers listening on port %d\n", PORT);
   for (;;) {
       int fd = accept(listener, NULL, NULL);
       if (fd < 0) {
           if (errno == EINTR) continue;
           die("accept");
       }
       swait(&empty_slots);
       pcheck(pthread_mutex_lock(&qlock));
       queue[in_pos] = fd;
       in_pos = (in_pos + 1) % QSIZE;
       pcheck(pthread_mutex_unlock(&qlock));
       spost(&ready_items);
   }
}

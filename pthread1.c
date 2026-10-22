#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void check(int rc) {
   if (rc) { fprintf(stderr, "%s\n", strerror(rc)); exit(1); }
}
static void *print_message(void *arg) {
   printf("%s\n", (char *)arg);
   return NULL;
}
int main(void) {
   pthread_t t1, t2;
   int r1 = pthread_create(&t1, NULL, print_message,
                      "I am thread 1");
   check(r1);
   int r2 = pthread_create(&t2, NULL, print_message,
                      "I am thread 2");
   check(r2);
   check(pthread_join(t1, NULL));
   check(pthread_join(t2, NULL));
   printf("T1 & T2 return: %d, %d\n", r1, r2);
   return 0;
}

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int g_var1 = 0;
static void check(int rc) {
   if (rc) { fprintf(stderr, "%s\n", strerror(rc)); exit(1); }
}
static void *inc_gv(void *arg) {
   const char *name = arg;
   unsigned long tid = (unsigned long)pthread_self();
   for (int i = 0; i < 10; ++i) {
       g_var1++; /* Intentional data race for Practice B. */
       for (volatile int j = 0; j < 5000000; ++j) { }
       printf("%s tid=%lu iteration=%d value=%d\n",
             name, tid, i + 1, g_var1);
       fflush(stdout);
   }
   return NULL;
}
int main(void) {
   pthread_t ta, tb;
   check(pthread_create(&ta, NULL, inc_gv, "A"));
   check(pthread_create(&tb, NULL, inc_gv, "B"));
   check(pthread_join(ta, NULL));
   check(pthread_join(tb, NULL));
   printf("\nFinal = %d\npthread2 completed\n", g_var1);
   return 0;
}

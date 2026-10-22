#include "net.h"
int main(int argc, char **argv) {
   if (argc != 2) {
       fprintf(stderr, "Usage: %s SERVER_IP\n", argv[0]);
       return 1;
   }
   signal(SIGPIPE, SIG_IGN);
   char input[128];
   long n;
   printf("Enter integer (-1000000 to 1000000): ");
   fflush(stdout);
   if (!fgets(input, sizeof input, stdin) ||
       !strchr(input, '\n') || parse_number(input, &n) < 0) {
       fprintf(stderr, "Invalid input\n"); return 1;
   }
   return request(argv[1], n);
}

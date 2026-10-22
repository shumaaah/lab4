#ifndef LAB4_NET_H
#define LAB4_NET_H
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 5000
#define LIMIT 1000000L
static inline void die(const char *s) { perror(s); exit(1); }
static inline void pcheck(int rc) {
   if (rc) { fprintf(stderr, "%s\n", strerror(rc)); exit(1); }
}
/* A request or reply is one newline-terminated text line. */
static inline int send_all(int fd, const char *s) {
   size_t left = strlen(s);
   while (left) {
       ssize_t n = send(fd, s, left, 0);
       if (n < 0 && errno == EINTR) continue;
       if (n <= 0) return -1;
       s += n;
       left -= (size_t)n;
   }
   return 0;
}
static inline int read_line(int fd, char *buf, size_t cap) {
   size_t i = 0;
   while (i + 1 < cap) {
       char c;
       ssize_t n = recv(fd, &c, 1, 0);
       if (n < 0 && errno == EINTR) continue;
       if (n <= 0) return -1;
       if (c == '\n') { buf[i] = '\0'; return 0; }
       buf[i++] = c;
   }
   return -1;
}
static inline int parse_number(const char *s, long *v) {
   char *end;
   errno = 0;
   long n = strtol(s, &end, 10);
   if (s == end || errno) return -1;
   while (*end == ' ' || *end == '\t' || *end == '\r' ||
          *end == '\n') ++end;
   if (*end || n < -LIMIT || n > LIMIT) return -1;
   *v = n;
   return 0;
}
static inline int connect_to(const char *ip) {
   int fd = socket(AF_INET, SOCK_STREAM, 0);
   if (fd < 0) die("socket");
   struct sockaddr_in a = {0};
   a.sin_family = AF_INET;
   a.sin_port = htons(PORT);
   if (inet_pton(AF_INET, ip, &a.sin_addr) != 1) {
       fprintf(stderr, "Invalid IPv4 address\n"); exit(1);
   }
  if (connect(fd, (struct sockaddr *)&a, sizeof a) < 0)
      die("connect");
  return fd;
}
static inline int open_listener(void) {
   int fd = socket(AF_INET, SOCK_STREAM, 0), yes = 1;
   if (fd < 0) die("socket");
   if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
                &yes, sizeof yes) < 0) die("setsockopt");
   struct sockaddr_in a = {0};
   a.sin_family = AF_INET;
   a.sin_port = htons(PORT);
   a.sin_addr.s_addr = htonl(INADDR_ANY);
   if (bind(fd, (struct sockaddr *)&a, sizeof a) < 0)
       die("bind");
   if (listen(fd, 8) < 0) die("listen");
   return fd;
}
static inline void serve_one(int fd) {
   char line[128], reply[128];
   long n;
   if (read_line(fd, line, sizeof line) < 0) {
       fprintf(stderr, "Incomplete or oversized request\n");
   } else if (parse_number(line, &n) < 0) {
       if (send_all(fd, "ERROR invalid number\n") < 0)
           perror("send");
   } else {
       printf("Request %ld -> %ld\n", n, n * 5);
       snprintf(reply, sizeof reply, "%ld\n", n * 5);
       if (send_all(fd, reply) < 0) perror("send");
   }
   close(fd);
}
static inline int request(const char *ip, long n) {
   int fd = connect_to(ip);
   char line[128];
   snprintf(line, sizeof line, "%ld\n", n);
   if (send_all(fd, line) < 0 ||
       read_line(fd, line, sizeof line) < 0) {
       fprintf(stderr, "Communication failed\n");
       close(fd); return 1;
   }
   printf("PID %ld: sent %ld, received %s\n",
          (long)getpid(), n, line);
   close(fd);
   return 0;
}
#endif

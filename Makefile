CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -O0 -pthread
PI_TARGETS = pthread1 pthread1_nojoin pthread2 mutex1 server server3
UBUNTU_TARGETS = client client3

.PHONY: all pi ubuntu practice clean
all: pi ubuntu
pi: $(PI_TARGETS)
ubuntu: $(UBUNTU_TARGETS)
practice: pthread1 pthread1_nojoin pthread2 mutex1

pthread1 pthread1_nojoin pthread2 mutex1: %: %.c
	$(CC) $(CFLAGS) $< -o $@

server server3 client client3: %: %.c net.h
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(PI_TARGETS) $(UBUNTU_TARGETS) B_output.txt C_output.txt

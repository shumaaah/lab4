# SC3103 Lab 4 — Multithreaded C and TCP Socket Programming

Course practice code for SC3103 Exercise #4 and the 17-page **SC3103 Lab 4 当日課題マニュアル**.

## What runs where?

| Machine | Compile and run |
| --- | --- |
| Raspberry Pi (SSH: `pi@192.168.3.1`) | `pthread1`, `pthread1_nojoin`, `pthread2`, `mutex1`, `server`, `server3` |
| Ubuntu lab PC (local terminal) | `client`, `client3` |

Build separately on the Pi and Ubuntu PC: the binaries can target different CPU architectures.

## Lab-PC setup

For a **public** repository:
```bash
git clone https://github.com/shumaaah/lab4.git ~/ce3103/lab4_today
cd ~/ce3103/lab4_today
```

**This repository is currently private.** Anonymous cloning will not work; use an authorized method to retrieve it. Avoid leaving any GitHub access tokens, SSH keys, or login sessions on a shared lab PC. You can change repository visibility from GitHub Settings yourself if course rules permit public code.

From the **Ubuntu local terminal**, copy source files to Pi (where `192.168.3.1` is the Pi address):
```bash
ssh pi@192.168.3.1 'mkdir -p ~/ce3103/lab4_today'
scp *.c net.h Makefile pi@192.168.3.1:~/ce3103/lab4_today/
```

## Practice A–C — on Pi

```bash
ssh pi@192.168.3.1
cd ~/ce3103/lab4_today
make practice
./pthread1
./pthread1_nojoin
./pthread2 | tee B_output.txt
./mutex1 | tee C_output.txt
```

The two threads in A may print in either order. In B, `g_var1` deliberately has a data race; even a final 20 does not prove it is safe. In C, a mutex protects increment, delay and print, so the displayed values should increase 1–20.

## Standard server–client exercise

**Pi terminal**:
```bash
make pi
./server
```

**Ubuntu local terminal** (not inside Pi SSH):
```bash
make ubuntu
./client 192.168.3.1
# Enter 12 -> received 60
```

The server uses TCP port 5000, one newline-terminated integer per connection, and responds with 5 times the integer. The supported integer range is -1000000 to 1000000.

## Bonus: three client processes, three worker threads

Stop `./server` on the Pi with Ctrl+C first (both versions use port 5000).

**Pi terminal**: `./server3`

**Ubuntu local terminal**:
```bash
time ./client3 192.168.3.1
# Results: 50, 100, 150 (order may vary)
./client3 192.168.3.1
```

The bonus server uses 3 worker threads, a bounded queue, mutex and semaphores, and a 2-second per-request demo delay. The fork client sends 10, 20, 30 from three children, then the parent waits for them.

## Files

- `pthread1.c`: 2 threads + `pthread_join`
- `pthread1_nojoin.c`: comparison without joins
- `pthread2.c`: thread IDs and intentionally unsynchronized global counter
- `mutex1.c`: mutex-protected counter and printing
- `net.h`: shared socket, I/O, and parsing helpers
- `server.c` / `client.c`: basic number-times-five TCP service
- `server3.c` / `client3.c`: 3-worker server and 3-fork client
- `Makefile`: `make pi`, `make ubuntu`, `make practice`, `make clean`
- `.gitignore`: excludes machine-specific executables and logs

**Security / lab rules:** Use only on the approved lab network. The servers bind to all IPv4 interfaces. Confirm with the TA before changing managed network settings or publishing coursework.

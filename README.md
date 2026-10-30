# CampusCompute — Operating Systems Level 3 Mini-Project

C11 command-line prototype for Ubuntu/Linux. **Not a menu mock-up:** implements job management, three CPU schedulers, bounded POSIX producer-consumer logger, POSIX shared-memory IPC with named semaphore, Banker's safety algorithm, fork/waitpid worker monitoring, and RLIMIT_NOFILE demonstration.

## Requirements
Ubuntu or WSL Ubuntu; `gcc`, `make`, POSIX threads and IPC. On Ubuntu: `sudo apt update && sudo apt install build-essential git`.

## Build and run
```bash
make clean && make
./campuscompute --help
./campuscompute                # interactive CLI
make test                      # smoke tests
./campuscompute demo           # integrated demonstration
```

## Independently testable commands
```bash
./campuscompute fcfs
./campuscompute rr 2
./campuscompute priority
./campuscompute compare
./campuscompute logger
./campuscompute ipc
./campuscompute banker-safe
./campuscompute banker-unsafe
./campuscompute worker-ok
./campuscompute worker-fail
./campuscompute limit
./campuscompute demo
./campuscompute not-a-command  # should show usage and return nonzero
./campuscompute rr 0           # should reject invalid quantum
cat campuscompute.log
```

Sample jobs for noninteractive scheduling: J1(arrival=0, burst=5, priority=3), J2(2,3,1), J3(4,2,2). Smaller priority number means higher priority. FCFS ties: arrival then ID. Priority ties: priority then arrival then ID. Round Robin ties: arrival then ID; new arrivals enter the ready queue before the preempted job is requeued. Default RR quantum=2; override with `rr N` or interactive menu.

### Banker test cases
Classic 5-process, 3-resource Allocation and Maximum matrices are printed at runtime. Safe example uses Available=[3,3,2]; unsafe example uses Available=[0,0,0]. These are *different test states* for the same allocation/maximum matrices, not a simulated transition from one state to the other. Unsafe means no complete safe sequence, not proof of deadlock.

### Threading, IPC, and process behavior
Logger uses one producer, one consumer, a bounded queue of 4, mutex and two condition variables, and writes 12 event lines to `campuscompute.log`. IPC uses unique process-scoped POSIX shared-memory and named semaphore objects, then unlinks them after the child exits. Worker-fail is an **intentional simulated nonzero exit (42)**. Resource-limit demo runs in a child process, lowers its soft RLIMIT_NOFILE to at most 32, and attempts to open descriptors until EMFILE; parent is unaffected.

## Cleanup
`make clean` removes this project's executable, object files, and generated log. IPC objects are unlinked by the program; do not remove unrelated system IPC resources. The program does not remove any external files.

## Screenshots and submission
All terminal screenshots must show your **actual complete SRN in the terminal prompt**. Before taking screenshots in Ubuntu, set:
```bash
export SRN='REPLACE_WITH_YOUR_FULL_SRN'
export PS1='[$SRN] \w\$ '
```
Replace the placeholder with your actual SRN; do not submit the placeholder. Capture genuine S01–S24 screenshots as applicable, with commands, meaningful output, and one explanatory sentence each, then combine into a **single PDF named YOUR_ACTUAL_SRN.pdf**. No separate project report. Provide source, headers, Makefile, README and ZIP if required. GitHub is only a development convenience, not a required submission item.

## Limitations
The scheduling simulator models CPU bursts and lifecycle states; it does not spawn a real OS process per scheduled job. The logger is intentionally a small demonstrator. IPC and resource-limit demonstrations require Linux POSIX facilities. Complete Ubuntu testing and screenshot evidence must be performed on the student's own VM.

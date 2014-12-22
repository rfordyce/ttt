ttt
===

This project functions both as a way to familiarize myself with git and as a fast solver for the following problem:

> How many unique tic tac toe games exist, excluding rotations and symmetries?

A more detailed look at this problem may be found [here][1]

Compiling currently requires support for c++11 and POSIX threads (Linux).

```bash
make
```

---

Stage 1 - Initial Program
---

Initially the program used no threading and simply solved the problem.

Stage 2 - OpenMP
---

Next I added OpenMP's parallel for pragma (simply insert prior to for loops!)

```c++
#pragma omp parallel for
```

```bash
g++ ttt.cpp -fopemp -o omp_ttt
```

While this greatly improved runtime, it did not equally balance the load.  A new thread would be created for each processor of the computer and the 255168 games divided evenly amongst the threads.

If 6 threads are created, they split as follows:

1. Games 0 through 42527
2. Games 42528 through 85055
3. Games 85056 through 127583
4. Games 127584 through 170111
5. Games 170112 through 212639
6. Games 212640 through 255167

Due to the nature of my solution, each game is checked against each game preceeding it - therefore the higher the game index, the more games to be checked.

Stage 3 - std::thread
---

To solve this, I used very basic POSIX threading.
To create threads, I use the following structure:

```c++
#include <thread>

int mutex = 0;

FUNCTION (ARGUMENT)
	PURPUSE
	mutex--

MAIN
	std::vector<std::thread> threads;
	LOOP
		WHILE (badmutex >= MAXTHREADS) {}
		mutex++
		threads.push_back(std::thread(FUNCTION, ARGUMENTS));
	WHILE (mutex > 0) {}
```

Using both a variable to track the number of threads and wait until they complete, and a vector of threads as described [here][2]

Stage 4 - Cleanup/Final?
---

I intend to

1. Make threading optional such that this cn be compiled with s standard prior to c++11.
2. Test the difference in runtimes between versions.

I may also attempt to get this to run under Windows and/or add threading to the generation functions.

[1]: http://www.se16.info/hgb/tictactoe.htm
[2]: http://www.cplusplus.com/reference/thread/thread/thread/

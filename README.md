ttt
===

This project functions both as a way to familiarize myself with git and as a fast solver for the following problem:

> How many unique tic tac toe games exist, excluding rotations and symmetries?

A more detailed look at this problem may be found [here][1]

Compiling currently requires support for c++11 and POSIX threads (Linux).

```bash
git clone https://github.com/rfordyce/ttt.git
cd ./ttt
make
./m_ttt
```

Stage 1 - Initial Program
---

Initially the program used no threading and simply solved the problem.

Stage 2 - OpenMP Multithreading
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

Due to the nature of my solution, each game is checked against each game preceding it - therefore the higher the game index, the more games to be checked.

Stage 3 - Threading
---

CURRENT STAGE!

The program should evenly divide the reduction between the threads by starting each for loop at one-off the next and changing the increment to the number of threads.

Stage 4 - Cleanup/Final
---

I intend to

1. Make threading optional such that it can be compiled with different standards.
2. Make the program compile under different operating systems.
3. Test the difference in runtimes between versions and provide the results here.

[1]: http://www.se16.info/hgb/tictactoe.htm

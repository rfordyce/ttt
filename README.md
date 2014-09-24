ttt
===

This program functions both as a way to familiarize myself with git and as a fast solver for the following problem:
How many tic tac toe games exist, excluding rotations and symmetries.

A more detailed look at this problem may be found [here][1]


This is already functional code, and can be compiled under Linux as simply
```bash
g++ ttt.cpp -o ttt
```

It includes pragmas for parallelizing for loops with openmp (unknown pragmas should be ignored by the compiler)
```bash
g++ ttt.cpp -fopemp -o omp_ttt
```

While this improves runtime, it does not equally balance the load.  A new thread is created for each processor of the computer and the 255168 games are split equally amongst the threads.

If 6 threads are created, they split as follows
1. games 0 through 42527
2. games 42528 through 85055
3. games 85056 through 127583
4. games 127584 through 170111
5. games 170112 through 212639
6. games 212640 through 255167

Due to the nature of my solution, each game is checked against each game preceeding it - therefore the higher the game index, the more games to be checked.

To solve this, I would like to create a thread pool equal to the number of available cores and dynamically assign checking the next game in the list to the next available thread.

[1]: http://web.archive.org/web/20131030192316/http://www.mathrec.org/old/2002jan/solutions.html

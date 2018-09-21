Duplicate finder
=======================

This is an attempt to solve the programming assignment level 2 that
consists of creating an application that searches through a directory structure for duplicated files,
then outputs the result to 'output.txt'

# How to run

#### Linux / OS X

```bash
g++ main.cpp -o prog -std=c++17 -lstdc++fs && ./prog
```

#### Windows

```bash
g++ main.cpp -o prog -std=c++17 -lstdc++fs
prog.exe
```

#### Usage

Running the program without parameters will use the current path. If you want to run at a specific path do
`./prog <path>`


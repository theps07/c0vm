#!/bin/bash
# run with run chmod 700 tests.sh -> ./tests.sh

function task1 {
    
    for file in tests/task1/*.c0 
    do
        cc0 -b $file 
    done
    make
    for file in tests/task1/*.bc0
    do
        ./c0vmd $file 
    done

}

function task2 {
    
    for file in tests/task2/*.c0 
    do
        cc0 -b $file 
    done
    make
    for file in tests/task2/*.bc0
    do
        ./c0vmd $file 
    done

}

function task3 {
    
    for file in tests/task3/*.c0 
    do
        cc0 -b $file 
    done
    make
    for file in tests/task3/*.bc0
    do
        ./c0vmd $file 
    done

}

function task4 {
    
    for file in tests/task4/*.c0 
    do
        cc0 -b $file 
    done
    make
    for file in tests/task4/*.bc0
    do
        ./c0vmd $file 
    done

}

function task5 {
    
    for file in tests/task5/*.c0 
    do
        cc0 -b $file 
    done
    make
    for file in tests/task5/*.bc0
    do
        ./c0vmd $file 
    done

}

function task6 {
    
    for file in tests/task6/*.c1 
    do
        cc0 -b $file 
    done
    make
    for file in tests/task6/*.bc1
    do
        ./c0vm $file 
        valgrind --leak-check=full ./c0vm $file 
    done

}

function lab09 {

   cc0 -b -d tests/lab09/lib/*.o1 tests/lab09/rollcall.c1 tests/lab09/test-rollcall.c1
   make
   ./c0vmd tests/lab09/test-rollcall.bc1

}

function queues {

    cc0 -b -d -W tests/queues/queue.c1 tests/queues/queue-use.c1 tests/queues/queue-test.c1
    make
    ./c0vm tests/queues/queue-test.bc1

}

function peg {

    cc0 -b -d -W -w tests/peg/lib/peg-util.o1 tests/peg/lib/stacks.o1 tests/peg/lib/hdict.o1 tests/peg/peg-moves.c1 tests/peg/lib/sol-util.o1 tests/peg/peg2.c1 tests/peg/peg-main.c1
    make 
    ./c0vm tests/peg/peg-main.bc1 tests/peg/english.txt

}

for arg in "$@"; do
  if [[ "$arg" = -task1 ]]; then
    task1
  fi
  if [[ "$arg" = -task2 ]]; then
    task2
  fi
  if [[ "$arg" = -task3 ]]; then
    task3
  fi
  if [[ "$arg" = -task4 ]]; then
    task4
  fi
  if [[ "$arg" = -task5 ]]; then
    task5
  fi
  if [[ "$arg" = -task6 ]]; then
    task6
  fi
  if [[ "$arg" = -lab09 ]]; then
    lab09
  fi
  if [[ "$arg" = -queues ]]; then
    queues
  fi
  if [[ "$arg" = -peg ]]; then
    peg
  fi
done

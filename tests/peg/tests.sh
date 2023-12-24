#!/bin/bash
# run with run chmod 700 tests.sh -> ./tests.sh
#cc0 -d -W -w -o peg1 lib/peg-util.o1 lib/stacks.o1 peg-moves.c1 lib/sol-util.o1 peg1.c1 peg-main.c1
#cc0 -d -W -w -o peg2 lib/peg-util.o1 lib/stacks.o1 lib/hdict.o1 peg-moves.c1 lib/sol-util.o1 peg2.c1 peg-main.c1

: << COMMENT peg1 tests
./peg1 german.txt
./peg1 solvable1.txt
./peg1 solvable2.txt
./peg1 solvable3.txt
./peg1 solvable4.txt
./peg1 solvable5.txt
./peg1 unsolvable1.txt
./peg1 unsolvable2.txt
./peg1 unsolvable3.txt
./peg1 unsolvable4.txt
./peg1 unsolvable5.txt
./peg1 english.txt
COMMENT

: << COMMENT peg1 tests on peg2
./peg2 german.txt
./peg2 solvable1.txt
./peg2 solvable2.txt
./peg2 solvable3.txt
./peg2 solvable4.txt
./peg2 solvable5.txt
./peg2 unsolvable1.txt
./peg2 unsolvable2.txt
./peg2 unsolvable3.txt
./peg2 unsolvable4.txt
./peg2 unsolvable5.txt
./peg2 english.txt
./peg2 solvable6.txt
./peg2 solvable7.txt
./peg2 solvable8.txt
./peg2 solvable9.txt
COMMENT

: << COMMENT peg1 tests on peg2 testing French
cc0 -W -w -o peg2 -r unsafe -c-O2 lib/peg-util.o1 lib/stacks.o1 lib/hdict.o1 peg-moves.c1 lib/sol-util.o1 peg2.c1 peg-main.c1
./peg2 french1.txt
./peg2 french2.txt
./peg2 french3.txt
COMMENT

: << COMMENT dict profiling
cc0 lib/peg-util.o1 lib/stacks.o1 peg-moves.c1 peg-moves.c1 lib/sol-util.o1 lib/hdict.o1 peg2.c1 pegmark.c1
time ./a.out
COMMENT


#: << COMMENT full profiling
cc0 -r unsafe -c-O2 -c-pg -o peg2 lib/peg-util.o1 lib/stacks.o1 peg-moves.c1 peg-moves.c1 lib/sol-util.o1 lib/hdict.o1 peg2.c1 peg-main.c1
time ./peg2 french1.txt
#./peg2 french2.txt
#./peg2 french3.txt
gprof peg2 gmon.out > gprof.txt 
less gprof.txt
COMMENT
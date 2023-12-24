Peg Solitaire:

Files you won't modify:
   lib/peg-util.o1    - Utilities for reading and printing boards
   lib/hdict.o1       - Hash dictionaries
   lib/stacks.o1      - Stacks
   lib/sol-util.o1    - Utilities for verifying solutions
   peg-main.c1        - Main function, to read in boards and call solver

Files you may extend:
   peg-moves.c1       - Peg solitaire, representation of moves
   peg1.c1            - Peg solitaire, simple solver
   peg2.c1            - Peg solitaire, memoizing solver
   peg-moves-test.c1  - Test file for peg-moves.c1
                        (optional, highly recommended!)

Data:
   german.txt         - A trivial-to-solve board
   english.txt        - A board that you may be able to solve with peg2.c1 if
                        you pick a very good move selection strategy (see
                        Appendix A)
   french1.txt        - A difficult-to-solve board (but one with a solution)
   french2.txt        - A difficult-to-solve board (but one with a solution)
   french3.txt        - A difficult-to-solve-board (but one with a solution)

==========================================================

Displaying the library interfaces
   % cc0 -i lib/peg-util.o1
   % cc0 -i lib/stacks.o1
   % cc0 -i lib/sol-util.o1
   % cc0 -i lib/hdict.o1

Compiling and running tests for peg-moves.c1 (optional):
   % cc0 -d -W -w lib/peg-util.o1 lib/stacks.o1 peg-moves.c1 peg-moves-test.c1

Compiling and running on the German board with -d:
   % cc0 -d -W -w -o peg1 lib/peg-util.o1 lib/stacks.o1 peg-moves.c1 lib/sol-util.o1 peg1.c1 peg-main.c1
   % ./peg1 german.txt

   % cc0 -d -W -w -o peg2 lib/peg-util.o1 lib/stacks.o1 lib/hdict.o1 peg-moves.c1 lib/sol-util.o1 peg2.c1 peg-main.c1
   % ./peg2 german.txt

Compiling and running on the english board. These direction give you
the fastest possible code, but use at your own risk!
   % cc0 -W -w -o peg1 -r unsafe -c-O2 lib/peg-util.o1 lib/stacks.o1 peg-moves.c1 lib/sol-util.o1 peg1.c1 peg-main.c1
   % ./peg1 english.txt

   % cc0 -W -w -o peg2 -r unsafe -c-O2 lib/peg-util.o1 lib/stacks.o1 lib/hdict.o1 peg-moves.c1 lib/sol-util.o1 peg2.c1 peg-main.c1
   % ./peg2 english.txt

==========================================================

Submitting to the unofficial timeout checker from the command line on andrew:
   % autolab122 handin pegtests peg-moves.c1 peg1.c1 peg2.c1
then display autolab's feedback by running:
   % autolab122 feedback pegtests

Submitting officially from the command line on andrew:
   % autolab122 handin peg peg-moves.c1 peg1.c1 peg2.c1
then display autolab's feedback by running:
   % autolab122 feedback

Creating a tarball to submit with autolab.andrew.cmu.edu web interface:
   % tar -czvf handin.tgz peg-moves.c1 peg1.c1 peg2.c1

# Makefile
CC = gcc
OBJS = Solver.o main.o Parser.o History.o Game.o

EXEC = sudoku-console
COMP_FLAGS = -ansi -O3 -Wall -Wextra -Werror -pedantic-errors
GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(GUROBI_LIB) -o $@ -lm
Solver.o: Solver.c
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
main.o: main.c
	$(CC) $(COMP_FLAGS) -c $*.c
Game.o: Game.c
	$(CC) $(COMP_FLAGS) -c $*.c
Parser.o: Parser.c
	$(CC) $(COMP_FLAGS) -c $*.c
History.o: History.c
	$(CC) $(COMP_FLAGS) -c $*.c

clean:
	rm -f $(OBJS) $(EXEC) *~

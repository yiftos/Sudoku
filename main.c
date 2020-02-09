#include "Parser.h"
#include "History.h"
#include "Game.h"
#include "Solver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(){
    game * gm;
    gm =initGm();
    printf("Sudoku\n------\n");
 /*   SP_BUFF_SET();
    parseFile(gm,"blank.txt");*/
    play(gm);

	return 0;
}



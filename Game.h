/*
 * Game.h
 *
 *  Created on: Jul 29, 2018
 *      Author: benha
 */

#ifndef GAME_H_
#define GAME_H_


#include "Parser.h"
#include "History.h"

/*mode enum*/
typedef enum {
    INIT_MD,
    SOLVE_MD,
    EDIT_MD
} mode;

/*game structure*/
typedef struct {
	mode gmMode;
    int ** board;
    int ** fixedCells;
    int ** errCells;
    int markErr;
    int solved;
    int erroneous;
    movesList * history;
    int m; /*number of block rows */
    int n; /*number of block cols */
} game;


/* creates game board, fixed cells board and err cells board of size NXN and fills
 * all cells with 0
 */
void createBoard(game * gm, int N);

/*Initializes game struct to INIT mode with default game fields
 * returns initialized game
 */
game * initGm();

/*main function of game.c, recieves game and command and iterates until exit command*/
void play(game * gm);

/*recieves cmd and handles solve function*/
void solveHandler(game * gm, command * cmd);

/* returns 1 if puzzle solved successfully, 0 otherwise */
int setHandler(game * gm, command * cmd);

/*recieves cmd and handles edit function*/
void editHandler(game * gm, command * cmd);

/*recieves cmd and handles mark error*/
void markErrHandler(game * gm, command * cmd);

/*resets board*/
void resetHandler(game * gm);

/*prints board*/
void printBoard(game * gm);

/* parses game file, updates game values (block size, game board, game fixed cells board)
 * if can't open @path, returns 0, otherwise, returns 1
 */
int parseFile(game * gm, char * filePath);

/*check if all cells of the board are filled with values*/
int isLastCell(game * gm);

/*saves game to file in pathForSave*/
void saveToFile(game * gm, char * pathForSave);

/*handles save fanciot*/
void saveHandler(game * gm, command * cmd);

/*handles user input string*/
void handleToken(game * gm, int valuesRead, char * token);

/*recieves cmd and handles autofill function*/
void autofillHandler(game * gm);

/*copies game*/
game * cpyGame(game * gm);

#endif /* GAME_H_ */

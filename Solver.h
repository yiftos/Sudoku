/*
 * Solver.h
 *
 *  Created on: Sep 15, 2018
 *      Author: benha
 */

#ifndef SOLVER_H_
#define SOLVER_H_

#include "Game.h"

/*structure to by returned by ilp solver that states rather board is solvable and containing a feasible solution*/
typedef struct {
    int isFeas;
    int ** optBoard;
} ilpResult;

/*checks if val is valid for that cell*/
int validCell (game * gm, int col, int row, int val);

/*checks if val is valid for that collumn*/
int validCol(game * gm, int col, int row, int val);

/*checks if val is valid for that row*/
int validRow(game * gm, int col, int row, int val);

/*checks if val is valid for that block*/
int validBlock(game * gm, int col, int row, int val);

/*deterministic backtracking algorithm*/
int detBacktracking(game * gm);

/*tranform 1d array to 2d matrix*/
int arrToMat(double * solution, int N, int ** board);

/*filling x cells with valid values*/
int generateBoard(game * gm, int x);

/*removing y cells from board*/
void removeCells(game * gm, int y);

/*solves board using ilp and return ilpResult structure */
void ilpSolver(game * gm, ilpResult * res);

/*check if board is solvable using ilp*/
int validateHandler(game * gm, int isFromSave);

/*clears all cells*/
void clearBoard(game * gm);

/*recieves game and cell and returns a valid value for that cell */
void hintHandler(game * gm, command * cmd);

/*returns the number of possible solutions for current board */
void numSolHandler(game * gm);

/*generates a valid board using generateBoard, ilpSolver's optimal board and removeCells*/
void generateHandler(game * gm, command * cmd);

/*checks if board is valid for each cell*/
int validBoard(game * gm);












#endif /* SOLVER_H_ */

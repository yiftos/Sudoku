#include "Solver.h"
#include "Game.h"
#include "gurobi_c.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>


#define DEFAULT_ERR "ERROR: invalid command\n"


int validBoard(game * gm){
    int i;
    int j;
    int res;
    res=1;
    for (i=0; i<gm->m*gm->n; i++){
        for (j=0; j<gm->n*gm->m; j++){
            if (!validCell(gm, j, i, gm->board[i][j]) && gm->fixedCells[i][j] == 0){
                res=0;
                gm->errCells[i][j]=1;
            }
            else
                gm->errCells[i][j]=0;
        }
    }
    return res;
}


int validCell (game * gm, int col, int row, int val){
    if (val == 0)
        return 1;
    if (!(validCol(gm, col, row, val)) || !(validRow(gm, col, row, val)) || !(validBlock(gm, col, row, val))){
        return 0;
    }
    else return 1;
}


int validCol(game * gm, int col, int row, int val){
    int i;
    int curVal;
    int size;
    size = gm->m*gm->n;
    for (i=0; i<size; i++){
        curVal=gm->board[i][col];
        if (i!=row){
            if (curVal==val)
                return 0;
        }
    }
    return 1;
}


int validRow(game * gm, int col, int row, int val){
    int i;
    int curVal;
    int size = gm->n*gm->m;
    for (i=0; i<size; i++){
        curVal=gm->board[row][i];
        if (i!=col){
            if (curVal==val)
                return 0;
        }
    }
    return 1;
}

int validBlock(game * gm, int col, int row, int val){
    /* colBlocks and rowBlocks are numbered respectively  0...size-1/gm->n (OR gm->m)
     * i.e if size=9, gm->n=3, then blocks are numbered 0...2
     * so cell <4,1> belongs to colBlock 1, rowBlock 0
     * */
    int colBlock;
    int rowBlock;
    int colBlockStart;
    int rowBlockStart;
    int i;
    int j;
    colBlock=col/gm->n;
    rowBlock=row/gm->m;
    colBlockStart=colBlock*gm->n;
    rowBlockStart=rowBlock*gm->m;
    for (i=rowBlockStart; i<rowBlockStart+gm->m; i++){
        for (j=colBlockStart; j<colBlockStart+gm->n; j++){
            if (i!=row&&j!=col){
                if (gm->board[i][j]==val)
                    return 0;
            }
        }
    }
    return 1;
}

int detBacktracking(game * gm){
    int curVal;
    int col;
    int row;
    int assigned;
    int size;
    int i;
    int j;
    int numOfSolutions;
    int forward;
    game * solGame;
    size = gm->m*gm->n;
    
    solGame = cpyGame(gm);
    for (i=0; i<size; i++){
        for (j=0; j<size; j++){
            if(solGame->board[i][j]>0)
                solGame->fixedCells[i][j]=1;
        }
    }
    
    row=0;
    col=0;
    numOfSolutions=0;
    forward = 1;
    
    while(row>=0){
        if (col>size-1){
            row++;
            col=0;
        }
        if (col<0){
            col=size-1;
            row--;
        }
        if (row>size-1){ /*solved board*/
            numOfSolutions++;
            row--;
            col=size-1;
            forward=0;
        }
        if (row<0){
            break;
        }
        
        if (solGame->fixedCells[row][col] && forward){
            col++;
            continue;
        }
        if (solGame->fixedCells[row][col] && !forward){
            col--;
            continue;
        }
        curVal = solGame->board[row][col]+1;
        assigned = 0;
        while (curVal <= size && !assigned) {
            if (validCell(solGame, col, row, curVal)) {
                solGame->board[row][col]=curVal;
                assigned=1;
            }
            else
                curVal++;
        }
        if (assigned){ /* moving forward*/
            col++;
            forward=1;
        }
        else{ /* backtracking */
            solGame->board[row][col]=0;
            col--;
            forward=0;
        }
        
    }
    return numOfSolutions;
}

int arrToMat(double * solution, int N, int ** board){
    int v;
    int i;
    int j;
    for (v = 0; v < N; v++) {
        for (j = 0; j < N; j++) {
            for (i = 0; i < N; i++) {
                if(solution[i*N*N + j*N + v] == 1){
                    board[i][j] = v + 1;
                }
            }
        }
    }
    return 0;
}

int validateHandler(game * gm, int isFromSave){
    int i;
    int N;
    ilpResult * res;
    if ((gm->gmMode!=SOLVE_MD)&&(gm->gmMode !=EDIT_MD)){
        printf(DEFAULT_ERR);
        return 0;
    }
    
    if (gm->erroneous){
        printf("Error: board contains erroneous values\n");
        return 0;
    }
    
    N = gm->n * gm->m;
    res = (ilpResult *)malloc(sizeof(ilpResult));
    res->optBoard = malloc(sizeof(int *)*N);
    for (i=0; i<N; i++){
        res->optBoard[i] = malloc(sizeof(int)*N);
    }
    ilpSolver(gm,res);
    if(res->isFeas == 1){
        if (!isFromSave)
            printf("Validation passed: board is solvable\n");
        free(res);
        return 1;
    }
    else{
        if (!isFromSave)
            printf("Validation failed: board is unsolvable\n");
        else
            printf("Error: board validation failed\n");
        free(res);
        return 0;
    }
}

int generateBoard(game * gm, int x){
    int N;
    int i;
    int j;
    int ** indices;
    int randI;
    int randJ;
    int randV;
    int seed;
    int ** values;
    int valCnt;
    
    seed = time(NULL);
    srand(seed);
    N = gm->n * gm->m;
    values = calloc(x,sizeof(int *));
    for(i = 0; i < x; i++){
        values[i] = calloc(N,sizeof(int));
    }
    
    indices = malloc(N*sizeof(int *));
    for (i=0; i<N; i++){
        indices[i] = calloc(N,sizeof(int));
    }
    
    randI = rand()%N;
    randJ =rand()%N;
    for(i = 0; i < x; i++){
        valCnt = 0;
        for(j = 0; j < N; j++){
            valCnt += values[i][j];
        }
        if(valCnt == N){/*all N values were tried unsuccesfully*/
            free(indices);
            return 0;
        }
        else{
            while(indices[randI][randJ] == 1){/*choose an index that hasnt been chosen*/
                randI = rand()%N;
                randJ =rand()%N;
            }
            randV = rand()%(N)+1;
            while(values[i][randV-1] == 1){/*choose a value that hasnt been tried yet*/
                randV = rand()%(N)+1;
            }
            if(validCell(gm, randI, randJ, randV) == 1){/*if value is valid for that cell set it and move to next iteration*/
                gm->board[randJ][randI] = randV;
                values[i][randV - 1] = 1;
                indices[randI][randJ] = 1;
                
                continue;
            }
            else{/*if value isnt valid for that cell mark it as invalid and try again*/
                gm->board[randI][randJ] = 0;
                values[i][randV-1] = 1;
                i--;
            }
        }
    }
    free(indices);
    return 1;
}

void removeCells(game * gm, int y){
    int N;
    int i;
    int randI;
    int randJ;
    int seed;
    
    seed = time(NULL);
    srand(seed);
    N = gm->n * gm->n;
    for(i = 0; i < y; i++){

        randI = rand()%N;
        randJ = rand()%N;
        if(gm->board[randI][randJ] != 0){
            gm->board[randI][randJ] = 0;
            continue;
        }
        i--;
    }
}

void clearBoard(game * gm){
    int i;
    int j;
    int N = gm->m * gm->n;
    for(i = 0; i < N; i++){
        for(j = 0 ; j < N; j++){
            gm->board[i][j] = 0;
        }
    }
}

void generateHandler(game * gm, command * cmd){
    int i;
    int j;
    int N;
    int empty;
    int iters;
    ilpResult * res;
    
    N = gm->n * gm->n;
    empty = 0;
    for(i = 0; i < N; i++){
        for(j = 0 ; j < N; j++){
            if(gm->board[i][j] == 0)
                empty++;
        }
    }
    
    if (gm->gmMode!=EDIT_MD){
        printf(DEFAULT_ERR);
        return;
    }
    if (cmd->defaultError){
        printf(DEFAULT_ERR);
        return;
    }
    if (!cmd->valid||(cmd->x > empty)||(cmd->y > empty)){
        printf("Error: value not in range 0-%d\n",empty);
        return;
    }
    if (gm->erroneous){
        printf("Error: board contains erroneous values\n");
        return;
    }
    if(empty < N*N){
        printf("Error: board is not empty \n");
        return;
    }
    
    iters = 0;
    
    while(iters < 1000){
        if(generateBoard(gm,cmd->x) == 0){
            iters++;
            continue;
        }
        else{/*X cells are filled with valid values*/
            res = (ilpResult *)malloc(sizeof(ilpResult));
            res->optBoard = malloc(sizeof(int *)*N);
            for (i=0; i<N; i++){
                res->optBoard[i] = malloc(sizeof(int)*N);
            }
            ilpSolver(gm,res);
            if(!res->isFeas){/*generated board is not solvable*/
                iters++;
                clearBoard(gm);
                free(res->optBoard);
                free(res);
                continue;
            }
            for(i = 0 ; i < N; i++){/*copy solution from ilp solution to game board*/
                for (j = 0; j < N; j++){
                    gm->board[i][j] = res->optBoard[i][j];
                }
            }
            removeCells(gm,cmd->y);
            printBoard(gm);
            free(res->optBoard);
            free(res);
            return;
        }
    }    
    printf("Error: puzzle generator failed\n");
    return;
}

void ilpSolver(game * gm, ilpResult * res){
    GRBenv * env   = NULL;
    GRBmodel * model = NULL;
    int * ind;
    double * val;
    double * lb;
    char * vtype;
    char ** names;
    char * namestorage;
    char * cursor;
    int optimstatus;
    double * solution;
    int i, j, v, ig, jg, count;
    int error = 0;
    int N;
    
    N = gm->n * gm->m;
    solution = malloc(sizeof(double)*N*N*N);
    vtype = malloc(sizeof(char)*N*N*N);
    lb = malloc(sizeof(double)*(N*N*N)+1);
    names = malloc(sizeof(char *)*N*N*N);
    for (i=0; i<N*N*N; i++){
        names[i] = malloc(sizeof(char)*N*N*N);
    }
    ind = malloc(sizeof(int)*N);
    val = malloc(sizeof(double)*(N+1));
    namestorage = malloc(sizeof(char)*10*N*N*N);
    for(i = 0; i < N;i++){
        for(j = 0; j < N; j++){
            res->optBoard[i][j] = gm->board[i][j] - 1;
        }
    }
    
    /* Create an empty model */
    
    cursor = namestorage;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            for (v = 0; v < N; v++) {
                if (res->optBoard[i][j] == v)
                    lb[i*N*N+j*N+v] = 1;
                else
                    lb[i*N*N+j*N+v] = 0;
                vtype[i*N*N+j*N+v] = GRB_BINARY;
                
                names[i*N*N+j*N+v] = cursor;
                sprintf(names[i*N*N+j*N+v], "x[%d,%d,%d]", i, j, v+1);
                cursor += strlen(names[i*N*N+j*N+v]) + 1;
            }
        }
    }
    
    /* Create environment */
    
    error = GRBloadenv(&env, "sudoku.log");
    if (error) goto QUIT;
    
    error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
    if(error) goto QUIT;
    
    /* Create new model */
    
    error = GRBnewmodel(env, &model, "sudoku", N*N*N, NULL, lb, NULL,vtype, names);
    if (error) goto QUIT;
    
    /* Each cell gets a value */
    
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            for (v = 0; v < N; v++) {
                ind[v] = i*N*N + j*N + v;
                val[v] = 1.0;
            }
            error = GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1.0, NULL);
            if (error) goto QUIT;
        }
    }
    
    /* Each value must appear once in each row */
    
    for (v = 0; v < N; v++) {
        for (j = 0; j < N; j++) {
            for (i = 0; i < N; i++) {
                ind[i] = i*N*N + j*N + v;
                val[i] = 1.0;
            }
            error = GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1.0, NULL);
            if (error) goto QUIT;
        }
    }
    
    /* Each value must appear once in each column */
    
    for (v = 0; v < N; v++) {
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                ind[j] = i*N*N + j*N + v;
                val[j] = 1.0;
            }
            
            error = GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1.0, NULL);
            if (error) goto QUIT;
        }
    }
    
    /* Each value must appear once in each block */
    
    for (v = 0; v < N; v++) {
        for (ig = 0; ig < gm->n; ig++) {
            for (jg = 0; jg < gm->m; jg++) {
                count = 0;
                for (i = ig*gm->m; i < (ig+1)*gm->m; i++) {
                    for (j = jg*gm->n; j < (jg+1)*gm->n; j++) {
                        ind[count] = i*N*N + j*N + v;
                        val[count] = 1.0;
                        count++;
                    }
                }
                
                error = GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1.0, NULL);
                if (error) goto QUIT;
            }
        }
    }
    
    /* Optimize model */
    
    error = GRBoptimize(model);
    if (error) goto QUIT;
    
    /* Capture solution information */
    
    error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
    if (error) goto QUIT;
    
    if(optimstatus == 2){
        res->isFeas = 1;
        error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, N*N*N,solution);
        if (error) goto QUIT;
    }else{
        res->isFeas = 0;
    }
    
    arrToMat(solution, N, res->optBoard);
    
QUIT:
    
    /* Error reporting */
    
    if (error) {
        printf("ERROR: %s\n", GRBgeterrormsg(env));
        exit(1);
    }
    
    /* Free model */
    
    GRBfreemodel(model);
    
    /* Free environment */
    
    GRBfreeenv(env);
    
    /*Free memory*/
    
    free(solution);
    free(vtype);
    free(lb);
    free(ind);
    free(val);
    free(names);
    free(namestorage);
}

void hintHandler(game * gm, command * cmd){
    int col,row;
    ilpResult * res;
    int i;
    int N;
    
    N = gm->m * gm->n;
    res = (ilpResult *)malloc(sizeof(ilpResult));
    res->optBoard = malloc(sizeof(int *)*N);
    for (i=0; i<N; i++){
        res->optBoard[i] = malloc(sizeof(int)*N);
    }
    
    if (gm->gmMode!=SOLVE_MD){
        printf(DEFAULT_ERR);
        return;
    }
    if (cmd->defaultError){
        printf(DEFAULT_ERR);
        return;
    }
    if (!cmd->valid){
        printf("Error: value not in range 1-%d\n",gm->m*gm->n);
        return;
    }
    if (cmd->x > gm->n*gm->m || cmd->y > gm->n*gm->m){
        printf("Error: value not in range 1-%d\n",gm->m*gm->n);
        return;
    }
    if (gm->erroneous){
        printf("Error: board contains erroneous values\n");
        return;
    }
    row = cmd->y-1;
    col = cmd->x-1;
    if (gm->fixedCells[row][col]){
        printf("Error: cell is fixed\n");
        return;
    }
    if (gm->board[row][col]>0){
        printf("Error: cell already contains a value\n");
        return;
    }
    ilpSolver(gm,res);
    if(res->isFeas){
        printf("Hint: set cell to %d\n",res->optBoard[row][col]);
        free(res->optBoard);
        free(res);
        return;
    }
    else{
        printf("Error: board is unsolvable\n");
        free(res->optBoard);
        free(res);
        return;
    }
}

void numSolHandler(game * gm){
    int numSols;
    if (gm->gmMode==INIT_MD){
        printf(DEFAULT_ERR);
        return;
    }
    if (gm->erroneous){
        printf("Error: board contains erroneous values\n");
        return;
    }
    numSols = detBacktracking(gm);
    printf("Number of solutions: %d\n", numSols);
    if (numSols == 0){
        return;
    }
    if (numSols == 1)
        printf("This is a good board!\n");
    else{
        printf("The puzzle has more than 1 solution, try to edit it further\n");
    }
}








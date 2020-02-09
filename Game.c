#include "Game.h"
#include "Parser.h"
#include "History.h"
#include "Solver.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 1024
#define DEFAULT_BLOCK_SIZE 3
#define DEFAULT_ERR "ERROR: invalid command\n"

game * initGm(){
    game * gm=(game *)malloc(sizeof(game));
    if (!gm){
        printf("Malloc failed\n");
        return NULL;
    }
    /*createBoard(gm);*/
    gm->solved=0;
    gm->markErr=1;
    gm->erroneous=0;
    gm->gmMode=INIT_MD;
    gm->history=initList();
    return gm;
}

void solveHandler(game * gm, command * cmd){
    int prevMode;
    if (cmd->noPath){
        printf("Error: File doesn't exist or cannot be opened");
        return;
    }
    prevMode=gm->gmMode;
    gm->gmMode=SOLVE_MD;
    if (!parseFile(gm, cmd->path)){
        gm->gmMode=prevMode;
    }
    return;
}

void editHandler(game * gm, command * cmd){
    int prevMode;
    if (cmd->noPath){
        gm->m=DEFAULT_BLOCK_SIZE;
        gm->n=DEFAULT_BLOCK_SIZE;
        gm->gmMode = EDIT_MD;
        createBoard(gm, gm->n * gm->m);
        gm->markErr=1;
        return;
    }
    prevMode=gm->gmMode;
    gm->gmMode=EDIT_MD;
    if (!parseFile(gm, cmd->path)){
        gm->gmMode=prevMode;
        return;
    }
    gm->markErr=1;
    return;
}

void markErrHandler(game * gm, command * cmd){
    if (gm->gmMode!=SOLVE_MD){
        cmd->valid=0;
        cmd->defaultError=1;
    }
    if (cmd->valid==0){ /* invalid command but specific error */
        if (cmd->defaultError==1)
            printf(DEFAULT_ERR);
        else
            printf("Error: the value should be 0 or 1\n");
        return;
    }
    gm->markErr=cmd->x;
    return;
}

void printHandler(game * gm, command * cmd){
    if (gm->gmMode==INIT_MD){
        cmd->valid=0;
        cmd->defaultError=1;
    }
    if (cmd->valid==0){
        printf(DEFAULT_ERR);
        return;
    }
    printBoard(gm);
}

/*setMove params are always legal (values within 1-N)*/
void setMove(game * gm, int row, int col, int val){
    gm->board[row][col]=val;
    if (validBoard(gm))
        gm->erroneous=0;
    else
        gm->erroneous=1;
}

int isLastCell(game * gm){
    int i;
    int j;
    for(i = 0 ; i< gm->m * gm->n; i++){
        for(j = 0 ; j < gm->m * gm->n; j++){
            if(gm->board[i][j] == 0){
                return 0;
            }
        }
    }
    return 1;
}

int setHandler(game * gm, command * cmd){
    int col;
    int row;
    int val;
    if (gm->gmMode==INIT_MD){
        cmd->valid=0;
        cmd->defaultError=1;
    }
    if (cmd->valid==0){
        if (cmd->defaultError==1)
            printf(DEFAULT_ERR);
        else{ /*value not an integer, or negative integer*/
            printf("Error: value not in range 0-%d\n",(gm->m*gm->n));
        }
        return 0;
    }
    if (cmd->x > gm->m*gm->n ||  cmd->y > gm->m*gm->n || cmd->z > gm->m*gm->n){
        printf("Error: value not in range 0-%d\n",(gm->m*gm->n));
        return 0;
    }
    col = cmd->x-1;
    row = cmd->y-1;
    val = cmd->z;
    if (gm->fixedCells[row][col]==1){
        printf("Error: cell is fixed\n");
        return 0;
    }
    clearNextMoves(gm->history);
    insertMove(cmd->x, cmd->y, cmd->z, gm->board[row][col], gm->history); /*values in History are per x,y,z (col+1, row+1, value)*/
    setMove(gm, row, col, val); /* actually setting move on board */
    printBoard(gm);
    if(isLastCell(gm)){
        if(validBoard(gm)){
            printf("Puzzle solved successfully\n");
            return 1;
        }
        else{
            printf("Puzzle solution erroneous\n");
            return 0;
        }
    }
    return 0;
}


/*  returns 1 if undo was successful */
int undoHandler(game * gm, int isReset){
    char x;
    char y;
    char z1;
    char z2;
    if (gm->gmMode==INIT_MD){
        if (!isReset)
            printf(DEFAULT_ERR);
        return 0;
    }
    if (gm->history->cur->headNode==1){
        if (!isReset)
            printf("Error: no moves to undo\n");
        return 0;
    }
    setMove(gm,  (gm->history->cur->y)-1, (gm->history->cur->x)-1, gm->history->cur->oldVal);
    if (!isReset){
        printBoard(gm);
        x=gm->history->cur->x+'0';
        y=gm->history->cur->y+'0';
        z1=gm->history->cur->z+'0';
        z2=gm->history->cur->oldVal+'0';
        if (z1=='0')
            z1='_';
        if (z2=='0')
            z2='_';
        printf("Undo %c,%c: from %c to %c\n",x, y, z1, z2);
    }
    traverseBackward(gm->history);
    return 1;
}


void redoHandler(game * gm){
    char x;
    char y;
    char z1;
    char z2;
    if (gm->gmMode==INIT_MD){
        printf(DEFAULT_ERR);
        return;
    }
    if (!traverseForward(gm->history)) {
        printf("Error: no moves to redo\n");
        return;
    }
    printBoard(gm);
    x=gm->history->cur->x+'0';
    y=gm->history->cur->y+'0';
    z1=gm->history->cur->oldVal+'0';
    z2=gm->history->cur->z+'0';
    if (z1=='0')
        z1='_';
    if (z2=='0')
        z2='_';
    setMove(gm, (gm->history->cur->y)-1, (gm->history->cur->x)-1, gm->history->cur->z);
    printBoard(gm);
    printf("Redo %c,%c: from %c to %c\n",x, y, z1, z2);
}



void saveHandler(game * gm, command * cmd){
    if (!validBoard(gm))
        gm->erroneous=1;
    if (gm->gmMode==INIT_MD){
        printf(DEFAULT_ERR);
        return;
    }
    if ((gm->gmMode==EDIT_MD && gm->erroneous==1)){
        printf("Error: board contains erroneous values\n");
        return;
    }
    if(!validateHandler(gm,1))
        return;
    if (cmd->valid==0){
        printf("Error: File cannot be created or modified\n");
        return;
    }
    saveToFile(gm, cmd->savePath);
}

void resetHandler(game * gm){
    if (gm->gmMode==INIT_MD){
        printf(DEFAULT_ERR);
        return;
    }
    while (undoHandler(gm, 1)){
        ;
    }
    gm->history = initList();
    printf("Board Reset\n");
}




void autofill(game * gm){
    int i;
    int j;
    int size;
    int legalValuesCounter;
    int curVal;
    int legalVal;
    game * cpy = cpyGame(gm);
    size=gm->m*gm->n;
    for (i=0; i<size; i++){
        for (j=0; j<size; j++){
            if (cpy->board[i][j]==0){
                legalValuesCounter = 0;
                curVal = 1;
                while (curVal <= size && legalValuesCounter <=1){
                    if (validCell(cpy, j, i, curVal)){
                        legalValuesCounter++;
                        legalVal = curVal;
                    }
                    curVal++;
                }
                if (legalValuesCounter == 1){
                    setMove(gm, i, j, legalVal);
                    printf("Cell <%d,%d> set to %d\n", j+1, i+1, legalVal);
                }
            }
            
        }
    }
}

void autofillHandler(game * gm){
    if (gm->gmMode!=SOLVE_MD){
        printf(DEFAULT_ERR);
        return;
    }
    if (gm->erroneous){
        printf("Error: board contains erroneous values\n");
        return;
    }
    autofill(gm);
    printBoard(gm);
    return;
}

void saveToFile(game * gm, char * pathForSave){
    char * filePath;
    int i;
    int j;
    FILE * fp;
    filePath = pathForSave;
    fp = fopen(filePath, "w");
    if (!fp){
        printf("Error: File cannot be created or modified\n");
        return;
    }
    fprintf(fp, "%d %d", gm->m, gm->n);
    for (i=0; i<gm->m*gm->n; i++){
        fprintf(fp,"\n");
        for (j=0; j<gm->m*gm->n; j++){
            fprintf(fp,"%d", gm->board[i][j]);
            if ((gm->fixedCells[i][j]==1)||(gm->gmMode == EDIT_MD && gm->board[i][j]>0))
                fprintf(fp,".");
            fprintf(fp," ");
        }
    }
    fclose(fp);
    printf("Saved to: %s\n", filePath);
}



int parseFile(game * gm, char * filePath){
    char * token;
    int valuesRead; /*number of numerical characters read*/
    FILE * fp;
    char fileLine[MAX_LINE];
    fp = fopen(filePath, "r");
    if (!fp){
        if (gm->gmMode == EDIT_MD)
            printf("Error: File cannot be opened\n");
        else /*gmMode is SOLVE*/
            printf("Error: File doesnÕt exist or cannot be opened\n");
        return 0;
    }
    valuesRead = 0;
    
    while (fgets(fileLine,MAX_LINE,fp)) {
        if (valuesRead>1){
            token = strtok(fileLine, " \t\r\n");
            if (token){
                handleToken(gm, valuesRead, token);
                valuesRead++;
            }
        }
        if (valuesRead == 0) { /*reading m*/
            token = strtok(fileLine, " \t\r\n");
            gm->m = atoi(token);
            valuesRead++;
        }
        while ((token = strtok(NULL, " \t\r\n"))){
            if (valuesRead == 1){ /*reading n*/
                gm->n = atoi(token);
                valuesRead++;
                createBoard(gm, gm->n * gm->m);
            }
            else {
                handleToken(gm, valuesRead, token);
                valuesRead++;
            }
        }
    }
    fclose(fp);
    return 1;
}

void handleToken(game * gm, int valuesRead, char * token){
    int row;
    int col;
    row = (valuesRead - 2) / (gm->n * gm->m); /*formula to calculate current cell row*/
    col = (valuesRead - 2) % (gm->n * gm->m); /*formula to calculate current cell col*/
    
    if (strlen(token)>1){
        if (strlen(token)==2){
            if (token[1]=='.')
                gm->fixedCells[row][col]=1;
        }
        else{
            gm->fixedCells[row][col]=1;
        }
    }
    gm->board[row][col] = atoi( token );
    return;
}






void createBoard(game * gm, int N){
    int i;
    int j;
    /*dynamically allocating 2D arrays*/
    gm->board = malloc(sizeof(int *)*N);
    gm->fixedCells = malloc(sizeof(int *)*N);
    gm->errCells = malloc(sizeof(int *)*N);
    if (!gm->board || !gm->fixedCells || !gm->errCells){
        printf("Malloc failed\n");
        exit(0);
    }
    for (i=0; i<N; i++){
        gm->board[i] = malloc(sizeof(int)*N);
        gm->fixedCells[i] = malloc(sizeof(int)*N);
        gm->errCells[i] = malloc(sizeof(int)*N);
        if (!gm->board[i] || !gm->fixedCells[i] || !gm->errCells[i]){
            printf("Malloc failed\n");
            exit(0);
        }
    }
    
    for (i=0; i<N; i++){
        for (j=0; j<N; j++){
            gm->board[i][j]=0;
            gm->fixedCells[i][j]=0;
            gm->errCells[i][j]=0;
        }
    }
}



void printBoard(game * gm) {
    char * sepRow;
    int rowLen;
    int i;
    int j;
    int m;
    int n;
    m = gm->m;
    n = gm->n;
    rowLen = (4 * m * n) + m + 1;
    sepRow = malloc(sizeof(char) * (rowLen + 1));
    for (i = 0; i < rowLen; i++) {
        sepRow[i] = '-';
    }
    sepRow[i] = '\0';
    printf("%s\n", sepRow);
    for (i = 0; i < m * n; i++) {
        /*printing row*/
        printf("%c", '|');
        for (j = 0; j < m * n; j++) {
            /*printing cell*/
            printf(" ");
            if (gm->board[i][j] == 0)
                printf("  ");
            else {
                printf("%2d", gm->board[i][j]);
            }
            
            /*printing special character*/
            if (gm->fixedCells[i][j] == 1)
                printf("%c", '.');
            else if (gm->markErr == 1 && gm->errCells[i][j] == 1)
                printf("%c", '*');
            else
                printf(" ");
            
            if (j % (n) == n - 1)
                printf("%c", '|');
        }
        printf("\n");
        if (i % (m) == m - 1)
            printf("%s\n", sepRow);
    }
    free(sepRow);
}


game * cpyGame(game * gm){
    int i;
    int j;
    int size;
    game * newGm;
    newGm = (game *)malloc(sizeof(game));
    if (!newGm){
        printf("Error: malloc has failed\n");
        return NULL;
    }
    size=gm->m*gm->n;
    createBoard(newGm, size);
    for (i=0; i<size; i++){
        for (j=0; j<size; j++){
            newGm->board[i][j]=gm->board[i][j];
            newGm->fixedCells[i][j]=gm->fixedCells[i][j];
        }
    }
    newGm->solved=gm->solved;
    newGm->m=gm->m;
    newGm->n=gm->n;
    return newGm;
}



void play(game * gm){
   /* char pathForSave[256];  to fix a bug */
    command * cmd;
    printf("Enter your command:\n");
    cmd = initCommand();
    while (cmd->cmdName!=EXIT){
        if (cmd->cmdName==SOLVE){
            solveHandler(gm, cmd);
        }
        else if (cmd->cmdName==EDIT){
            editHandler(gm, cmd);
        }
        else if (cmd->cmdName==MARK_ERRORS)
            markErrHandler(gm, cmd);
        else if (cmd->cmdName==PRINT)
            printHandler(gm, cmd);
        else if (cmd->cmdName==SET){
            if (setHandler(gm, cmd))
                gm=initGm();
        }
        else if (cmd->cmdName==UNDO)
            undoHandler(gm, 0);
        else if (cmd->cmdName==REDO)
            redoHandler(gm);
        else if (cmd->cmdName==SAVE){
            saveHandler(gm, cmd);
        }
        else if (cmd->cmdName==RESET)
            resetHandler(gm);
        else if (cmd->cmdName==NUM_SOLUTIONS)
            numSolHandler(gm);
        else if (cmd->cmdName==HINT)
            hintHandler(gm, cmd);
        else if (cmd->cmdName==AUTOFILL){
            autofillHandler(gm);
        }
        else if (cmd->cmdName==VALIDATE){
            validateHandler(gm, 0);
        }
        else if (cmd->cmdName==GENERATE){
            generateHandler(gm, cmd);
        }
        else{
            printf(DEFAULT_ERR);
        }
        free(cmd);
        printf("Enter your command:\n");
        cmd = initCommand();
    }
    free(cmd);
    printf("Exiting...\n");
    free(gm);
}


/* TODO: in play, check if command is legal for current game mode.
 * TODO: also in play, check errors in values
 *  */

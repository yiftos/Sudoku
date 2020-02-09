/*
 * Parser.h
 *
 *  Created on: Aug 2, 2018
 *      Author: benha
 */

#ifndef PARSER_H_
#define PARSER_H_


/*command name enum*/
typedef enum {
	SOLVE,
	EDIT,
	MARK_ERRORS,
	PRINT,
    SET,
	VALIDATE,
	GENERATE,
	UNDO,
	REDO,
	SAVE,
    HINT,
	NUM_SOLUTIONS,
	AUTOFILL,
    RESET,
    EXIT,
	UNIDENTIFIED
} commandName;

/*command structure*/
typedef struct {
    commandName cmdName;
    int x,y,z;
    char path[256];
    char savePath[256];
    int noPath;
    int valid;
    int defaultError; /* if turned on, command is invalid and default error message should be printed*/
} command;

/*
 * dynamically allocating memory and returns initialized command
 */
command * initCommand();




#endif /* PARSER_H_ */

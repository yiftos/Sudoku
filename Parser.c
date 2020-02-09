#include "Parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CMD_LEN 256
#define MAX_ERR_LEN 256



command * initCommand(){
	char inputStr[MAX_CMD_LEN]; /*TODO: every command with len>256 is treated as invalid  */
	command * cmd;
	char * token;
	int intArg;

	cmd = (command *) malloc(sizeof(command));
	if (!cmd) {
		printf("Error: malloc has failed\n");
		exit(0);
	}
	cmd->defaultError = 0;
	cmd->valid = 1;
    cmd->noPath = 0;
    if(fgets(inputStr, MAX_CMD_LEN, stdin) == 0){
        printf("Error geting input string\n");
    }
	token = strtok(inputStr, " \t\r\n");
    
	if (strcmp(token,"solve")==0){
		cmd->cmdName=SOLVE;
		token = strtok(NULL, " \t\r\n");
        if (token){ /* if path is not NULL */
			/*cmd->path = (char *)(malloc(strlen(token)+1));
            cmd->path = token;*/
            strcpy(cmd->path, token);
        }
		else{ /* path is NULL*/
			cmd->valid = 0;
            cmd->noPath = 0;
		}
	}

	else if(strcmp(token, "edit")==0){
		cmd->cmdName=EDIT;
		token = strtok(NULL, " \t\r\n");
        if (token){ /* if path is not NULL */
            /*cmd->path = (char *)(malloc(strlen(token)+1));
             cmd->path = token;*/
            strcpy(cmd->path, token);
        }
		else /* path is NULL*/
            cmd->noPath = 1;
	}

	else if(strcmp(token, "mark_errors")==0){
		cmd->cmdName=MARK_ERRORS;
		token = strtok(NULL, " \t\r\n");
		if (token){ /*argument is not NULL*/
			if (strcmp(token,"0")==0){
				cmd->x=0;
			}
			else if (strcmp(token,"1")==0){
				cmd->x=1;
			}
			else{
				cmd->valid = 0;
			}
		}
		else{ /* argument is NULL*/
			cmd->valid = 0;
			cmd->defaultError = 1;
		}
	}

	else if(strcmp(token, "print_board")==0){
		cmd->cmdName=PRINT;
	}

	else if(strcmp(token, "set")==0){
		cmd->cmdName=SET;
		/*treating x*/
		token = strtok(NULL, " \t\r\n");
		if (!token){ /*not enough arguments*/
			cmd->valid = 0;
			cmd->defaultError = 1;
		}
		else{
			intArg = atoi(token);
			if (intArg<1)
				cmd->valid = 0;
			else
				cmd->x = intArg;
		}

		/*treating y*/
		token = strtok(NULL, " \t\r\n");
		if (!token){ /*not enough arguments*/
			cmd->valid = 0;
			cmd->defaultError = 1;
		}
		else{
			if (cmd->valid){
				intArg = atoi(token);
				if (intArg<1)
					cmd->valid = 0;
				else
					cmd->y = intArg;
			}
		}

		/*treating z*/
		token = strtok(NULL, " \t\r\n");
		if (!token){ /*not enough arguments*/
			cmd->valid = 0;
			cmd->defaultError = 1;
		}
		else{
			if (cmd->valid){
				intArg = atoi(token);
				if (strcmp(token,"0")==0) /* zero is allowed for z */
					cmd->z = intArg;
				else if (intArg<1)
					cmd->valid = 0;
				else
					cmd->z = intArg;
			}
		}

	}

	else if(strcmp(token, "validate")==0){
		cmd->cmdName=VALIDATE;
	}

	else if(strcmp(token, "generate")==0){
		cmd->cmdName=GENERATE;

		/*treating x*/
		token = strtok(NULL, " \t\r\n");
		if (!token){ /*not enough arguments*/
			cmd->valid = 0;
			cmd->defaultError = 1;
		}
		else{
			intArg = atoi(token);
			if (strcmp(token,"0")==0 || intArg > 0) /* x is allowed to be >= 0 */
				cmd->x = intArg;
			else
				cmd->valid = 0;
		}

		/*treating y*/
		token = strtok(NULL, " \t\r\n");
		if (!token){ /*not enough arguments*/
			cmd->valid = 0;
			cmd->defaultError = 1;
		}
		else if (cmd->valid){
			intArg = atoi(token);
			if (strcmp(token,"0")==0 || intArg > 0) /* y is allowed to be >= 0 */
				cmd->y = intArg;
			else
				cmd->valid = 0;
		}
	}

	else if(strcmp(token, "undo")==0){
		cmd->cmdName = UNDO;
	}

	else if(strcmp(token, "redo")==0){
		cmd->cmdName = REDO;
	}

	else if(strcmp(token, "save")==0){
		cmd->cmdName = SAVE;
		token = strtok(NULL, " \t\r\n");
        if (token){ /* if path is not NULL */
            /*cmd->path = (char *)(malloc(strlen(token)+1));
             cmd->path = token;*/
            strcpy(cmd->savePath, token);
        }
		else{ /* path is NULL*/
			cmd->valid = 0;
			cmd->defaultError = 1;
		}
	}

	else if(strcmp(token, "hint")==0){
		cmd->cmdName = HINT;


		/*treating x*/
		token = strtok(NULL, " \t\r\n");
		if (!token){ /*not enough arguments*/
			cmd->valid = 0;
			cmd->defaultError = 1;
		}
		else{
			intArg = atoi(token);
			if (intArg<1)
				cmd->valid = 0;
			else
				cmd->x = intArg;
		}

		/*treating y*/
		token = strtok(NULL, " \t\r\n");
		if (!token){ /*not enough arguments*/
			cmd->valid = 0;
			cmd->defaultError = 1;
		}
		else{
			intArg = atoi(token);
			if (intArg<1)
				cmd->valid = 0;
			else
				cmd->y = intArg;
		}
	}

	else if(strcmp(token, "num_solutions")==0){
		cmd->cmdName=NUM_SOLUTIONS;
	}

	else if(strcmp(token, "autofill")==0){
		cmd->cmdName=AUTOFILL;
	}

	else if(strcmp(token, "exit")==0){
		cmd->cmdName=EXIT;
	}

	else if(strcmp(token, "reset")==0){
		cmd->cmdName=RESET;
	}

	else{ /*command can't be identified*/
		cmd->cmdName=UNIDENTIFIED;
		cmd->valid=0;
		cmd->defaultError=1;
	}


	return cmd;


}

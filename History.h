
#ifndef HISTORY_H_
#define HISTORY_H_


/*linked list node structure*/
typedef struct node{
    int x,y,z;
    int oldVal; /* for undo*/
    int headNode; /*is current node the head node = empty node*/
    struct node * next;
    struct node * prev;
} movesNode;

/*linked list structure*/
typedef struct list{
	movesNode * cur;
	int numOfElements;
} movesList;

/* initializes movesNode struct ,and sets its x,y,z values.
 * returns node
 */
movesNode * initNode(int x, int y, int z, int oldVal);


/* initializes movesList struct
 * returns list
 */
movesList * initList();


/* advances current move pointer forward
 * returns 1 on success, 0 on failure (current member is last in the list)
 */
int traverseForward(movesList *);


/* advances current move pointer backward
 * returns 1 on success, 0 on failure (current member is first in the list)
 */
int traverseBackward(movesList *);

/*
 * returns 1 if lst has no members, 0 otherwise
 */
int isListEmpty(movesList *);

/*
 * creates new node with values x,y,z, updates new node as current node in lst
 */
void insertMove(int x, int y, int z, int oldVal, movesList * lst);

/*
 * clears all next moves (current move in lst becomes last : lst->cur->next = NULL)
 * frees memory of next moves
 */
void clearNextMoves(movesList * lst);


/*
 * clears list and frees memory
 */
void clearAllMoves(movesList * lst);


/*
 * prints History list (for testing purposes)
 */
void printLst(movesList * lst);

#endif /* HISTORY_H_ */

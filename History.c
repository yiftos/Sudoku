#include "History.h"
#include <stdlib.h>
#include <stdio.h>


movesNode * initNode(int x, int y, int z, int oldVal){
	movesNode * node = malloc(sizeof(movesNode));
	if (!node){
		printf("Malloc Failed\n");
		exit(0);
	}
	node->x=x;
	node->y=y;
	node->z=z;
	node->oldVal=oldVal;
	node->next=NULL;
	node->prev=NULL;
	node->headNode=0;
	return node;
}

movesList * initList(){
	movesList * lst = malloc(sizeof(movesList));
	if (!lst){
		printf("Malloc Failed\n");
		exit(0);
	}
	lst->cur=initNode(0,0,0,0);
	lst->cur->headNode=1;
	lst->numOfElements = 0;
	return lst;
}


int traverseForward(movesList * lst){
	if (isListEmpty(lst))
		return 0;
	if (!lst->cur->next) /* cannot traverse forward */
		return 0;
	lst->cur=lst->cur->next;
	return 1;
}

int traverseBackward(movesList * lst){
	if (lst->cur->headNode==1)
		return 0;
	lst->cur=lst->cur->prev;
	return 1;
}


int isListEmpty(movesList * lst){
	if (lst->numOfElements==0)
		return 1;
	return 0;
}

void insertMove(int x, int y, int z, int oldVal, movesList * lst){
	movesNode * newNode = initNode(x, y, z, oldVal);
	lst->cur->next=newNode;
	newNode->prev=lst->cur;
	lst->cur=newNode;
	lst->numOfElements++;
	return;
}

void clearNextMoves(movesList * lst){
	movesNode * tmp = lst->cur;
	if (isListEmpty(lst))
		return;
	if (!lst->cur->next)
		return;
	while(tmp->next){ /*moving tmp to end of list*/
		tmp=tmp->next;
		lst->numOfElements--;
	}
	while(tmp->prev!=lst->cur){
		tmp=tmp->prev;
		tmp->next=NULL;
	}
	lst->cur->next=NULL;
	return;
}

void clearAllMoves(movesList * lst){
	while (traverseBackward(lst));
	clearNextMoves(lst);
	free(lst->cur);
	lst=initList();
	return;
}

void printLst(movesList * lst){
	int i;
	movesNode * tmp;
	if (isListEmpty(lst)){
		printf("list is empty!\n");
		return;
	}
	tmp = lst->cur;
	while (tmp->prev){
		tmp=tmp->prev;
	}
	i=0;
	while (tmp){
		printf("i:%d\n",i);
		printf("x:%d, y:%d, z:%d, oldVal:%d, headNode:%d\n",tmp->x,tmp->y,tmp->z, tmp->oldVal, tmp->headNode);
		if (tmp==lst->cur)
			printf("^---CUR---\n");
		tmp=tmp->next;
		i++;
	}
}


#include <stdio.h>
#include <stdlib.h>

#define MAX_LEVEL 10

typedef enum { false,true} bool;


typedef struct nodeStructure
{
	int key;
	int value;
	struct nodeStructure * forward[1];
}nodeStructure;

typedef struct skiplist
{
	int level;
	nodeStructure *head;
}skiplist;

//createNode
nodeStructure *createNode(int level,int key,int value)
{
	nodeStructure *ns = (nodeStructure *)malloc(sizeof(nodeStructure) + level * sizeof(nodeStructure *));
	ns->key = key;
	ns->value = value;
	return ns;
}

skiplist* createSkiplist()
{
	skiplist *sl = (skiplist *)malloc(sizeof(skiplist));
	sl->level = 0;
	sl->head = createNode(MAX_LEVEL-1,0,0);
	for (int i = 0; i < MAX_LEVEL ; i++)
	{
		sl->head->forward[i] = NULL;
	}
	return sl;
}

int randomLevel()
{
	int k = 1;
	while (rand() % 2)
		k++;
	k = (k < MAX_LEVEL) ? k : MAX_LEVEL;
	return k;
}


int main()
{
	return 0;
}
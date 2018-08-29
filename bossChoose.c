#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define Name typedef struct

typedef enum{ATTACK, HEAL, DEFEND} move_type;
typedef enum{FALSE, TRUE} bool;

Name intrv_{
	int low;
	int high;
	int index;
	move_type myMove;
}Interval;

Name node_{
	Interval range;
	int max;
	struct node_ *left;
	struct node_ *right;
}IntNode;

IntNode *opponent, *boss;
Interval *ranges[2];
int indx=0;

move_type setMoveMyTree(int maxhealth, int currenthealth){//send the starting health value and the lower health value
	int SEVENTYFIVE=0.75*maxhealth, FIFTY=0.5*maxhealth, THIRTYFIVE=0.35*maxhealth, TWENTYFIVE=0.25*maxhealth, FIFTEEN=0.15*maxhealth; //truncates all values
	
	if(currenthealth<=maxhealth && currenthealth>SEVENTYFIVE) return ATTACK;
	else if(currenthealth<=SEVENTYFIVE && currenthealth>FIFTY) return rand()%2;//return either heal or attack
	else if(currenthealth<=FIFTY && currenthealth>THIRTYFIVE) return rand()%3;//return heal, attack, or defend
	else if(currenthealth<=THIRTYFIVE && currenthealth>TWENTYFIVE) return DEFEND;//return defend
	else if(currenthealth<=TWENTYFIVE && currenthealth>FIFTEEN) return (1+rand()%2);//return heal or defend
	else return HEAL; //if less than 15% health, always try to heal				
}

move_type setMoveTheirTree(int maxhealth, int currenthealth){
	int SEVENTYFIVE=0.75*maxhealth, FIFTY=0.5*maxhealth, THIRTYFIVE=0.35*maxhealth, TWENTYFIVE=0.25*maxhealth, FIFTEEN=0.15*maxhealth; //truncates all values	
	
	if(currenthealth<=maxhealth && currenthealth>SEVENTYFIVE) return ATTACK;//if they have full health, attack
	else if(currenthealth<=SEVENTYFIVE && currenthealth>FIFTY) return rand()%3;//return heal, attack, or defend
	else if(currenthealth<=FIFTY && currenthealth>THIRTYFIVE) return (1+rand()%2);//return either heal or defend
	else if(currenthealth<=THIRTYFIVE && currenthealth>TWENTYFIVE) return DEFEND;//return defend
	else if(currenthealth<=TWENTYFIVE && currenthealth>FIFTEEN) return rand()%2;//return heal or Attack
	else return ATTACK; //if less than 15% health, always try to attack
}

void printMove(move_type move){
	if(move==ATTACK) printf("ATTACK\n");
	else if(move==DEFEND) printf("DEFEND\n");
	else if(move==HEAL) printf("HEAL\n");
	else printf("Dead\n");
}	

void printInOrder(IntNode *root){//test print
	if(root==NULL) return;//nothing to print
	printInOrder(root->left);//recur left
	
	printf("[%d, %d]  \tmax:%d\t\t", root->range.low, root->range.high, root->max);
	printMove(root->range.myMove);
	printInOrder(root->right); //recur right 
}

void printTrees(){
	printf("boss:\n"); printInOrder(boss);
	printf("\nOpponent:\n"); printInOrder(opponent);
	printf("\n\n");	
}

Interval* getIntervals(int fullhealth, int typeTree){//0 means type tree is my health, 1 means their tree health
	int numIntervals=(fullhealth*0.9)+1, dec=fullhealth*0.1, starthealth=fullhealth;
	ranges[indx]=(Interval*)malloc(sizeof(Interval)*numIntervals);//allocate space, 10% ranges truncated plus 1 
	
	for(int i=0; i<numIntervals; i++){//set intervals
		/*if((fullhealth-dec)<=0){//keeps from adding excessive ranges
			numIntervals=i;
			break;
		}*/	
		ranges[indx][i].high=fullhealth;
		ranges[indx][i].low=fullhealth*=0.9; //set to 90% of range's max
		//ranges[i].low=fullhealth-=dec;//set to dec of 10%
		ranges[indx][i].index=i;//save index
		if(!typeTree) ranges[indx][i].myMove=setMoveMyTree(starthealth, ranges[indx][i].low);
		else ranges[indx][i].myMove=setMoveTheirTree(starthealth, ranges[indx][i].low);
		//printf("interval %d\t[%d,%d]\n", i+1, ranges[i].low, ranges[i].high);
		if(fullhealth<=0){//keeps from adding excessive ranges
			//printf("\n\nBEFORE: for %s\thigh: %d\t\tlow: %d\tindex: %d\n", (indx==0)? "boss":"opponent", ranges[indx][i-1].high, ranges[indx][i-1].low, ranges[indx][i].index);
			ranges[indx][i-1].low=0; //set to 0
			//printf("AFTER:  for %s\thigh: %d\t\tlow: %d\tindex: %d\n\n\n", (indx==0)? "boss":"opponent", ranges[indx][i-1].high, ranges[indx][i-1].low, ranges[indx][i].index);			
			numIntervals=i;
			break;
		}		
	}
	//ranges[indx][numIntervals-1].low=0; //set lowest range to 0, incase it wasnt already there
	//printf("numInterval: %d\n", numIntervals);
	return ranges[indx++];
}

IntNode* new(Interval add){//allocate for new node in tree
	IntNode *create=(IntNode*)malloc(sizeof(IntNode));
	create->range=add;//save interval
	create->max=add.high;
	create->left=create->right=NULL;//terminate ends
	return create;
}

IntNode* store(IntNode *root, Interval add){//insert into the tree
	if(root==NULL) return new(add);
	int low=root->range.low;
	if(add.low<low) root->left=store(root->left, add);//if given interval lower range is less than tree lower range
	else root->right=store(root->right, add);//else it must have been greater
	if(add.high > root->max) root->max=add.high; //check and update tree's max value
	return root;
}

int getSize(Interval *arr){//weird recur loop i designed to find size of array
	static int count=0, i=0;
	if(arr[++i].high!=0){//while next one isnt 0, assumes non allocated memory is 0, might be a problem depending on the system
		//printf("size is %d\n", count);
		count++;
		getSize(arr);
	}
	return count;
}

IntNode* buildTree(int health, int type){
	IntNode *tree=NULL;
	Interval *all=getIntervals(health, type);//send character's health to start with
	
	int size=getSize(all); //printf("size of array: %d\n", size);
	for(int i=0; i<size; i++){
		if(i==size-1) all[i].low=0;
		//printf("Interval %d\tlow: %d\thigh: %d\n", all[i].index, all[i].low, all[i].high);
		tree=store(tree, all[i]);
	}	
	return tree;
}

void freeTree(IntNode *tree){
	if(tree==NULL) return;
	freeTree(tree->left);//recur left
	freeTree(tree->right);//recur right
	free(tree);
}

move_type searchTree(IntNode *tree, int key){//looks for key value within the interval tree
	static int count=0;
	//printf("%d: my value: %d\tlow: %d\thigh %d\n", ++count, key, tree->range.low, tree->range.high);
	if(tree==NULL || key<=0){//shouldnt happen
		count=0;
		//printTrees();	
		//printf("error, tree is empty\n\n");
		return -1;
	}
	//if given health is between ranges
	if(key>tree->range.low && key<=tree->range.high) return tree->range.myMove;
	//if health was greater than the range, recur right
	if(tree->range.low<key) return searchTree(tree->right, key);
	//mustve been smaller than root
	return searchTree(tree->left, key);
}

move_type bossChoose(int bossH, int oppoH, bool first){
	if(first){
		opponent=buildTree(oppoH, 1);//opponents tree
		boss=buildTree(bossH, 0);//boss tree
		//printTrees();
	}
	move_type moves[2];
	moves[0]=searchTree(boss, bossH);//good move for boss's current health
	moves[1]=searchTree(opponent, oppoH); //good move for boss with opponent's current health
	
	//randomly decide which of the 2 moves to return
	return moves[rand()%2];
}

void freeGableStuff(){
	freeTree(boss);
	freeTree(opponent);
	
	for(int i=0; i<indx; i++) free(ranges[i]);//free allocated memory
	indx=0;//reset index	
}

int main(void){
	srand(time(NULL));//for random nums
	/*
	printf("tree 1:\n");
	IntNode *tree1=buildTree(75, 1);//opponents tree
	printInOrder(tree1);
	printf("\n\n");
	printf("tree 2:\n");
	
	IntNode *tree2=buildTree(100, 0);//boss tree
	printInOrder(tree2);
	printf("\n\n");
	
	freeTree(tree1);
	freeTree(tree2);
	
	for(int i=0; i<indx; i++) free(ranges[i]);//free allocated memory
	indx=0;//reset index
	*/
	int bossH=50, OppH=100;
	for(int i=0; i<10; i++){
		printf("Boss health: %d\tOpponent health: %d\n", bossH, OppH);
		if(bossH<=0 || OppH<=0) break;
		printf("Boss move type:\t"); fflush(stdout);
		if(i==0) printMove(bossChoose(bossH, OppH, TRUE));
		else printMove(bossChoose(bossH, OppH, FALSE));
		printf("\n");
		if(bossH>=10) bossH-=5;
		if(OppH>=10) OppH-=5;
	}
	
	freeGableStuff();
	return 0;
}

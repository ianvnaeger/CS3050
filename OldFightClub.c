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

struct stats
{
    char name[64];
    int health;
    int attack;
    int defense;
};

//struct stats user;
//struct stats opponent;

IntNode *opponent, *boss;
Interval *ranges[2];
int indx=0;
int fightRound;
int totalRounds;

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




/*                          Notes From Gable
--------------------------------------------------------------------------------
I added my portion of the assignment to this file. I changed how I originally 
thought to do my portion. At first, I was going to build something like a simple
neural net that learned over time. But we didn't have enough iterations unless I
simulated like 1000 games each execution and it was getting too complicated. So
now the boss's "choosing what move to do" simply takes in the opponent's health
and the boss's health. Now, the implementation is different, and there is only 1
algorithm to decide what to do. Because of this, we need to find a different way
to make the boss "better" each round. A simple way this could be done might be to
just mod the boss's base stats each round to make him better and better. In a nut
shell for my approach, the very first time it is called, it builds 2 interval
trees and then searches them for a move. I think algorithmically it is "difficult"
enough for Xu's standards. So to use my stuff, do the following:

move_to_do=bossChoose(boss current health, opponent current health, true/false);

The first 2 arguments are pretty self-explantory. The third argument HAS to be
set to "true" only the first time this function is called per fight. It signals 
to build the trees. Other than that, it HAS to be set to "false"

If we are moving to the next round and fighting a new boss, before that happens,
the memory allocated for my stuff needs to be released. I wrote a function to do
this, but just remember that it has to be done. Call it like this:

freeGableStuff()

--------------------------------------------------------------------------------
*/



int FightClub( struct stats user , struct stats opponent )
{
    srand(time(NULL));
    int userth , userar , userdr , oppth , oppar , oppdr , userhl , opphl , oppprevdr;
    userth = user.health;
    oppth = opponent.health;
    oppprevdr = 0;
    userar = 0;
    userdr = 0;
    oppar = 0;
    oppdr = 0;
    userhl = 0;
    opphl = 0;
    int choice;
    //Get Variables and set stuff to zero
    
    printf("%s VS. %s", user.name, opponent.name);
    printf("Round %s! \nFIGHT!!!\n", fightRound);
    //Shows round
    
    printf("%s's Health: %d \t\t\t\t\t %s's Health: %d\n\n\n", user.name, userth, opponent.name, oppth );
    printf("What will %s do???\n\t1. Attack \n\t2. Defend \n\t3. Heal\n", user.name);
    scanf( "%d", &choice );
    while( choice != 1 || choice != 2 || choice != 3 )
    {
        printf("Invalid choice, please choose:\n\t1. Attack \n\t2. Defend \n\t3. Heal\n");
    }
    //displays health, asks for action and error checks
        
    switch(choice)
    {
        case 1://attack
            userar = ((rand() % 10) + 1) + user.attack;//Rolls for attack
            printf("%s attacks %s for %d damage!\n",user.name, opponent.name, userar);
            if( oppprevdr != 0 )
            {
                printf("But %s was ready for the attack and blocked %d damage!\n",opponent.name, oppprevdr);
                userar = userar - oppprevdr;//other guy parries the attack away
            }
            oppth = oppth - userar;//Does attack
            printf("%s's Health: %d \t\t\t\t\t %s's Health: %d\n\n\n", user.name, userth, opponent.name, oppth );//show health
            break;
        case 2://defend
            userdr = ((rand() % 5) + 1) + user.defense;//Rolls for defense
            printf("%s braces for the an attack!", user.name);             
            break;
        case 3://heal
            userhl = ((rand() % 5) + 1);  //Rolls for healing
            printf("%s bandages their wounds to heal for %d health",user.name, userhl);
            userth = userth + userhl; 
            printf("%s's Health: %d \t\t\t\t\t %s's Health: %d\n\n\n", user.name, userth, opponent.name, oppth );
            break; 
        default:
            break;
    }
    switch(bossChoose(opponent.health, user.health, TRUE))
    {
        case 0://attack
            oppar = ((rand() % 10) + 1) + opponent.attack;//Rolls for attack
            printf("%s attacks %s for %d damage!\n",opponent.name, user.name, oppar);
            if( oppprevdr != 0 )
            {
                printf("But %s was ready for the attack and blocked %d damage!\n",opponent.name, userdr);
                oppar = oppar - userdr;//other guy parries the attack away
            }
            userth = userth - oppar;//Does attack
            printf("%s's Health: %d \t\t\t\t\t %s's Health: %d\n\n\n", user.name, userth, opponent.name, oppth );
            break;
        case 1://heal
            opphl = ((rand() % 5) + 1); //Rolls for healing           
             printf("%s bandages their wounds to heal for %d health",opponent.name, opphl);
            oppth = oppth + opphl; 
            printf("%s's Health: %d \t\t\t\t\t %s's Health: %d\n\n\n", user.name, userth, opponent.name, oppth );
            break;
        case 2://defend
            oppdr = ((rand() % 5) + 1) + opponent.defense;//Rolls for defense
            printf("%s braces for the an attack!", opponent.name);
            break; 
        default:
            break;
            
    }
    oppprevdr = oppdr;
    
    while( userth <= 0 || oppth <= 0 )
    {
        userar = 0;
        userdr = 0;
        oppar = 0;
        oppdr = 0;
        userhl = 0;
        opphl = 0;
        //re-zeroes stuff
        system("clear");
        //clears terminal
        printf("%s's Health: %d \t\t\t\t\t %s's Health: %d\n\n\n", user.name, userth, opponent.name, oppth );
        printf("What will %s do???\n\t1. Attack \n\t2. Defend \n\t3. Heal\n", user.name);
        scanf( "%d", &choice );
        while( choice != 1 || choice != 2 || choice != 3 )
        {
            printf("Invalid choice, please choose:\n\t1. Attack \n\t2. Defend \n\t3. Heal\n");
        }
        //displays health, asks for action and error checks
        
        switch(choice)
        {
            case 1://attack
                userar = ((rand() % 10) + 1) + user.attack;//Rolls for attack
                printf("%s attacks %s for %d damage!\n",user.name, opponent.name, userar);
                if( oppprevdr != 0 )
                {
                    printf("But %s was ready for the attack and blocked %d damage!\n",opponent.name, oppprevdr);
                    userar = userar - oppprevdr;//other guy parries the attack away
                }
                oppth = oppth - userar;//Does attack
                printf("%s's Health: %d \t\t\t\t\t %s's Health: %d\n\n\n", user.name, userth, opponent.name, oppth );
                break;
            case 2://defend
                userdr = ((rand() % 5) + 1) + user.defense;//Rolls for defense
                printf("%s braces for the an attack!", user.name);
                break;
            case 3://heal
                userhl = ((rand() % 5) + 1);  //Rolls for healing
                printf("%s bandages their wounds to heal for %d health",user.name, userhl);
                userth = userth + userhl; 
                printf("%s's Health: %d \t\t\t\t\t %s's Health: %d\n\n\n", user.name, userth, opponent.name, oppth );
                break; 
            default:
                break;
        }
        
        switch(bossChoose(opponent.health, user.health, FALSE))
        {
            case 0://attack
                oppar = ((rand() % 10) + 1) + opponent.attack;//Rolls for attack
                printf("%s attacks %s for %d damage!\n",opponent.name, user.name, oppar);
                if( oppprevdr != 0 )
                {
                    printf("But %s was ready for the attack and blocked %d damage!\n",opponent.name, userdr);
                    oppar = oppar - userdr;//other guy parries the attack away
                }
                userth = userth - oppar;//Does attack
                printf("%s's Health: %d \t\t\t\t\t %s's Health: %d\n\n\n", user.name, userth, opponent.name, oppth );
                break;
            case 1://heal
                opphl = ((rand() % 5) + 1); //Rolls for healing      
                printf("%s bandages their wounds to heal for %d health",opponent.name, opphl);
                oppth = oppth + opphl; 
                printf("%s's Health: %d \t\t\t\t\t %s's Health: %d\n\n\n", user.name, userth, opponent.name, oppth );
                break;
            case 2://defend
                oppdr = ((rand() % 5) + 1) + opponent.defense;
                printf("%s braces for the an attack!", opponent.name);
                break; 
            default:
                break;
            
        }
        oppprevdr = oppdr;
    }
    
    freeGableStuff();
    
    if( oppth > userth )
    {
        printf("%s stands victorious. %s has been defeated. Finding easier opponent.", opponent.name, user.name);
        return 0;
    }
   
    if( fightRound == totalRounds )
    {
        printf("CONGRATULATIONS!!! %s has defeated all those in their way!\n\n\n\tYOU WIN!!!\n\n\n");
        return 2;
    }
    
    printf("%s stands victorious!!! %s has been defeated!\n\t%s moves on to round %d", user.name, opponent.name, (fightRound+1) );
    return 1;
    
}

int GetBossLevel( struct stats boss )
{
    int level;
    
    level = ((boss.attack + boss.defense + boss.health) / 3 ); 
    
    return level;
}


int main( void )
{
    char Charname[64];
    int WinOrLose = 1;
    int VictoryPoints = 10;
//This will make the user, the opponent, find the number of rounds
    printf("Welcome to Fight Club! \n"); //Please enter your character name:\n");
    //fgets(Charname, 64, stdin);
    //printf("Welcome %s!!! \nHow many rounds would you like to fight?\nEnter number between 1 and 10:\n");
    //scanf("%d", &totalRounds);
    
    //Make User
    
    //Make Opponents
    
    //Get boss level
    
    //Get number of rounds
    
    fightRound = 0;
    
    while( fightRound < (totalRounds+1) )
    {
        WinOrLose = FightClub( user, opponent );
        
        if( WinOrLose == 2 )
        {
            VictoryPoints = VictoryPoints * 2;
            break;
        }

        if( WinOrLose == 1 )
        {
            VictoryPoints = VictoryPoints + GetBossLevel( opponent );
        }
        
        //Find new enemy
        
        fightRound++;
    }
    return 0;
}
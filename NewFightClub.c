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
	char *name;
	int level;
	int health;
	int attack;
	int defense;
	int placeInTree;

};

typedef struct Node
{
    int level;
    char* name;
    int health;
    int attack_level;
    int defense_level;
    struct stats person;
    struct Node* next;
}Node;

struct node{
   struct stats person;
   char color;
   struct node *left;
   struct node *right;
   struct node *child[2]; //was for rb tree
}node;

char CharNameArray[32][64] = {"Scott","Jeff","Batman","Rob","Bruce","Spodermen","Letho","Geralt","Scooby",
"Donnie","KaiserIan","SirKnightGable","KingHenry","MessiahLanden","SinorDong","MisterJon","MadamMeng",
"Pikachu","Zelda","Mario","HelloWorld","Roomba","Lafferre","Naka","Strickland","Bigfoot","LockNessMonster",
"Chupathingy","Fadi","Projector","Rodrigo","Franchesqua"};

Node* Build_List(Node* root, char** Name_Array);
Node* Add_Node(Node* root, int level, char* name, int health, int attack_level, int defense_level);
void Free_Tree(Node* root);
int Stat_Generator(int* health, int* attack_level, int* defense_level, bool is);
void Name_Generator(char** Name_Array, int size);
void Create_User();

struct stats user, person;

IntNode *opponent, *boss;
struct node *root = NULL;
Interval *ranges[2];
int indx=0, fightRound, totalRounds, size;

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

void freeChoice(){
	freeTree(boss);
	freeTree(opponent);
	
	for(int i=0; i<indx; i++) free(ranges[i]);//free allocated memory
	indx=0;//reset index	
}

int FightClub( struct stats user , struct stats opponent )
{
   // srand(time(NULL));
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
	//intialize rolls to zero
    printf("\n\n\t\t\t%s VS. %s\n", user.name, opponent.name); fflush(stdout);
    printf("Round %d! FIGHT!!!\n\n", fightRound+1);
    
    printf("%s's Health: %d \t\t\t\t\t %s's Health: %d\n\n", user.name, userth, opponent.name, oppth );
    printf("What will %s do???\n\t1. Attack \n\t2. Defend \n\t3. Heal\n", user.name);
    scanf( "%d", &choice );
    while( choice != 1 && choice != 2 && choice != 3 )
    {
        printf("Invalid choice, please choose:\n\t1. Attack \n\t2. Defend \n\t3. Heal\n");
		scanf( "%d", &choice );
	}//Gets user choice and error checks
       
    printf("\n");    
    switch(choice)
    {
        case 1://attack
            userar = ((rand() % 5) + 1) + user.attack;//rolls attack
            printf("%s attacks %s for %d damage!\n",user.name, opponent.name, userar);
            if( oppprevdr != 0 )
            {
                printf("But %s was ready for the attack and blocked %d damage!\n",opponent.name, oppprevdr);
                userar = userar - oppprevdr;
            }//checks if there was a block prepared and calculates damage accordingly
            if( userar < 0 )
			{
				oppth = oppth;
			}
			else
			{
				oppth = oppth - userar;
            }//Ensures that if the block is more than damage done that it won't heal the enemy, that kinda broke the game...
			printf("%s's Health: %d \t\t\t\t\t %s's Health: %d\n\n\n", user.name, userth, opponent.name, oppth );
            break;
        case 2://defend
            userdr = ((rand() % 5) + 1) + user.defense; //rolls defense
            printf("%s braces for the an attack!\n", user.name);             
            break;
        case 3://heal
            userhl = ((rand() % 5) + 1);  //rolls for health
            printf("%s bandages their wounds to heal for %d health\n",user.name, userhl);
            userth = userth + userhl; //heals person
            printf("%s's Health: %d \t\t\t\t\t %s's Health: %d\n\n\n", user.name, userth, opponent.name, oppth );
            break; 
        default:
            break;
    }
    switch(bossChoose(opponent.health, oppth, TRUE))
    {
        case 0://attack
            oppar = ((rand() % 10) + 1) + opponent.attack;//rolls attack
            printf("%s attacks %s for %d damage!\n",opponent.name, user.name, oppar);
            if( userdr != 0 )
            {
                printf("But %s was ready for the attack and blocked %d damage!\n",opponent.name, userdr);
                oppar = oppar - userdr;
            }//checks if there was a block prepared and calculates damage accordingly
			if( oppar < 0 )
			{
				userth = userth;
			}
			else
			{
				userth = userth - oppar;
			}//Ensures that if the block is more than damage done that it won't heal the enemy, that kinda broke the game...
            printf("%s's Health: %d \t\t\t\t\t %s's Health: %d\n\n\n", user.name, userth, opponent.name, oppth );
            break;
        case 1://heal
            opphl = ((rand() % 5) + 1);   //rolls for health         
             printf("%s bandages their wounds to heal for %d health\n",opponent.name, opphl);
            oppth = oppth + opphl; //heals person
            printf("%s's Health: %d \t\t\t\t\t %s's Health: %d\n\n\n", user.name, userth, opponent.name, oppth );
            break;
        case 2://defend
            oppdr = ((rand() % 5) + 1) + opponent.defense; //rolls defense
            printf("%s braces for the an attack!\n", opponent.name);
            break; 
        default:
            break;
            
    }
    oppprevdr = oppdr; //saves opponent's defense roll for the next attack
    
    while( userth > 0 && oppth > 0 )
    {
        userar = 0;
        userdr = 0;
        oppar = 0;
        oppdr = 0;
        userhl = 0;
        opphl = 0;
		//intialize rolls to zero
        printf("%s's Health: %d \t\t\t\t\t %s's Health: %d\n\n", user.name, userth, opponent.name, oppth );
        printf("What will %s do???\n\t1. Attack \n\t2. Defend \n\t3. Heal\n", user.name);
        scanf( "%d", &choice );
        while( choice != 1 && choice != 2 && choice != 3 )
        {
            printf("Invalid choice, please choose:\n\t1. Attack \n\t2. Defend \n\t3. Heal\n");
			scanf( "%d", &choice );
		}//Gets user choice and error checks
        
        switch(choice)
        {
            case 1://attack
                userar = ((rand() % 10) + 1) + user.attack;//rolls attack
                printf("%s attacks %s for %d damage!\n",user.name, opponent.name, userar);
                if( oppprevdr != 0 )
                {
                    printf("But %s was ready for the attack and blocked %d damage!\n",opponent.name, oppprevdr);
                    userar = userar - oppprevdr;
                }//checks if there was a block prepared and calculates damage accordingly
				if( userar < 0 )
				{
					oppth = oppth;
				}
				else
				{
					oppth = oppth - userar;
				}//Ensures that if the block is more than damage done that it won't heal the enemy, that kinda broke the game...
                printf("%s's Health: %d \t\t\t\t\t %s's Health: %d\n\n\n", user.name, userth, opponent.name, oppth );
                break;
            case 2://defend
                userdr = ((rand() % 5) + 1) + user.defense; //rolls defense
                printf("%s braces for the an attack!\n", user.name);
                break;
            case 3://heal
                userhl = ((rand() % 5) + 1);  //rolls for health
                printf("%s bandages their wounds to heal for %d health\n",user.name, userhl);
                userth = userth + userhl; //heals person
                printf("%s's Health: %d \t\t\t\t\t %s's Health: %d\n\n\n", user.name, userth, opponent.name, oppth );
                break; 
            default:
                break;
        }
        
        switch(bossChoose(opponent.health, oppth, FALSE))
        {
            case 0://attack
                oppar = ((rand() % 10) + 1) + opponent.attack;//rolls attack
                printf("%s attacks %s for %d damage!\n",opponent.name, user.name, oppar);
                if( userdr != 0 )
                {
                    printf("But %s was ready for the attack and blocked %d damage!\n",opponent.name, userdr);
                    oppar = oppar - userdr;
                }//checks if there was a block prepared and calculates damage accordingly
				if( oppar < 0 )
				{
					userth = userth;
				}
				else
				{
					userth = userth - oppar;
				}//Ensures that if the block is more than damage done that it won't heal the enemy, that kinda broke the game...
				printf("%s's Health: %d \t\t\t\t\t %s's Health: %d\n\n\n", user.name, userth, opponent.name, oppth );
                break;
            case 1://heal
                opphl = ((rand() % 5) + 1);   //rolls for health         
                printf("%s bandages their wounds to heal for %d health\n",opponent.name, opphl);
                oppth = oppth + opphl; //heals person
                printf("%s's Health: %d \t\t\t\t\t %s's Health: %d\n\n\n", user.name, userth, opponent.name, oppth );
                break;
            case 2://defend
                oppdr = ((rand() % 5) + 1) + opponent.defense; //rolls defense
                printf("%s braces for the an attack!\n", opponent.name);
                break; 
            default:
                break;
            
        }
        oppprevdr = oppdr;//saves opponent's defense roll for the next attack
    }
    
    freeChoice();
    
    if( ((fightRound+1) == totalRounds) && (userth > oppth) ) 
    {
        printf("\n\nCONGRATULATIONS!!! %s has made it to the end of the Gauntlet!\n\n\n\tYOU WIN!!!\n\n\n", user.name);
		return 2;
    }
	if(((fightRound+1) == totalRounds) && (oppth > userth))
	{
		printf("\n\n%s stands victorious. %s has been defeated. Their are no more opponents standing in your way.\n\n\n\tGAME OVER\n\n\n",opponent.name, user.name );
		return 0;
	}
	if( oppth > userth )
    {
        printf("\n\n%s stands victorious. %s has been defeated. Finding Weaker Opponent for round %d...\n\n", opponent.name, user.name, (fightRound+1)+1);
        return 0;
    }
    
    printf("\n\n%s stands victorious!!! %s has been defeated! %s moves on to round %d\n\n", user.name, opponent.name, user.name, (fightRound+1)+1 );
    return 1;
    
}

int Stat_Generator(int* health, int* attack_level, int* defense_level, bool is)//Randomly generates the character stats
{	int level;
	if(!is){
		*health = rand() % 75 + 25;//range 25 to 100
		*attack_level = rand() % 15 + 5;//range is 5 to 20
		*defense_level = rand() % 10 + 5;//range is 5 to 15
		level=(*health + *attack_level + *defense_level) / 3;
	}
	else{
		*health = rand() % 10 + 90;//range 90 to 100
		*attack_level = rand() % 5 + 15;//range is 15 to 20
		*defense_level = rand() % 5 + 5;//range is 5 to 10
		level=(*health + *attack_level + *defense_level) / 3;		
	}
    return level;
}

void Name_Generator(char** Name_Array, int size)//Pulls a list of names from a file called names.txt
{
    int i = 0;

    for(i = 0; i < size; i++)
    {
        *(Name_Array + i) = malloc(sizeof(char)*64);

		strcpy( *(Name_Array + i) , CharNameArray[i]);
    }
}

int power(int num, int tothe){
	int result=1;
	while(tothe!=0){
		result*=num;
		--tothe;
	}
	return result;
}

Node* Build_List(Node* root, char** Name_Array)//Takes the random stats and name and creates a tree
{
    int level;
    char* name = NULL;
    int health;
    int attack_level;
    int defense_level;
    int enemy_count=6;
    int i = 0;
	char c;
	while((c=getchar())!='\n' && c!=EOF) fflush(stdin);	//flush out input buffer
	while(totalRounds>5 || totalRounds<1){
		fflush(stdin);	
		printf("Please enter the number of enemies (5 or less): ");
		scanf("%d", &totalRounds);
    }
    
	size=power(2, totalRounds);    
    Name_Generator(Name_Array, size);    
    
    //totalRounds=enemy_count;
    for(i = 0; i < size; i++)
    {
        name = *(Name_Array + i);
        if(!strcmp("SinorDong", name) || !strcmp("MisterJon", name) || !strcmp("MadamMeng", name)) level = Stat_Generator(&health, &attack_level, &defense_level, TRUE);
        else level = Stat_Generator(&health, &attack_level, &defense_level, FALSE);
        root = Add_Node(root, level, name, health, attack_level, defense_level);
    	 printf("\nBuild_List%d %d\n", attack_level, defense_level);        
    }

    return root;

}

Node* Add_Node(Node* root, int level, char* name, int health, int attack_level, int defense_level)
//creates the nodes that will go in the tree
{
    if(root == NULL)
    {
        root = malloc(sizeof(Node));
        root->level = level;
        root->name = name;
        root->attack_level = attack_level;
        root->defense_level = defense_level;
        root->health = health;
        root->next = NULL;

		printf("\nAdd_Node%d %d\n", attack_level, defense_level);
        //printf("---Stats---\nName: %s\nLevel: %d\n", name, level);
        //printf("Health: %d\nAttack: %d\nDefense: %d\n", *health, *attack_level, *defense_level);
    }

    else
    {
        root->next = Add_Node(root->next, level, name, health, attack_level, defense_level);
    }

    return root;

}

void Create_User()
{
	//char* name = malloc(sizeof(char)*50);

	user.name = malloc(sizeof(char)*64);

	printf("Please enter a name for your character: ");
	scanf("%s", user.name);
	printf("\n");

	user.level = Stat_Generator(&user.health, &user.attack, &user.defense, FALSE);

	//printf("---Stats---\nName: %s\nLevel: %d\n", name, ptr->level);
	//printf("Health: %d\nAttack: %d\nDefense: %d\n", ptr->health, ptr->attack_level, ptr->defense_level);
}

int GetBossLevel( struct stats b )
{
    int level;
    
    level = ((b.attack + b.defense + b.health) / 3 ); 
    
    return level;
}

struct node * createNode(struct stats name) {
    struct node *newnode;
    newnode = (struct node *)malloc(sizeof(struct node));
    newnode->person.name = name.name;
    //strcpy(newnode->person.name, name.name);
    newnode->person.level = name.level;
    newnode->person.health = name.health;
	newnode->person.attack = name.attack;
	newnode->person.defense = name.defense;
		
    return newnode;
}

void case3(struct node* move,struct node* list[], int k, struct node* yPtr, int dir[]){
    move = list[k - 2];
    yPtr->color = 'B';
    move->color = 'R';
    move->child[1] = yPtr->child[0];
    yPtr->child[0] = move;
        if (move == root) {
            root = yPtr;
            } else {
                list[k - 3]->child[dir[k - 3]] = yPtr;
                    }
}

void case2(struct node* move,struct node* list[], int k, struct node* yPtr){
    move = list[k - 1];
    yPtr = move->child[1];
    move->child[1] = yPtr->child[0];
    yPtr->child[0] = move;
    list[k - 2]->child[0] = yPtr;
}

void case1(struct node* list[], int k, struct node* yPtr){
    list[k - 2]->color = 'R';
    list[k - 1]->color = yPtr->color = 'B';
    k = k -2;
}

void insertion (struct stats person) {
	//	printf("\n\n---health: %d\n\n", person.health);
    struct node *list[98], *hold, *newnode, *move, *ptr;
    int dir[98], k = 0, index;
    hold = root;
 //set root
    if (!root) {
            root = createNode(person);
            root->color = 'B';
            return;
    }
    list[k] = root;
    dir[k++] = 0;
    while (hold != NULL) {
            if (hold->person.name == person.name) {
                    printf("you have multiple %s's thats not allowed\n",person.name);
                    return;
            }
            index = (person.level - hold->person.level) > 0 ? 1 : 0;
            list[k] = hold;
            hold = hold->child[index];
            dir[k++] = index;
    }
    list[k - 1]->child[index] = newnode = createNode(person);
    while ((k >= 3) && (list[k - 1]->color == 'R')) {
            if (dir[k - 2] == 0) {
                    ptr = list[k - 2]->child[1];

                    if (ptr != NULL && ptr->color == 'R') { 
                        case1(list,k,ptr);
                    } else {
                            if (dir[k - 1] == 0) {
                                    ptr = list[k - 1];
                            } else {
                                 case2(move, list, k, ptr);
                            }
                             case3(move, list, k, ptr, dir);
                            break;
                    }
            } else {
                    ptr = list[k - 2]->child[0];
                    if ((ptr != NULL) && (ptr->color == 'R')) {
                        case1(list,k,ptr);
                    } else {
                            if (dir[k - 1] == 1) {
                                    ptr = list[k - 1];
                            } else {
                                    case2(move, list, k, ptr);
                            }
                            case3(move, list, k, ptr, dir);
                            break;
                    }
            }
    }
    root->color = 'B';
}

void print(struct node* head, int level)
{
    if (head == NULL)
        return;
    if (level == 1)
        printf("%s-%c: Health: %d, Level: %d\n ", head->person.name, head->color, head->person.health,head->person.level);
    else if (level > 1)
    {
        print(head->child[0], level-1);
        print(head->child[1], level-1);
    }
}

void printLevelOrder(struct node* head)
{
    int i;
    for (i=1; i<=8; i++)
    {
        print(head, i);
       
    }
}
 
struct node* pickOpp(int outcome, struct node* currentBoss){
/*
	if(outcome == 1){
	   return currentBoss->child[1];
	}else if(outcome == 0){
	    return currentBoss->child[0];
	}
*/

	if(outcome && currentBoss->right!=NULL) return currentBoss->right;
	else if(!outcome && currentBoss->left!=NULL) return currentBoss->left;
	else return NULL;
}

void fillTree(Node* list){
	Node* temp=list;
	int count=0;
	while(temp!=NULL){
	
		person.name=temp->name;
		person.level=temp->level;
		person.health=temp->health;
		person.defense=temp->defense_level;
		person.attack=temp->attack_level;
		printf("Name: %s\tlevel: %d\tattack: %d\tdefense: %d\thealth: %d\n", person.name, person.level, person.attack, person.defense, person.health); fflush(stdout);		
		insertion(person);
		
		temp=temp->next;
	}
}

void printList(Node* list){
	Node *temp=list;
	while(temp!=NULL){
		printf("Name: %s\tlevel: %d\tattack: %d\tdefense: %d\thealth: %d\n", temp->name, temp->level, temp->attack_level, temp->defense_level, temp->health); fflush(stdout);
		temp=temp->next;
	}
}

void swapVals(Node *swap, Node *with){
	int level, health, attack, defense;
	char *name;
	//store contents
	name=swap->name;
	level=swap->level;
	health=swap->health;
	attack=swap->attack_level;
	defense=swap->defense_level;
	
	//store to other node
	swap->name=with->name;
	swap->level=with->level;
	swap->health=with->health;
	swap->attack_level=with->attack_level;
	swap->defense_level=with->defense_level;
	
	//replace temp vals to other node
	with->name=name;
	with->level=level;
	with->health=health;
	with->attack_level=attack;
	with->defense_level=defense;
}

void sortList(Node *h){
	Node *start, *move, *least;
	start=h;
	
	while(start->next){
		least=start;
		move=start->next;//set to next node
		while(move){
			if(least->level>move->level) least=move;
			move=move->next;//move forward in list
		}
		swapVals(start, least);//move values within ndes
		start=start->next; //
	}
}

struct node* list_BST(Node **head, int len){//turn sorted linked list to balanced bst helper function
	if(len<=0) return NULL; //break out recur case
	struct node *left, *tree;
	left=list_BST(head, len/2);//recur with half length
	int temph=0, tempa=0, tempd=0;
	
	(*head)->person.name=(*head)->name;	
	(*head)->person.level=(*head)->level;

	(*head)->person.health=0;
	(*head)->person.attack=0;	
	(*head)->person.defense=0;
	temph=(*head)->health;
	tempa=(*head)->attack_level;	
	tempd=(*head)->defense_level;
	
	(*head)->person.health =69;
	(*head)->person.attack = 8;
	(*head)->person.defense = 100;	
	
	(*head)->person.health= temph;
	(*head)->person.attack= tempa;	
	(*head)->person.defense= tempd;
	
	tree=createNode((*head)->person); 
	tree->left=left;//allocate for left node
	*head=(*head)->next;
	tree->right=list_BST(head, len-len/2-1);//recur for other portion dec by 1

	return tree;
}

void freeList(Node *head){
	Node *temp;
	while((temp=head)!=NULL){
		head=head->next;//move through list
		free(temp);
	}
}

void freeT(struct node *tree){
	if(tree==NULL) return;
	freeT(tree->left);//recur left
	freeT(tree->right);//recur right
	free(tree);
}

int main(void){
	srand(time(NULL));

	int WinOrLose = 1;
	int VictoryPoints = 10;
	struct stats opp;
	//This will make the user, the opponent, find the number of rounds
	printf("\n---------------------------------------------\tWelcome to Fight Club!\t---------------------------------------------\n\n"); 

	//Make User
	Create_User();

	//Make opponents
    char** Name_Array = malloc(sizeof(char*)*32);
    Node* head = NULL;

    head = Build_List(head, Name_Array);

	sortList(head);
	
	//fillTree(head);
	
	root=list_BST(&head, size);//create balaned bst
	
	//printLevelOrder(root);

	fightRound = 0;
	struct node* bossman;
	opp=root->person;
	bossman = root; 
	while( fightRound < (totalRounds+1) )//While we haven't reached number of rounds, keep playing
	{
		WinOrLose = FightClub( user, opp ); // play the game with the user fighting the opponent

		if( totalRounds == 1 && WinOrLose == 0 )
		{
			break;
		}
		if( WinOrLose == 2 )
		{
			VictoryPoints = VictoryPoints * 2;
			break;
		}// If you when the last round, you get double score

		if( WinOrLose == 1 )
		{
			VictoryPoints = VictoryPoints + GetBossLevel( opp );
		}// Adds to your score for every win you have

		//Find new enemy
		bossman=pickOpp(WinOrLose, bossman);
		if(bossman==NULL){
			break;
		}
		opp= bossman->person;
		
		fightRound++; //bumb counter
	}
	
	printf("Your score is: %d\nThank you for playing!\n", VictoryPoints);
	//Display final score
	
	//for(int i=0; i<32; i++) free(Name_Array[i]);
	free(Name_Array);
	free(user.name);
	freeList(head);
	free(root);
	//free memory
		
	return 0;
}

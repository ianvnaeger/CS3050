/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: AaronHenry
 *
 * Created on April 30, 2017, 4:52 PM
 */



#include <stdio.h>
#include <stdlib.h>


struct character{
    char* name;
    int level;
    int health;
    int placeInTree;
};

struct node{
   struct character person;
   char color;
   struct node *child[2];
}node;

struct node *root = NULL;

struct node * createNode(char* name) {
    struct node *newnode;
    newnode = (struct node *)malloc(sizeof(struct node));
    newnode->person.name = name;
    newnode->person.level = 5;
    newnode->person.health = 100;
    newnode->color = 'R';
    newnode->child[0] = newnode->child[1] = NULL; /* link[0] - Left Child; link[1] - Right Child */
   
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

void insertion (struct character person) {
    struct node *list[98], *hold, *newnode, *move, *ptr;
    int dir[98], k = 0, index;
    hold = root;
 //set root
    if (!root) {
            root = createNode(person.name);
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
            index = ((int)person.name - (int)hold->person.name) > 0 ? 1 : 0;
            list[k] = hold;
            hold = hold->child[index];
            dir[k++] = index;
    }
    list[k - 1]->child[index] = newnode = createNode(person.name);
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

/*
 * 
 */
 
 
 struct character winOrLose(int outcome, struct node* currentBoss){
     
     if(outcome == 1){
        return currentBoss->child[1];
     }else if(outcome == 0){
         return currentBoss->child[0];
     }
     
 }


int main(int argc, char** argv) {
    int numOfPeople=1;
    printf("# of ppl: ");
    scanf("%d", &numOfPeople);
    int i=0;
    char* name;// = malloc(sizeof(char*));
    struct character person;
    
    
    for(i = 0; i<numOfPeople;i++){
        name = malloc(sizeof(char*));
        printf("Name: ");
        scanf("%s", name);
        person.name = name;
        person.level = (int)name;
        insertion(person);
        free(name);
    }
    
    
    struct character person1;
    struct character person2;
    
    person1.name = "aaron";
    person1.level = (int) "aaron";
    person2.name = "Bob";
    person2.level = (int) "Bob";
    
    insertion(person1);
    insertion(person2);
    
    printLevelOrder(root);
    
    
    return (EXIT_SUCCESS);
}


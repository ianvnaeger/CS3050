#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct Node
{
    int level;
    char* name;
    int health;
    int attack_level;
    int defense_level;
    struct Node* next;
}Node;

Node* Build_List(Node* root, char** Name_Array);
Node* Add_Node(Node* root, int level, char* name, int* health, int* attack_level, int* defense_level);
void Free_Tree(Node* root);
int Stat_Generator(int* health, int* attack_level, int* defense_level);
void Name_Generator();
void Create_User(Node* ptr);

/*int main()
{
    srand(time(0));

    char** Name_Array = malloc(sizeof(char*)*10);
    Node* root = NULL;
    Node* User_Ptr = malloc(sizeof(Node));

    Create_User(User_Ptr);

    Name_Generator(Name_Array);

    root = Build_List(root, Name_Array);

    printf("\nRoot Pointer: %p\n", root);

    return 0;
}
*/
int Stat_Generator(int* health, int* attack_level, int* defense_level)//Randomly generates the character stats
{
    *health = rand() % 50 + 1;
    *attack_level = rand() % 10 + 1;
    *defense_level = rand() % 5 + 1;
    int level = (*health + *attack_level + *defense_level) / 3;

    return level;
}

void Name_Generator(char** Name_Array)//Pulls a list of names from a file called names.txt
{
    int i = 0;

    FILE *fp = fopen("names.txt", "r");

    if (fp == NULL)
    {
        printf("Error reading name file!\n");
        fclose(fp);
    }

    for(i = 0; i < 10; i++)
    {
        *(Name_Array + i) = malloc(sizeof(char)*20);
        fscanf(fp, "%s", *(Name_Array + i));
    }
}

Node* Build_List(Node* root, char** Name_Array)//Takes the random stats and name and creates a tree
{
    int level;
    char* name = NULL;
    int* health = malloc(sizeof(int));
    int* attack_level = malloc(sizeof(int));
    int* defense_level = malloc(sizeof(int));
    int enemy_count;
    int i = 0;

    printf("Please enter 10 or less for the number of enemies you want in the tournament.\n");
    printf("Number of enemies: ");
    scanf("%d", &enemy_count);

    for(i = 0; i < enemy_count; i++)
    {
        level = Stat_Generator(health, attack_level, defense_level);
        name = *(Name_Array + i);
        root = Add_Node(root, level, name, health, attack_level, defense_level);
    }

    return root;

}

Node* Add_Node(Node* root, int level, char* name, int* health, int* attack_level, int* defense_level)
//creates the nodes that will go in the tree
{
    if(root == NULL)
    {
        root = malloc(sizeof(Node));
        root->level = level;
        root->name = name;
        root->attack_level = *attack_level;
        root->defense_level = *defense_level;
        root->health = *health;
        root->next = NULL;

        printf("---Stats---\nName: %s\nLevel: %d\n", name, level);
        printf("Health: %d\nAttack: %d\nDefense: %d\n", *health, *attack_level, *defense_level);
    }

    else (level <= root->level)
    {
        root->next = Add_Node(root->next, level, name, health, attack_level, defense_level);
    }

    return root;

}

void Create_User(Node* ptr)
{
    char* name = malloc(size(char)*50);

        char* temp_name = malloc(sizeof(char)*50);

        printf("Please enter a name for your character.\n");
        scanf("%s", temp_name);

        ptr->name = temp_name;
        ptr->level = Stat_Generator(ptr->health, ptr->attack_level, ptr->defense_level);

        printf("---Stats---\nName: %s\nLevel: %d\n", name, level);
        printf("Health: %d\nAttack: %d\nDefense: %d\n", *health, *attack_level, *defense_level);
        

}

//
// Created by giacomo on 18/07/20.
//
#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 1024

/* line list */
typedef struct line {
    struct line* prev;
    struct line* next;
    int index;
    char* content;
}lineNode;
typedef lineNode* lineList;
// handling functions
lineNode* create_line(int index, char* content);
lineList insert_line(lineList listHead, lineNode* newNode, int index);
/* main functions */
void change_lines(int line1, int line2);

/********
 * MAIN *
 ********/
int main() {
    char inputStr[MAX_LINE_LENGTH];
    char cmdChar = '\0';
    // main loop
    while(cmdChar!='q') {
        // read command
        fgets(inputStr, MAX_LINE_LENGTH, stdin);
        // decode command
        char *buffStr;
        int param1 = (int)strtol(inputStr, &buffStr, 10);
        int param2 = 0;
        if(buffStr[0]==',') {
            param2 = (int)strtol(&buffStr[1], &buffStr, 10);
        }
        cmdChar = buffStr[0];
        // switch command cases
        switch(cmdChar) {
            case 'c':
                // change lines
                break;
            case 'd':
                // delete lines
                break;
            case 'p':
                // print lines
                break;
            case 'u':
                // undo
                break;
            case 'r':
                // redo
                break;
            default:
                break;
        }
    }
}

/*************
 * FUNCTIONS *
 *************/
/* Line list handling */
lineNode* create(int index, char* content) {
    // create empty node
    lineNode* newNode = malloc(sizeof(lineNode));
    // initialize node
    newNode->index = index;
    newNode->content = content;
    newNode->prev = NULL;
    newNode->next = NULL;
    // return node pointer
    return newNode;

//
// Created by Giacomo Pizzamiglio
//

#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 1028

/*
 * DEFINITIONS
 */
/** CMDS **/
/* list types */
typedef struct c{
    struct c* next;
    struct c* prev;
    int line1, line2;
    char** linesBck;
}cmdNode;
typedef cmdNode* cmdList;
/* handling functions for command lists */
cmdNode* create_command(const int* linesLimit, char** linesBck);
cmdList append_command(cmdList listHead, cmdNode* newNode);
/** TEXT **/
typedef struct l {
    struct l *next;
    struct l *prev;
    int index;
    char* content;
}txtNode;
typedef txtNode* txtList;
/* handling functions for text lists */
txtNode* create_line(int index, char* content);
txtList append_line(txtList listHead, txtNode* newNode);
/** MACRO FUNCTIONS **/
void change(const int* linesLimit);
void delete(const int* linesLimit);
void print(const int* linesLimit);
void undo(int n);
void redo(int n);

/** GLOBAL VARIABLES **/
cmdList cmdHistory = NULL;
txtList txtLines = NULL;

/*
 * MAIN
 */
int main() {
    // get input from stdin
    char inputString[MAX_LINE_LENGTH];
    fgets(inputString, MAX_LINE_LENGTH, stdin);
    // decode inputString
    int* param;
    param = calloc(sizeof(int), 1);
    char cmd, *buffer = NULL;
    if(inputString[0] != 'q') {
        param[0] = (int) strtol(inputString, &buffer, 10);
        if (buffer[0] == ',') {
            param = realloc(param, sizeof(int) * 2);
            param[1] = (int) strtol(&buffer[1], &buffer, 10);
        }
        cmd = buffer[0];

        switch (cmd) {
            /* change */
            case 'c':
                change(param);
                break;
            default:
                break;
        }
    }

    free(txtLines);
    free(cmdHistory);
    return 0;
}

/*
 * COMMANDS HANDLING FUNCTIONS
 */
/* create new command node with given fields */
cmdNode* create_command(const int* linesLimit, char** linesBck) {
    // create empty node
    cmdNode* newNode = malloc(sizeof(cmdNode));
    // initialize fields
    newNode->prev = NULL;
    newNode->next = NULL;
    newNode->line1 = linesLimit[0];
    newNode->line2 = linesLimit[1];
    newNode->linesBck = linesBck;

    return newNode;
}
/* append new command to a given list */
cmdList append_command(cmdList listHead, cmdNode* newNode) {
    // if listHead is empty add new node as head
    if(listHead==NULL) {
        return newNode;
    }
    // if listHead is not empty add new node as last node
    else{
        cmdNode* listCur = listHead;
        while(listCur->next!=NULL){
            listCur = listCur->next;
        }
        listCur->next = newNode;
    }
    return listHead;
}
/*
 * LINES HANDLING FUNCTIONS
 */
/* create new txt node with given line and index */
txtNode* create_line(int index, char* content) {
    // create empty node
    txtNode* newNode = malloc(sizeof(txtNode));
    // initialize fields
    newNode->prev = NULL;
    newNode->next = NULL;
    newNode->index = index;
    newNode->content = content;

    return newNode;
}
/* append new line to a given list */
txtList append_line(txtList listHead, txtNode* newNode) {
    // if listHead is empty add new node as head
    if(listHead==NULL) {
        return newNode;
    }
    // if listHead is not empty add new node as last node
    else{
        txtNode* listCur = listHead;
        while(listCur->next!=NULL){
            listCur = listCur->next;
        }
        listCur->next = newNode;
    }
    return listHead;
}
txtList search_line(txtList listHead, txtNode* newNode, int index) {

}
/*
 * MACRO FUNCTIONS
 */
/* change lines */
void change(const int* linesLimit) {
    // count
    int linesNum = linesLimit[1] - linesLimit[0] + 1;
    char* linesCont[linesNum];
    for(int i=0; i<linesNum; i++) {
        linesCont[i] = malloc(sizeof(char) * MAX_LINE_LENGTH);
        fgets(linesCont[i], MAX_LINE_LENGTH, stdin);
    }

    if(txtLines==NULL && linesLimit[0]==1) {
        for(int i=0; i<linesNum; i++) {
            txtLines = append_line(txtLines, create_line(i+1, linesCont[i]));
        }
        cmdHistory = append_command(cmdHistory, create_command(linesLimit, NULL));
    }
    else if(txtLines!=NULL) {

    }

    /** debug **/
    printf("\n");
    int n = 1;
    txtNode* cur = txtLines;
    while(cur!=NULL) {
        printf("%d: %s", n, cur->content);
        n++;
        cur = cur->next;
    }
}

/** STAI SCRIVENDO IL CASO DELLA FUNZIONE CHANGE IN CUI IL TESTO NON E' VUOTO **/
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
void print_list(txtList listHead);
/** MACRO FUNCTIONS **/
txtList change(const int* linesLimit);
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
    while(1) {
        // get input from stdin
        char inputString[MAX_LINE_LENGTH];
        fgets(inputString, MAX_LINE_LENGTH, stdin);
        // decode inputString
        int *param;
        param = calloc(sizeof(int), 1);
        char cmd, *buffer = NULL;
        if (inputString[0] != 'q') {
            param[0] = (int) strtol(inputString, &buffer, 10);
            if (buffer[0] == ',') {
                param = realloc(param, sizeof(int) * 2);
                param[1] = (int) strtol(&buffer[1], &buffer, 10);
            }
            cmd = buffer[0];

            switch (cmd) {
                /* change */
                case 'c':
                    txtLines = change(param);
                    print_list(txtLines);
                    break;
                default:
                    free(txtLines);
                    free(cmdHistory);
                    return 0;
            }
        }
    }
    /* free(txtLines);
    free(cmdHistory);
    return 0; */
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
        newNode->prev = listCur;
    }
    return listHead;
}

/*
 * LINES HANDLING FUNCTIONS
 */
/* create new line node with given fields */
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
    txtNode* listCur = listHead;
    while(listCur->next!=NULL){
        listCur = listCur->next;
    }
    listCur->next = newNode;
    newNode->prev = listCur;
    return listHead;
}
/* search for insertion point */
txtNode* search_insert_point(txtList listHead, int topLimit) {
    if(listHead==NULL) {
        return NULL;
    }
    txtNode* listCur = listHead;
    while(listCur->index != topLimit-1) {
        if(listCur!=NULL) {
            return NULL;
        }
        listCur = listCur->next;
    }
    return listCur;
}
/* print entire list */
void print_list(txtList listHead) {
    if(listHead==NULL) {
        printf("EMPTY!\n");
    }
    else {
        txtNode* listCur = listHead;
        int counter = 1;
        while(listCur!=NULL) {
            printf("%d: %s", counter, listCur->content);
            counter++;
            listCur = listCur->next;
        }
    }
}
/*
 * MACRO FUNCTIONS
 */
/* change lines */
txtList change(const int* linesLimit) {
    int linesNum = linesLimit[1] - linesLimit[0] + 1;
    // create record in command history
    char* linesBackup[linesNum];
    cmdNode* lastCmd = create_command(linesLimit, linesBackup);
    cmdHistory = append_command(cmdHistory, lastCmd);

    if (linesLimit[0]!=1 && txtLines==NULL) {
        return txtLines;
    }
    else {
        txtNode* topLimNode = search_insert_point(txtLines, linesLimit[0]);
        txtNode* tempNode = topLimNode==NULL ? NULL : topLimNode->next;
        char* newLine[linesNum];
        for(int i=0; i<linesNum; i++) {
            newLine[i] = malloc(sizeof(char) * MAX_LINE_LENGTH);
            fgets(newLine[i], MAX_LINE_LENGTH, stdin);
        }
        for(int i=0; i<linesNum; i++) {
            if(tempNode!=NULL) {
                lastCmd->linesBck[i] = tempNode->content;
                tempNode->content = newLine[i];
            }
            else {
                txtNode* newNode = create_line(linesLimit[0]+i, newLine[i]);
                txtLines = append_line(txtLines, newNode);
            }
        }
    }
    return txtLines;
}

/** HAI IMPLEMENTATO PROBABILMENTE CON SUCCESSO LA FUNZIONE CHANGE; HAI SCRITTO UNA BOZZA DELLA FUNZIONE PRINT;
 * DEVI IMPLEMENTARE LA FUNZIONE DELETE; DEVI IMPLEMENTARE LE FUNZIONI UNDO E REDO.
 * PER IL MOMENTO L'OUTPUT E' COERENTE SOLO FINO ALLA PRIMA SERIE; DEVI COMPLETARE LA GESTIONE DELLA LISTA DEI COMANDI.
 */
//
// Created by giacomo on 17/07/20.
//

#include <stdio.h>
#include <stdlib.h>

typedef struct n{
    struct n* prev;
    struct n* next;
    int value;
}node;
typedef node* list;

node* create(int value);
list append1(list listHead, node* newNode);
list append2(list listHead, node* newNode);

list head = NULL;

int main() {
    head = append1(head, create(10));
    head = append1(head, create(15));
    head = append1(head, create(33));
    printf("%d\n", head->value);
    printf("%d\n", head->next->value);
    printf("%d\n", head->next->next->value);
    return 0;
}

node* create(int value) {
    node* newNode = malloc(sizeof(node));
    newNode->prev = NULL;
    newNode->next = NULL;
    newNode->value = value;
    return newNode;
}

list append1(list listHead, node* newNode) {
    /** debug **
    printf("%p\n", newNode);
    printf("%p\n", listHead);*/

    if(listHead == NULL) {
        listHead = newNode;
    }
    else {
        node* curr = listHead;
        while(curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = newNode;
    }
    /** debug **
    printf("\n%p\n", newNode);
    printf("%p\n", listHead);*/

    return listHead;
}

list append2(list listHead, node* newNode) {
    if(listHead==NULL) {
        return newNode;
    }
    else {
        listHead->next = append2(listHead->next, newNode);
        return listHead;
    }
}
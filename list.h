//
// Created by Kevin Funderburg on 2019-09-23.
//
#include "event.h"

#ifndef LIST_H
#define LIST_H

struct node {
    event *event;
    struct node *next;
};

// insert and delete operations.
void insert(struct node **head, event *event);
void delete(struct node **head, event *event);
void traverse(struct node *head);


#endif //LIST_H

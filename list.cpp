//
// Created by Kevin Funderburg on 2019-09-23.
//

#include <cstdlib>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "event.h"


// add a new task to the list of tasks
void insert(struct node *head, event *newEvent) {
    // add the new task to the list
    node *newNode = new node;

    newNode->event = newEvent;
    newNode->next = *head;
    *head = newNode;
}

// delete the selected task from the list
void deleteNode(struct node **head, Task *task)
{
    struct node *temp;
    struct node *prev;

    temp = *head;
    // special case - beginning of list
    if (strcmp(task->name,temp->task->name) == 0) {
        *head = (*head)->next;
    }
    else {
        // interior or last element in the list
        prev = *head;
        temp = temp->next;
        while (strcmp(task->name,temp->task->name) != 0) {
            prev = temp;
            temp = temp->next;
        }

        prev->next = temp->next;
    }
}

// traverse the list
void traverse(struct node *head) {
    struct node *temp;
    temp = head;

    while (temp != NULL) {
        printf("[%s] [%d] [%d]\n",temp->task->name, temp->task->priority, temp->task->burst);
        temp = temp->next;
    }
}

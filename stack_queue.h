/*
 * Basic Queue implementation to store markdown attribute
 * ordering. Used to track if brackets are closed in the correct
 * order to ensure valid html is created
 */
#ifndef QUEUE_H
#define QUEUE_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct node {
    char* data;
    struct node *next;
    struct node *prev;
};
typedef struct node Node;

Node* init_node(char* new_data);

void set_node_next(Node* node, Node* nextNode);

void set_node_prev(Node* node, Node* prevNode);

char* get_node_data(Node* node);

void free_node(Node* node);

struct stack_queue {
    Node* head;
    Node* tail;
};
typedef struct stack_queue Stack_Queue;

Stack_Queue* init_queue();

void free_queue(Stack_Queue* queue);

void enqueue(Stack_Queue* queue, Node* new_node);

void dequeue(Stack_Queue* queue);

void pop(Stack_Queue* queue);

Node* get_front(Stack_Queue* queue);

Node* get_top(Stack_Queue* queue);

void print_queue(Stack_Queue* queue);

#endif

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

struct queue {
    Node* head;
    Node* tail;
};
typedef struct queue Queue;

Queue* init_queue();

void free_queue(Queue* queue);

void enqueue(Queue* queue, Node* new_node);

void dequeue(Queue* queue);

Node* get_front(Queue* queue);

void print_queue(Queue* queue);

#endif

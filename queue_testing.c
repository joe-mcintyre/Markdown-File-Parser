#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

#define TEST_SIZE 10000

void stress_test_queue() {
    Queue* queue = init_queue();
    
    // Enqueue TEST_SIZE elements
    for (int i = 0; i < TEST_SIZE; i++) {
        char* data = (char*)malloc(20 * sizeof(char));
        sprintf(data, "Node-%d", i);
        enqueue(queue, init_node(data));
    }
    
    printf("Queue after enqueueing %d elements:\n", TEST_SIZE);
    print_queue(queue);
    
    // Dequeue half of the elements
    for (int i = 0; i < TEST_SIZE / 2; i++) {
        dequeue(queue);
    }
    
    printf("Queue after dequeuing half the elements:\n");
    print_queue(queue);
    
    // Dequeue the rest of the elements
    while (get_front(queue) != NULL) {
        dequeue(queue);
    }
    
    printf("Queue after completely dequeuing all elements:\n");
    print_queue(queue);
    
    // Ensure queue is empty
    if (get_front(queue) == NULL) {
        printf("Queue is empty as expected.\n");
    } else {
        printf("Error: Queue is not empty!\n");
    }
    
    free_queue(queue);
}

int main() {
    stress_test_queue();
    return 0;
}

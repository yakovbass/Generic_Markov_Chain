#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_
#include <stdlib.h> // For malloc()

/**
 * Node structure for linked list.
 * Each node contains a pointer to MarkovNode data and a pointer to the next node.
 */
typedef struct Node {
    struct MarkovNode *data;  // Pointer to the MarkovNode data stored in this node
    struct Node *next;        // Pointer to the next node in the list
} Node;

/**
 * LinkedList structure.
 * Maintains pointers to the first and last nodes, and tracks the size of the list.
 */
typedef struct LinkedList {
    Node *first;  // Pointer to the first node in the list
    Node *last;   // Pointer to the last node in the list
    int size;     // Current number of nodes in the list
} LinkedList;

/**
 * Add data to new node at the end of the given linked list.
 *
 * This function creates a new node, initializes it with the provided data,
 * and appends it to the end of the linked list. It also updates the list's
 * size and last pointer.
 *
 * @param link_list Link list to add data to
 * @param data Pointer to dynamically allocated data to be stored in the new node
 * @return 0 on success, 1 if memory allocation fails
 */
int add(LinkedList *link_list, void *data);

#endif //_LINKEDLIST_H_
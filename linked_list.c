#include "linked_list.h"

/**
 * Add data to a new node at the end of the linked list.
 *
 * This function performs the following steps:
 * 1. Allocates memory for a new node
 * 2. Initializes the node with the provided data and NULL next pointer
 * 3. If the list is empty, sets both first and last to the new node
 * 4. Otherwise, appends the new node to the end and updates the last pointer
 * 5. Increments the list size
 *
 * @param link_list Pointer to the LinkedList to add data to
 * @param data Pointer to the data to be stored in the new node
 * @return 0 on success, 1 if memory allocation fails
 */
int add(LinkedList *link_list, void *data)
{
    // Allocate memory for the new node
    Node *new_node = malloc(sizeof(Node));
    if (new_node == NULL)
    {
        return 1;  // Memory allocation failed
    }

    // Initialize the new node with the provided data and NULL next pointer
    *new_node = (Node) {data, NULL};

    // If the list is empty, set both first and last to the new node
    if (link_list->first == NULL)
    {
        link_list->first = new_node;
        link_list->last = new_node;
    }
    else
    {
        // Otherwise, append the new node to the end of the list
        link_list->last->next = new_node;
        link_list->last = new_node;
    }

    // Increment the size counter
    link_list->size++;
    return 0;  // Success
}
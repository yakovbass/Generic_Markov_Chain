#ifndef _MARKOV_CHAIN_H
#define _MARKOV_CHAIN_H

#include "linked_list.h"
#include <stdio.h>  // For printf(), sscanf()
#include <stdlib.h> // For exit(), malloc()
#include <stdbool.h> // for bool

// Error message for memory allocation failures
#define ALLOCATION_ERROR_MASSAGE \
"Allocation failure: Failed to allocate new memory\n"

/***************************/
/*   TYPE DEFINITIONS      */
/***************************/

// Function pointer type for printing data
typedef void (*print_func_t)(void * data);

// Function pointer type for comparing two data elements
typedef int (*comp_func_t)(void *first_data, void *second_data);

// Function pointer type for freeing data
typedef void (*free_data_t)(void *data);

// Function pointer type for copying data
typedef void *(*copy_func_t)(void *data);

// Function pointer type for checking if data represents a last state
typedef bool (*is_last_t)(void *data);

/***************************/
/*        STRUCTS          */
/***************************/

/**
 * MarkovNode structure.
 * Represents a single state in the Markov chain.
 * Contains the state data and a frequency list of possible next states.
 */
typedef struct MarkovNode {
    void *data;                              // Generic pointer to the state data
    struct MarkovNodeFrequency *frequency_list;  // Array of possible next states with frequencies
    int all_following;                       // Total count of all transitions from this node
    int following_count;                     // Number of distinct states that can follow this one
} MarkovNode;

/**
 * MarkovNodeFrequency structure.
 * Represents a possible transition from one state to another,
 * along with how frequently this transition occurs.
 */
typedef struct MarkovNodeFrequency {
    struct MarkovNode *markov_node;  // Pointer to the next state
    int frequency;                   // Number of times this transition has been observed
    struct MarkovNodeFrequency *next;  // Pointer to next frequency node (unused in current implementation)
    int num_of_nodes;                // Total number of nodes in frequency list
} MarkovNodeFrequency;

/**
 * MarkovChain structure.
 * Represents the entire Markov chain model.
 * Contains a database of all states and function pointers for generic operations.
 *
 * This structure allows the Markov chain to work with any data type by
 * providing custom functions for printing, comparing, freeing, copying,
 * and checking terminal states.
 */
typedef struct MarkovChain {
    LinkedList *database;        // Database of all MarkovNodes in the chain

    // Function pointer to print data (receives generic data pointer)
    print_func_t print_func;

    // Function pointer to compare two data elements
    // Returns: positive if first > second, negative if first < second, 0 if equal
    comp_func_t comp_func;

    // Function pointer to free dynamically allocated data
    free_data_t free_data;

    // Function pointer to create a deep copy of data
    // Returns: pointer to newly allocated copy of the data
    copy_func_t copy_func;

    // Function pointer to check if a state is terminal (last in sequence)
    // Returns: true if it's the last state, false otherwise
    is_last_t is_last;
} MarkovChain;

/***************************/
/*    FUNCTION PROTOTYPES  */
/***************************/

/**
 * Get one random state from the given markov_chain's database.
 *
 * Randomly selects a state from the database that is not a terminal state
 * (i.e., not a "last state" in a sequence). This is typically used to
 * start generating a random sequence.
 *
 * @param markov_chain Pointer to the MarkovChain
 * @return Pointer to a randomly selected MarkovNode that is not a last state
 */
MarkovNode* get_first_random_node(MarkovChain *markov_chain);

/**
 * Choose randomly the next state based on occurrence frequency.
 *
 * Uses the frequency list of the current node to probabilistically
 * select the next state. States that occur more frequently have
 * a higher probability of being selected.
 *
 * @param cur_markov_node Current MarkovNode to choose successor from
 * @return Pointer to the randomly chosen next MarkovNode
 */
MarkovNode* get_next_random_node(MarkovNode *cur_markov_node);

/**
 * Generate and print a random sequence from the markov chain.
 *
 * Starting from the given node (or a random starting node if NULL),
 * generates a sequence of states by repeatedly selecting the next state
 * based on transition frequencies. The sequence continues until either:
 * - A terminal state is reached, or
 * - The maximum length is reached
 *
 * The sequence must have at least 2 states.
 *
 * @param markov_chain Pointer to the MarkovChain
 * @param first_node Starting node (if NULL, a random non-terminal node is chosen)
 * @param max_length Maximum length of the sequence to generate
 */
void generate_random_sequence(MarkovChain *markov_chain, MarkovNode *first_node,
                              int max_length);

/**
 * Free all memory associated with the markov chain.
 *
 * This function performs a complete cleanup of the Markov chain:
 * 1. Frees all data stored in each MarkovNode using the free_data function
 * 2. Frees all frequency lists
 * 3. Frees all MarkovNode structures
 * 4. Frees all linked list nodes
 * 5. Frees the linked list itself
 * 6. Frees the MarkovChain structure
 * 7. Sets the pointer to NULL
 *
 * @param chain_ptr Pointer to pointer to the MarkovChain to free
 */
void free_markov_chain(MarkovChain **chain_ptr);

/**
 * Add the second node to the frequency list of the first node.
 *
 * This function records a transition from first_node to second_node.
 * If second_node already exists in first_node's frequency list,
 * its frequency counter is incremented. Otherwise, second_node is
 * added to the frequency list with a frequency of 1.
 *
 * @param first_node Pointer to the source MarkovNode
 * @param second_node Pointer to the destination MarkovNode
 * @param markov_chain Pointer to the MarkovChain (used for comparison function)
 * @return 0 on success, 1 on memory allocation failure
 */
int add_node_to_frequency_list(MarkovNode *first_node, MarkovNode *second_node,
                               MarkovChain *markov_chain);

/**
 * Check if data_ptr exists in the database.
 *
 * Searches through the markov chain's database to find a node
 * containing data that matches data_ptr (using the comparison function).
 *
 * @param markov_chain Pointer to the MarkovChain to search in
 * @param data_ptr Pointer to the data to search for
 * @return Pointer to the Node containing the matching data, or NULL if not found
 */
Node* get_node_from_database(MarkovChain *markov_chain, void *data_ptr);

/**
 * Get existing node from database or create a new one.
 *
 * Searches for data_ptr in the database. If found, returns the existing node.
 * If not found, creates a new MarkovNode with a copy of the data,
 * adds it to the end of the database, and returns the new node.
 *
 * @param markov_chain Pointer to the MarkovChain
 * @param data_ptr Pointer to the data to find or add
 * @return Pointer to the Node containing the data (new or existing)
 */
Node* add_to_database(MarkovChain *markov_chain, void *data_ptr);

#endif /* MARKOV_CHAIN_H */
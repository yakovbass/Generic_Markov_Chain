#include "markov_chain.h"
#include <string.h>

/***************************/
/*   CONSTANT DEFINITIONS  */
/***************************/

#define TRAVELLER 0      // Initial value for node traversal counter
#define FLAG 1           // Constant true value for infinite loop
#define WHICH_WORD 0     // Initial accumulator for frequency selection
#define LEN_OF_TWEET 1   // Initial sequence length counter

/**
 * Get a random number between 0 and max_number [0, max_number).
 *
 * @param max_number Upper bound (exclusive)
 * @return Random number in range [0, max_number)
 */
int get_random_number(int max_number)
{
    return rand() % max_number;
}

/**
 * Search for a node in the database that contains the specified data.
 *
 * Traverses the linked list database and uses the comparison function
 * to find a node whose data matches the provided data_ptr.
 *
 * @param markov_chain Pointer to the MarkovChain containing the database
 * @param data_ptr Pointer to the data to search for
 * @return Pointer to the matching Node, or NULL if not found
 */
Node* get_node_from_database(MarkovChain *markov_chain, void *data_ptr)
{
    Node *traveller = markov_chain->database->first;

    // Traverse the linked list
    while (traveller)
    {
        // Compare current node's data with the search data
        if (markov_chain->comp_func(traveller->data->data, data_ptr) == 0)
        {
            return traveller;  // Found matching node
        }
        traveller = traveller->next;
    }

    return NULL;  // Data not found in database
}

/**
 * Create a new MarkovNode and add it to the database.
 *
 * This function:
 * 1. Allocates memory for a new MarkovNode
 * 2. Creates a copy of the provided data using the copy function
 * 3. Initializes the node's frequency list and counters
 * 4. Adds the node to the end of the database
 *
 * @param markov_chain Pointer to the MarkovChain
 * @param data_ptr Pointer to the data to be copied and added
 * @return Pointer to the newly added Node, or NULL on failure
 */
Node* add_to_database(MarkovChain *markov_chain, void *data_ptr)
{
    // Allocate memory for new MarkovNode
    MarkovNode *new_markov_node = (MarkovNode*)malloc(sizeof(MarkovNode));
    if (new_markov_node == NULL)
    {
        fprintf(stdout, ALLOCATION_ERROR_MASSAGE);
        return NULL;
    }

    // Create a copy of the data using the chain's copy function
    new_markov_node->data = markov_chain->copy_func(data_ptr);
    if (new_markov_node->data == NULL)
    {
        return NULL;
    }

    // Initialize the node's frequency tracking
    new_markov_node->frequency_list = NULL;
    new_markov_node->following_count = 0;

    // Add the new node to the database linked list
    int addNode = add(markov_chain->database, new_markov_node);
    if (addNode == 1)
    {
        return NULL;  // Failed to add to list
    }

    return markov_chain->database->last;  // Return pointer to newly added node
}

/**
 * Initialize a new frequency list for a MarkovNode.
 *
 * Creates the first entry in the frequency list, recording a transition
 * from first_node to second_node with an initial frequency of 1.
 *
 * @param first_node Source node to initialize frequency list for
 * @param second_node Destination node to add to frequency list
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on allocation error
 */
int new_frequency_list(MarkovNode *first_node, MarkovNode *second_node)
{
    // Allocate memory for the first frequency list entry
    first_node->frequency_list = (MarkovNodeFrequency*)
            malloc(sizeof(MarkovNodeFrequency));
    if (first_node->frequency_list == NULL)
    {
        fprintf(stdout, ALLOCATION_ERROR_MASSAGE);
        return EXIT_FAILURE;
    }

    // Initialize the first frequency entry
    first_node->frequency_list[0].markov_node = second_node;
    first_node->frequency_list[0].frequency = 1;
    first_node->following_count = 1;
    first_node->frequency_list->num_of_nodes = 1;
    first_node->all_following = 1;

    return EXIT_SUCCESS;
}

/**
 * Increment the frequency of an existing transition.
 *
 * Searches the frequency list of first_node for second_node.
 * If found, increments its frequency counter.
 *
 * @param first_node Source node
 * @param second_node Destination node to search for
 * @param markov_chain Pointer to MarkovChain (for comparison function)
 * @return EXIT_SUCCESS if node found and updated, EXIT_FAILURE if not found
 */
int add_num_of_frequency(MarkovNode *first_node, MarkovNode *second_node,
                         MarkovChain *markov_chain)
{
    // Search through existing frequency list entries
    for (int i = 0; i < first_node->following_count; i++)
    {
        // Check if this entry matches the second_node
        if ((markov_chain->comp_func(
                first_node->frequency_list[i].markov_node->data,
                second_node->data)) == 0)
        {
            // Found it - increment frequency counters
            first_node->frequency_list[i].frequency++;
            first_node->all_following++;
            return EXIT_SUCCESS;
        }
    }

    return EXIT_FAILURE;  // Node not found in frequency list
}

/**
 * Add a node to the frequency list or update its frequency.
 *
 * This function handles recording a transition from first_node to second_node.
 * It either:
 * - Creates a new frequency list if one doesn't exist
 * - Increments the frequency if second_node already exists in the list
 * - Adds a new entry to the frequency list if second_node doesn't exist yet
 *
 * @param first_node Source node
 * @param second_node Destination node
 * @param markov_chain Pointer to MarkovChain
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on allocation error
 */
int add_node_to_frequency_list(MarkovNode *first_node, MarkovNode *second_node,
                               MarkovChain *markov_chain)
{
    // Case 1: No frequency list exists yet
    if (first_node->frequency_list == NULL)
    {
        int make_new_frequency_list = new_frequency_list(first_node, second_node);
        if (make_new_frequency_list == EXIT_FAILURE)
        {
            return EXIT_FAILURE;
        }
    }
    else
    {
        // Case 2: Frequency list exists - try to update existing entry
        int add_num_to_frequency_list =
                add_num_of_frequency(first_node, second_node, markov_chain);

        if (add_num_to_frequency_list == EXIT_SUCCESS)
        {
            return EXIT_SUCCESS;  // Successfully updated existing entry
        }

        // Case 3: Node not in list - need to add new entry
        first_node->frequency_list->num_of_nodes++;

        // Reallocate the frequency list to make room for new entry
        MarkovNodeFrequency *new_list = (MarkovNodeFrequency*)realloc(
                first_node->frequency_list,
                (first_node->following_count + 1) * sizeof(MarkovNodeFrequency));

        if (new_list == NULL)
        {
            fprintf(stdout, ALLOCATION_ERROR_MASSAGE);
            return EXIT_FAILURE;  // Memory reallocation failed
        }

        // Add the new node to the frequency list
        new_list[first_node->following_count].markov_node = second_node;
        new_list[first_node->following_count].frequency = 1;
        first_node->following_count++;
        first_node->all_following++;
        first_node->frequency_list = new_list;
    }

    return EXIT_SUCCESS;
}

/**
 * Get a random non-terminal starting node from the database.
 *
 * Randomly selects nodes from the database until finding one that
 * is not a terminal state (as determined by the is_last function).
 * This ensures sequences don't start with a terminal state.
 *
 * @param markov_chain Pointer to the MarkovChain
 * @return Pointer to a randomly selected non-terminal MarkovNode
 */
MarkovNode* get_first_random_node(MarkovChain *markov_chain)
{
    Node *node_to_return = markov_chain->database->first;

    // Keep selecting random nodes until we find a non-terminal one
    while (FLAG)
    {
        // Get random index in range [0, database size)
        int num = get_random_number(markov_chain->database->size);
        int traveller = TRAVELLER;

        // Traverse to the randomly selected node
        while (traveller != num)
        {
            node_to_return = node_to_return->next;
            traveller++;
        }

        void *one_word = node_to_return->data->data;

        // Check if this node is a terminal state
        if (!(markov_chain->is_last(one_word)))
        {
            break;  // Found a non-terminal node
        }

        // Reset to start of list for next iteration
        node_to_return = markov_chain->database->first;
    }

    return node_to_return->data;
}

/**
 * Select which node to transition to based on cumulative frequency.
 *
 * Uses a random number to probabilistically select the next state
 * based on transition frequencies. Higher frequency transitions have
 * a higher probability of being selected.
 *
 * @param first_node Current node
 * @param random_num Random number in range [0, total_frequency)
 * @return Pointer to the selected MarkovNodeFrequency
 */
MarkovNodeFrequency* which_node(MarkovNode *first_node, int random_num)
{
    int which_word = WHICH_WORD;

    // Iterate through frequency list, accumulating frequencies
    for (int i = 0; i < first_node->following_count; ++i)
    {
        MarkovNodeFrequency *freq = &(first_node->frequency_list[i]);
        which_word = which_word + freq->frequency;

        // If cumulative frequency exceeds random number, select this node
        if (which_word > random_num)
        {
            return freq;
        }
    }

    // Fallback to first node (shouldn't normally reach here)
    MarkovNodeFrequency *freq = &(first_node->frequency_list[0]);
    return freq;
}

/**
 * Get the next random node based on frequency distribution.
 *
 * Randomly selects the next state to transition to, with the probability
 * of each state proportional to its frequency in the current node's
 * frequency list.
 *
 * @param cur_markov_node Current MarkovNode
 * @return Pointer to the next randomly selected MarkovNode
 */
MarkovNode* get_next_random_node(MarkovNode *cur_markov_node)
{
    // Get random number in range [0, total_transitions)
    int random_num = get_random_number(cur_markov_node->all_following);

    // Select node based on cumulative frequency distribution
    MarkovNodeFrequency *freq_node = which_node(cur_markov_node, random_num);

    return freq_node->markov_node;
}

/**
 * Generate and print a random sequence from the Markov chain.
 *
 * Starting from first_node, generates a sequence by repeatedly selecting
 * the next state based on transition frequencies. Continues until either:
 * - A terminal state is reached (as determined by is_last)
 * - The maximum length is reached
 *
 * @param markov_chain Pointer to the MarkovChain
 * @param first_node Starting node for sequence generation
 * @param max_length Maximum number of states to generate
 */
void generate_random_sequence(MarkovChain *markov_chain, MarkovNode *first_node,
                              int max_length)
{
    int len_of_tweet = LEN_OF_TWEET;

    // Continue until terminal state or max length reached
    while ((!markov_chain->is_last(first_node->data)) && len_of_tweet < max_length)
    {
        // Select next node based on frequency distribution
        first_node = get_next_random_node(first_node);

        // Print the selected node's data
        markov_chain->print_func(first_node->data);

        len_of_tweet++;
    }
}

/**
 * Free all memory associated with the Markov chain.
 *
 * Performs a complete cleanup:
 * 1. Frees data in each MarkovNode using the chain's free_data function
 * 2. Frees frequency lists for each node
 * 3. Frees each MarkovNode structure
 * 4. Frees each linked list Node
 * 5. Frees the LinkedList structure
 * 6. Frees the MarkovChain structure
 * 7. Sets the pointer to NULL
 *
 * @param ptr_chain Pointer to pointer to the MarkovChain to free
 */
void free_markov_chain(MarkovChain **ptr_chain)
{
    // Check for NULL pointers
    if (ptr_chain == NULL || *ptr_chain == NULL)
    {
        return;
    }

    MarkovChain *chain = *ptr_chain;

    // If database is NULL, just free the chain itself
    if (chain->database == NULL)
    {
        free(chain);
        chain = NULL;
        return;
    }

    Node *traveller = chain->database->first;

    // Traverse through all nodes in the database
    while (traveller)
    {
        MarkovNode *our_node = traveller->data;

        if (our_node)
        {
            // Free the data stored in the MarkovNode
            if (our_node->data)
            {
                chain->free_data(our_node->data);
                our_node->data = NULL;
            }

            // Free the frequency list
            if (our_node->frequency_list)
            {
                free(our_node->frequency_list);
                our_node->frequency_list = NULL;
            }

            // Free the MarkovNode itself
            free(our_node);
            our_node = NULL;
        }

        // Move to next node and free current
        Node *next = traveller->next;
        free(traveller);
        traveller = NULL;
        traveller = next;
    }

    // Free remaining structures
    free(traveller);
    traveller = NULL;
    free(chain->database);
    chain->database = NULL;
    free(chain);
    chain = NULL;
    *ptr_chain = NULL;
}
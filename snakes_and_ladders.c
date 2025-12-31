#include <string.h> // For strlen(), strcmp(), strcpy()
#include "markov_chain.h"

/***************************/
/*   MACRO DEFINITIONS     */
/***************************/

#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))  // Returns maximum of two values

#define EMPTY -1                    // Indicates no snake or ladder on a cell
#define BOARD_SIZE 100              // Total number of cells on the game board
#define CURR_WALK 1                 // Initial walk counter value
#define MAX_GENERATION_LENGTH 60    // Maximum length of generated path
#define BASE_TEN 10                 // Base for string to integer conversion
#define DICE_MAX 6                  // Maximum value on a dice
#define HEAD 0                      // Index for source cell in transition
#define TAIL 1                      // Index for destination cell in transition
#define NUM_OF_TRANSITIONS 20       // Total number of snakes and ladders
#define NUM_ARGS 3                  // Expected number of command line arguments
#define NUM_ARGS_ERROR "Usage: invalid number of arguments"  // Error message

/**
 * Array of game transitions (snakes and ladders).
 * Each tuple (x,y) represents:
 * - A ladder from x to y if x < y
 * - A snake from x to y if x > y
 */
const int transitions[][2] = {{13, 4},
                              {85, 17},
                              {95, 67},
                              {97, 58},
                              {66, 89},
                              {87, 31},
                              {57, 83},
                              {91, 25},
                              {28, 50},
                              {35, 11},
                              {8,  30},
                              {41, 62},
                              {81, 43},
                              {69, 32},
                              {20, 39},
                              {33, 70},
                              {79, 99},
                              {23, 76},
                              {15, 47},
                              {61, 14}};

/***************************/
/*   STRUCTURE DEFINITIONS */
/***************************/

/**
 * Cell structure representing a square on the game board.
 */
typedef struct Cell {
    int number;      // Cell number (1-100)
    int ladder_to;   // Destination of ladder (-1 if no ladder)
    int snake_to;    // Destination of snake (-1 if no snake)
} Cell;

/***************************/
/*   FUNCTION DEFINITIONS  */
/***************************/

/**
 * Validate the number of command line arguments.
 *
 * @param args Number of arguments provided
 * @return EXIT_SUCCESS if valid, EXIT_FAILURE otherwise
 */
int is_right_num_args(int args)
{
    if (args != NUM_ARGS)
    {
        fprintf(stdout, NUM_ARGS_ERROR);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

/**
 * Handle errors and perform cleanup.
 *
 * Prints error message and frees the Markov chain if provided.
 *
 * @param error_msg Error message to display
 * @param database Pointer to MarkovChain to free (can be NULL)
 * @return EXIT_FAILURE
 */
int handle_error_snakes(char *error_msg, MarkovChain **database)
{
    printf("%s", error_msg);
    if (database != NULL)
    {
        free_markov_chain(database);
    }
    return EXIT_FAILURE;
}

/**
 * Create and initialize the game board.
 *
 * Allocates memory for all cells and initializes them with:
 * - Cell numbers (1-100)
 * - Snake and ladder destinations from the transitions array
 *
 * @param cells Array of pointers to Cell structures
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on allocation error
 */
int create_board(Cell *cells[BOARD_SIZE])
{
    // Allocate and initialize all cells
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        cells[i] = malloc(sizeof(Cell));
        if (cells[i] == NULL)
        {
            // Free previously allocated cells on error
            for (int j = 0; j < i; j++)
            {
                free(cells[j]);
            }
            handle_error_snakes(ALLOCATION_ERROR_MASSAGE, NULL);
            return EXIT_FAILURE;
        }
        // Initialize cell with number and no snakes/ladders
        *(cells[i]) = (Cell) {i + 1, EMPTY, EMPTY};
    }

    // Add snakes and ladders based on transitions array
    for (int i = 0; i < NUM_OF_TRANSITIONS; i++)
    {
        int from = transitions[i][0];
        int to = transitions[i][1];

        if (from < to)
        {
            // This is a ladder (going up)
            cells[from - 1]->ladder_to = to;
        }
        else
        {
            // This is a snake (going down)
            cells[from - 1]->snake_to = to;
        }
    }
    return EXIT_SUCCESS;
}

/**
 * Fill the Markov chain database with board transitions.
 *
 * Creates the Markov chain model for the Snakes and Ladders game:
 * 1. Creates the game board
 * 2. Adds all cells to the database
 * 3. For each cell, adds transitions to possible next cells
 *    - If cell has snake/ladder: adds single transition to destination
 *    - Otherwise: adds transitions for all possible dice rolls (1-6)
 *
 * @param markov_chain Pointer to the MarkovChain to populate
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on error
 */
int fill_database_snakes(MarkovChain *markov_chain)
{
    Cell *cells[BOARD_SIZE];

    // Create the game board
    if (create_board(cells) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    MarkovNode *from_node = NULL, *to_node = NULL;
    size_t index_to;

    // Add all cells to the database
    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        add_to_database(markov_chain, cells[i]);
    }

    // Build transition relationships
    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        from_node = get_node_from_database(markov_chain, cells[i])->data;

        // Check if this cell has a snake or ladder
        if (cells[i]->snake_to != EMPTY || cells[i]->ladder_to != EMPTY)
        {
            // Cell has snake or ladder - single transition to destination
            index_to = MAX(cells[i]->snake_to, cells[i]->ladder_to) - 1;
            to_node = get_node_from_database(markov_chain, cells[index_to])->data;
            add_node_to_frequency_list(from_node, to_node, markov_chain);
        }
        else
        {
            // Regular cell - add transitions for all possible dice rolls
            for (int j = 1; j <= DICE_MAX; j++)
            {
                index_to = ((Cell*)(from_node->data))->number + j - 1;

                // Don't add transitions beyond the board
                if (index_to >= BOARD_SIZE)
                {
                    break;
                }

                to_node = get_node_from_database(markov_chain, cells[index_to])->data;
                int res = add_node_to_frequency_list(from_node, to_node, markov_chain);

                if (res == EXIT_FAILURE)
                {
                    return EXIT_FAILURE;
                }
            }
        }
    }

    // Free temporary cell array
    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        free(cells[i]);
    }

    return EXIT_SUCCESS;
}

/**
 * Comparison function for Cell data.
 *
 * Compares two cells by their number values.
 *
 * @param first_data Pointer to first cell number
 * @param second_data Pointer to second cell number
 * @return Difference between first and second cell numbers
 */
int comp_fun(void *first_data, void *second_data)
{
    const int *first_num = (const int *)first_data;
    const int *second_num = (const int *)second_data;
    return *first_num - *second_num;
}

/**
 * Print function for Cell data.
 *
 * Prints the cell number with appropriate formatting based on whether
 * it contains a snake, ladder, or is a regular cell.
 *
 * @param data Pointer to cell number to print
 */
void ptint_func(void *data)
{
    int *d = (int *)data;

    // Check if this cell has a snake or ladder
    for (size_t i = 0; i < NUM_OF_TRANSITIONS; i++)
    {
        if (transitions[i][HEAD] == *d)
        {
            if (transitions[i][TAIL] < transitions[i][HEAD])
            {
                // This cell has a snake
                fprintf(stdout, " [%d] -snake to->", *d);
                return;
            }
            else
            {
                // This cell has a ladder
                fprintf(stdout, " [%d] -ladder to->", *d);
                return;
            }
        }
    }

    // Check if this is the final cell
    if (*d == BOARD_SIZE)
    {
        fprintf(stdout, " [%d]", *d);
        return;
    }

    // Regular cell
    fprintf(stdout, " [%d] ->", *d);
}

/**
 * Copy function for Cell data.
 *
 * Creates a deep copy of a cell number.
 *
 * @param data Pointer to cell number to copy
 * @return Pointer to newly allocated copy, or NULL on failure
 */
void *copy_func(void *data)
{
    const int *num = (const int *)data;
    int *dup = (int *)malloc(sizeof(int));
    if (dup != NULL)
    {
        *dup = *num;
    }
    return dup;
}

/**
 * Free function for Cell data.
 *
 * Frees the memory allocated for a cell number.
 *
 * @param data Pointer to cell number to free
 */
void free_data(void *data)
{
    free((int *)data);
}

/**
 * Check if a cell is the last state (cell 100).
 *
 * @param data Pointer to cell number
 * @return true if cell number is BOARD_SIZE (100), false otherwise
 */
bool is_last(void *data)
{
    int *num = (int *)data;
    return (*num == BOARD_SIZE);
}

/**
 * Main function - Snakes and Ladders game simulator.
 *
 * Generates random game paths using a Markov chain model.
 *
 * Usage: ./snakes_and_ladders <seed> <num_paths>
 *   seed: Random seed for reproducible results
 *   num_paths: Number of random game paths to generate
 *
 * @param argc Number of command line arguments
 * @param argv Array of argument strings
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on error
 */
int main(int argc, char *argv[])
{
    // Validate number of arguments
    int check_args = is_right_num_args(argc);
    if (check_args == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    // Allocate and initialize LinkedList
    LinkedList *list = (LinkedList *)malloc(sizeof(LinkedList));
    if (list == NULL)
    {
        fprintf(stdout, ALLOCATION_ERROR_MASSAGE);
        return EXIT_FAILURE;
    }
    list->first = NULL;
    list->last = NULL;
    list->size = 0;

    // Allocate and initialize MarkovChain
    MarkovChain *markov_chain = (MarkovChain *)malloc(sizeof(MarkovChain));
    if (markov_chain == NULL)
    {
        fprintf(stdout, ALLOCATION_ERROR_MASSAGE);
        return EXIT_FAILURE;
    }

    // Set up MarkovChain with appropriate functions for Cell data
    markov_chain->database = list;
    markov_chain->copy_func = copy_func;
    markov_chain->comp_func = comp_fun;
    markov_chain->is_last = is_last;
    markov_chain->print_func = ptint_func;
    markov_chain->free_data = free_data;

    // Set random seed from command line argument
    long seed = strtol(argv[1], NULL, BASE_TEN);
    srand(seed);

    // Build the Markov chain database
    int make_the_chain = fill_database_snakes(markov_chain);
    if (make_the_chain == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    // Get number of paths to generate from command line
    long max_paths = strtol(argv[2], NULL, BASE_TEN);
    int curr_walk = CURR_WALK;

    // Generate and print random game paths
    while (curr_walk <= max_paths)
    {
        // Always start from cell 1 (first cell)
        MarkovNode *first_node = markov_chain->database->first->data;
        fprintf(stdout, "Random Walk %d: [%d] ->",
                curr_walk, *(int *)first_node->data);

        // Generate the random path
        generate_random_sequence(markov_chain, first_node, MAX_GENERATION_LENGTH);

        curr_walk++;
        fprintf(stdout, "\n");
    }

    // Clean up and free all allocated memory
    free_markov_chain(&markov_chain);

    return EXIT_SUCCESS;
}
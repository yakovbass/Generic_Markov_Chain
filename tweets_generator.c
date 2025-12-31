#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "markov_chain.h"
#include "linked_list.h"

/***************************/
/*   MACRO DEFINITIONS     */
/***************************/

#define FILE_PATH_ERROR "Error: incorrect file path"  // Error for invalid file path
#define NUM_ARGS_ERROR "Usage: invalid number of arguments"  // Error for wrong arg count
#define DELIMITERS " \n\t\r"       // Word delimiters for tokenization
#define MAX_LEN_ROW 1000           // Maximum length of a line in input file
#define START_CHAIN 0              // Initial word counter value
#define BASE_TEN 10                // Base for string to integer conversion
#define LEN_OF_TWEETS 1            // Initial tweet counter value
#define MAX_LEN_OF_TWEET 20        // Maximum words per generated tweet
#define MIN_NUM_ARGS 4             // Minimum command line arguments
#define MAX_NUM_ARGS 5             // Maximum command line arguments

/***************************/
/*   FUNCTION DEFINITIONS  */
/***************************/

/**
 * Validate command line arguments and file path.
 *
 * Checks if the correct number of arguments was provided and if the
 * specified file path can be opened.
 *
 * @param path File path to validate
 * @param args Number of command line arguments
 * @return EXIT_SUCCESS if valid, EXIT_FAILURE otherwise
 */
int is_right_path(char *path, int args)
{
    // Check argument count
    if (args != MIN_NUM_ARGS && args != MAX_NUM_ARGS)
    {
        fprintf(stdout, NUM_ARGS_ERROR);
        return EXIT_FAILURE;
    }

    // Attempt to open file
    FILE *input_file = fopen(path, "r");
    if (input_file == NULL)
    {
        fprintf(stdout, FILE_PATH_ERROR);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/**
 * Fill database without word limit.
 *
 * Reads the entire input file and builds a Markov chain from all words.
 * Words are tokenized and added to the database, with transitions recorded
 * between consecutive words (except after sentence-ending periods).
 *
 * @param fp File pointer to read from
 * @param markov_chain Pointer to MarkovChain to populate
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on error
 */
int fill_without_limit(FILE *fp, MarkovChain *markov_chain)
{
    int start_chain = START_CHAIN;

    // Allocate buffer for reading lines
    char *row = malloc(sizeof(char) * MAX_LEN_ROW);
    if (row == NULL)
    {
        fprintf(stdout, ALLOCATION_ERROR_MASSAGE);
        return EXIT_FAILURE;
    }

    MarkovNode *save_last_one = NULL;  // Track previous word

    // Read file line by line
    while (fgets(row, MAX_LEN_ROW, fp) != NULL)
    {
        // Tokenize line into words
        void *token = strtok(row, DELIMITERS);

        while (token)
        {
            // Check if word already exists in database
            Node *has_node = get_node_from_database(markov_chain, (char *)token);

            if (has_node == NULL)
            {
                // Word doesn't exist - add it to database
                has_node = add_to_database(markov_chain, (char *)token);
                if (has_node == NULL)
                {
                    return EXIT_FAILURE;
                }
            }

            // Add transition from previous word to current word
            if (save_last_one != NULL)
            {
                char *s = (char *)save_last_one->data;
                unsigned long len_s = strlen(s);

                // Only add transition if previous word doesn't end with period
                if (s[len_s - 1] != '.')
                {
                    int add_to_frequency_list = add_node_to_frequency_list(
                            save_last_one, has_node->data, markov_chain);

                    if (add_to_frequency_list == EXIT_FAILURE)
                    {
                        return EXIT_FAILURE;
                    }
                }
            }

            // Update previous word tracker
            save_last_one = has_node->data;

            // Get next token
            token = strtok(NULL, DELIMITERS);
            start_chain++;
        }
    }

    // Free buffer
    free(row);
    row = NULL;

    return EXIT_SUCCESS;
}

/**
 * Fill database with a limited number of words.
 *
 * Reads up to words_to_read words from the input file and builds a
 * Markov chain. Words are tokenized and added to the database, with
 * transitions recorded between consecutive words.
 *
 * @param fp File pointer to read from
 * @param words_to_read Maximum number of words to read
 * @param markov_chain Pointer to MarkovChain to populate
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on error
 */
int fill_database(FILE *fp, long words_to_read, MarkovChain *markov_chain)
{
    int start_chain = START_CHAIN;

    // Allocate buffer for reading lines
    char *row = malloc(MAX_LEN_ROW * sizeof(char));
    if (row == NULL)
    {
        fprintf(stdout, ALLOCATION_ERROR_MASSAGE);
        return EXIT_FAILURE;
    }

    MarkovNode *save_last_one = NULL;  // Track previous word

    // Read file line by line until word limit reached
    while (fgets(row, MAX_LEN_ROW, fp) != NULL && start_chain < words_to_read)
    {
        // Tokenize line into words
        void *token = strtok(row, DELIMITERS);

        while (token && start_chain < words_to_read)
        {
            // Check if word already exists in database
            Node *has_node = get_node_from_database(markov_chain, (char *)token);

            if (has_node == NULL)
            {
                // Word doesn't exist - add it to database
                has_node = add_to_database(markov_chain, token);
            }

            // Add transition from previous word to current word
            if (save_last_one != NULL)
            {
                char *s = (char *)save_last_one->data;
                unsigned long len_s = strlen(s);

                // Only add transition if previous word doesn't end with period
                if (s[len_s - 1] != '.')
                {
                    int add_to_frequency_list = add_node_to_frequency_list(
                            save_last_one, has_node->data, markov_chain);

                    if (add_to_frequency_list == EXIT_FAILURE)
                    {
                        return EXIT_FAILURE;
                    }
                }
            }

            // Update previous word tracker
            save_last_one = has_node->data;

            // Get next token
            token = strtok(NULL, DELIMITERS);
            start_chain++;
        }
    }

    // Free buffer
    free(row);
    row = NULL;

    return EXIT_SUCCESS;
}

/**
 * Print function for string data.
 *
 * Prints a word followed by a space.
 *
 * @param data Pointer to string to print
 */
void (check_print_func)(void *data)
{
    fprintf(stdout, "%s ", (char *)data);
}

/**
 * Comparison function for string data.
 *
 * Compares two strings lexicographically.
 *
 * @param first_data Pointer to first string
 * @param second_data Pointer to second string
 * @return Comparison result (negative, zero, or positive)
 */
int check_comp_fun(void *first_data, void *second_data)
{
    return strcmp((char *)first_data, (char *)second_data);
}

/**
 * Copy function for string data.
 *
 * Creates a deep copy of a string.
 *
 * @param data Pointer to string to copy
 * @return Pointer to newly allocated copy, or NULL on failure
 */
void *check_copy_func(void *data)
{
    char *s = (char *)data;
    size_t len = strlen(s) + 1;
    char *dup = malloc(len);
    if (dup != NULL)
    {
        strcpy(dup, s);
    }
    return dup;
}

/**
 * Free function for string data.
 *
 * Frees the memory allocated for a string.
 *
 * @param data Pointer to string to free
 */
void check_free_data(void *data)
{
    free((char *)data);
}

/**
 * Check if a word is a sentence ending (ends with period).
 *
 * @param data Pointer to string to check
 * @return true if string ends with '.', false otherwise
 */
bool check_is_last(void *data)
{
    char *s = (char *)data;
    return (s[strlen(s) - 1] == '.');
}

/**
 * Main function - Tweet generator using Markov chains.
 *
 * Generates random tweet-like text sequences based on a text corpus.
 * The program learns word patterns from an input file and generates
 * new sentences that follow similar patterns.
 *
 * Usage: ./tweets_generator <seed> <num_tweets> <file_path> [words_to_read]
 *   seed: Random seed for reproducible results
 *   num_tweets: Number of tweets to generate
 *   file_path: Path to input text file
 *   words_to_read: (Optional) Maximum words to read from file
 *
 * @param args Number of command line arguments
 * @param argv Array of argument strings
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on error
 */
int main(int args, char *argv[])
{
    // Validate arguments and file path
    if (is_right_path(argv[3], args) == EXIT_FAILURE)
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

    // Set up MarkovChain with appropriate functions for string data
    markov_chain->database = list;
    markov_chain->free_data = check_free_data;
    markov_chain->print_func = check_print_func;
    markov_chain->is_last = check_is_last;
    markov_chain->comp_func = check_comp_fun;
    markov_chain->copy_func = check_copy_func;

    // Set random seed from command line argument
    long seed = strtol(argv[1], NULL, BASE_TEN);
    srand(seed);

    // Open input file
    FILE *input_file = fopen(argv[3], "r");
    int make_the_chain = EXIT_FAILURE;

    // Build database - with or without word limit
    if (args == MAX_NUM_ARGS)
    {
        // Word limit specified
        long long_value = strtol(argv[4], NULL, BASE_TEN);
        make_the_chain = fill_database(input_file, long_value, markov_chain);
    }
    else
    {
        // No word limit - read entire file
        make_the_chain = fill_without_limit(input_file, markov_chain);
    }

    if (make_the_chain == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    // Get number of tweets to generate from command line
    long max_tweets = strtol(argv[2], NULL, BASE_TEN);
    int num_tweets = LEN_OF_TWEETS;

    // Generate and print tweets
    while (num_tweets <= max_tweets)
    {
        fprintf(stdout, "Tweet %d: ", num_tweets);

        // Get random starting word (not ending with period)
        MarkovNode *first_node = get_first_random_node(markov_chain);
        markov_chain->print_func(first_node->data);

        // Generate rest of the tweet
        generate_random_sequence(markov_chain, first_node, MAX_LEN_OF_TWEET);

        num_tweets++;
        fprintf(stdout, "\n");
    }

    // Clean up and free all allocated memory
    free_markov_chain(&markov_chain);
    fclose(input_file);

    return EXIT_SUCCESS;
}
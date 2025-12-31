# Markov Chain Text Generator

A generic Markov chain implementation in C that can generate random sequences based on learned patterns from input data. This project includes two main applications: a tweet generator and a Snakes and Ladders game simulator.


## Overview

This project implements a generic Markov chain data structure that can work with any data type through function pointers. A Markov chain is a stochastic model that transitions from one state to another based on probability distributions learned from historical data.

The implementation includes:
- **Generic Markov Chain Library**: Core data structures and algorithms
- **Tweet Generator**: Generates random tweet-like text based on a corpus
- **Snakes and Ladders Simulator**: Simulates random game paths

## Features

-  Generic implementation using function pointers (works with any data type)
-  Frequency-based probability distribution for state transitions
-  Configurable random seed for reproducible results
-  Efficient linked list-based storage
-  Complete memory management with no leaks
-  Comprehensive documentation and comments

## Project Structure

```
.
├── linked_list.h          # Linked list interface
├── linked_list.c          # Linked list implementation
├── markov_chain.h         # Markov chain interface
├── markov_chain.c         # Markov chain implementation
├── tweets_generator.c     # Text generation application
├── snakes_and_ladders.c   # Game simulation application
├── Makefile               # Build automation
└── README.md              # This file
```

## How It Works

### Markov Chain Algorithm

1. **Learning Phase**: The program reads input data and builds a database of states and their transitions
2. **State Transitions**: For each state, it tracks which states can follow and how frequently
3. **Generation Phase**: Starting from a random non-terminal state, it probabilistically selects the next state based on learned frequencies
4. **Termination**: Generation continues until either a terminal state is reached or maximum length is hit

### Probability Selection

When transitioning from state A to the next state:
- Each possible next state has a frequency (how many times it followed A in the training data)
- A random number is generated in the range [0, total_frequency)
- The next state is selected based on cumulative frequency distribution

## Compilation

This project includes a Makefile for easy compilation.

### Using Make (Recommended)

**Build Tweet Generator:**
```bash
make tweets_generator
```

**Build Snakes and Ladders:**
```bash
make snakes_and_ladders
```

**Clean build files:**
```bash
make clean
```

### Manual Compilation

If you prefer to compile manually:

**Tweet Generator:**
```bash
gcc tweets_generator.c markov_chain.c linked_list.c -o tweets_generator
```

**Snakes and Ladders:**
```bash
gcc snakes_and_ladders.c markov_chain.c linked_list.c -o snakes_and_ladders
```

**Recommended flags for development:**
```bash
gcc -Wall -Wextra -Wvla -std=c99 tweets_generator.c markov_chain.c linked_list.c -o tweets_generator
```

## Usage

### Tweet Generator

Generates random text sequences based on an input text file.

**Syntax:**
```bash
./tweets_generator <seed> <num_tweets> <file_path> [words_to_read]
```

**Parameters:**
- `seed`: Random seed for reproducible results (integer)
- `num_tweets`: Number of tweets to generate (integer)
- `file_path`: Path to input text file
- `words_to_read`: (Optional) Maximum number of words to read from file

**Example:**
```bash
./tweets_generator 42 5 corpus.txt 1000
```

This generates 5 tweets using seed 42, reading the first 1000 words from `corpus.txt`.

**Output Format:**
```
Tweet 1: The quick brown fox jumps over the lazy dog.
Tweet 2: Machine learning is a fascinating field of study.
...
```

### Snakes and Ladders

Simulates random game paths through a Snakes and Ladders board.

**Syntax:**
```bash
./snakes_and_ladders <seed> <num_paths>
```

**Parameters:**
- `seed`: Random seed for reproducible results (integer)
- `num_paths`: Number of game paths to generate (integer)

**Example:**
```bash
./snakes_and_ladders 123 3
```

**Output Format:**
```
Random Walk 1: [1] -> [3] -> [8] -ladder to-> [30] -> [35] -snake to-> [11] -> ... [100]
Random Walk 2: [1] -> [5] -> [13] -snake to-> [4] -> ... [100]
...
```

## Code Structure

### Core Components

#### `LinkedList` (linked_list.h/c)
- Generic singly linked list implementation
- Supports adding nodes to the end
- Tracks first, last, and size

#### `MarkovChain` (markov_chain.h/c)
Generic Markov chain implementation with function pointers for:
- `print_func`: Printing data
- `comp_func`: Comparing two data elements
- `free_data`: Freeing allocated data
- `copy_func`: Creating deep copies of data
- `is_last`: Checking if a state is terminal

**Key Functions:**
- `get_node_from_database()`: Search for existing state
- `add_to_database()`: Add new state to the chain
- `add_node_to_frequency_list()`: Record state transition
- `get_first_random_node()`: Get random non-terminal starting state
- `get_next_random_node()`: Probabilistically select next state
- `generate_random_sequence()`: Generate a complete sequence
- `free_markov_chain()`: Complete memory cleanup

### Applications

#### Tweet Generator (tweets_generator.c)
- Reads text from file and tokenizes by whitespace
- Treats words ending with '.' as terminal states
- Generates sentences up to 20 words or until a period is reached

#### Snakes and Ladders (snakes_and_ladders.c)
- Models a 100-cell game board
- 20 snakes and ladders predefined in transitions array
- Simulates dice rolls (1-6) for regular cells
- Forced transitions for snake/ladder cells

## Examples

### Example 1: Tweet Generation

**Input File (corpus.txt):**
```
The cat sat on the mat. The dog sat on the log.
The cat and the dog are friends.
```

**Command:**
```bash
./tweets_generator 42 3 corpus.txt
```

**Possible Output:**
```
Tweet 1: The cat sat on the dog are friends.
Tweet 2: The dog sat on the mat.
Tweet 3: The cat and the log.
```

### Example 2: Game Simulation

**Command:**
```bash
./snakes_and_ladders 100 2
```

**Possible Output:**
```
Random Walk 1: [1] -> [4] -> [6] -> [8] -ladder to-> [30] -> [33] -ladder to-> [70] -> [75] -> [79] -ladder to-> [99] -> [100]
Random Walk 2: [1] -> [2] -> [8] -ladder to-> [30] -> [35] -snake to-> [11] -> [15] -ladder to-> [47] -> [52] -> ... [100]
```

## Requirements

- C compiler with C99 support (gcc recommended)
- Standard C library
- POSIX-compliant system (for file I/O)

**Compiler Flags:**
- `-Wall`: Enable all warnings
- `-Wextra`: Enable extra warnings
- `-Wvla`: Warn about variable-length arrays
- `-std=c99`: Use C99 standard

## Memory Management

The implementation carefully manages all dynamic memory:
- All allocated memory is freed before program termination
- `free_markov_chain()` performs complete cleanup of all structures
- No memory leaks when run through valgrind

## Design Patterns

### Generic Programming via Function Pointers
The Markov chain is designed to work with any data type by requiring the user to provide:
- Comparison function
- Print function  
- Copy function
- Free function
- Terminal state checker

This allows the same core implementation to handle strings (tweets) and integers (game cells).

### Separation of Concerns
- `linked_list`: Generic list operations
- `markov_chain`: Core Markov chain logic
- `tweets_generator`/`snakes_and_ladders`: Specific applications

## Contributing

Contributions are welcome! Please ensure:
- Code follows existing style and conventions
- All functions are documented
- No memory leaks (test with valgrind)
- Compilation produces no warnings




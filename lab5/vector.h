/**********************************************************
* Filename: vector.h
* Author: Zach Campbell
* Description: Function declarations for Vector Calculator
* Date: 10/1/26
***********************************************************/

#ifndef VECTOR_H_
#define VECTOR_H_

#include "struct.h"

#define MAX_VECTORS 10

// Operation types
#define OP_ADD '+'
#define OP_SUB '-'
#define OP_MULT '*'
#define OP_DOT '.'
#define OP_CROSS 'x'

// Command types
typedef enum command
{
    CMD_OPERATION,
    CMD_HELP,
    CMD_LIST,
    CMD_CLEAR,
    CMD_QUIT
} Command;

// Vector mathematics
void addvect (Vector *a, Vector *b, Vector *result); // stores sum of a and b in result
void subvect (Vector *a, Vector *b, Vector *result); // stores difference of a and b in result
void scalmult (Vector *a, double scalar, Vector *result); //stores product of a and scalar in result (int * vector/ vector * int)

double dotprod (Vector *a, Vector *b); // stores dot product of a and b in result [scalar result]
void crossprod (Vector *a, Vector *b, Vector *result); // stores cross product of a and b in result

// Operation helper
int resopvect (Vector *a, Vector *b, char operation, double scalar, Vector *result, double *scalar_result); // operation + assignment, assigns value after performing operation

// Display functions
void disvect (Vector vector); // displays vector given name
void list (void); // lists all vectors

// Memory functions
void clear (void); // empties all stored vectors
int add_vector (Vector new_vector); // adds a vector or updates existing vector with same name
int findvect (const char *name); // returns the index of a stored vector, or -1 if not found
int get_vector (const char *name, Vector *result);  // copies a stored vector into result, returns -1 if not found.

// UI Functions
int tokenize (char *input, char *tokens[]); // string tokenization helper function
int is_number (const char *token); // Checks if token is number
void getvect (char *input); // processes vector calculator commands
void quit (void); // quits program
void help (void); // lists commands
void error (void); // lists errors (i.e. can't find var, too many vectors, > 3 dimensions, etc.)

// Command handling
Command get_command (const char *input); // determines top level commands


#endif // VECTOR_H_



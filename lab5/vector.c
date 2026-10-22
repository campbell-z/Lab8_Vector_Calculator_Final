/***************************************************************
* Filename: vector.c
* Author: Zach Campbell
* Description: Function definitions for vector calculator
* Date: 10/1/26
* Version: 2.2
* Notes:
*   - Cleaned up style
*   - Made comments more accurate
*   - Added input-validation safeguards
*       - Protecting findvect() against a null name
*       - Protecting get_vector() against a null pointer
*   - gcc -Wall -Wextra -Wpedantic -Werror -o main vectop.c vector.c -lm
* Run:
*   - ./main
****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include "struct.h"
#include "vector.h"

// Reuse temporary result slots to avoid excessive vector reallocation
static Vector vectors[MAX_VECTORS];

// Temporary vector storage for operation results
static Vector new_vect[MAX_VECTORS];

// Current working-vector position
static int new_vect_index = 0;

// Internal command handlers
static void display_vector_command(char *tokens[]);
static void handle_operation(char *tokens[]);
static void handle_assignment(char *tokens[], int count);
static void handle_assignment_operation(char *tokens[]);
static int valid_vector_name(const char *name);

// Returns nonzero when name fits in vector.name and is not empty
static int valid_vector_name(const char *name)
{
    return name != NULL && name[0] != '\0' && strlen(name) < sizeof vectors[0].name;
}

// Vector memory functions

int findvect(const char *name) 
{
    if (name == NULL)
    {
        return -1;
    }

    for (int i = 0; i < MAX_VECTORS; i++)
    {
        if (strcmp(vectors[i].name, name) == 0)
        {
            return i;
        }
    }

    return -1;
}


int add_vector(Vector new_vector)
{
    int index;

    if (memchr(new_vector.name, '\0', sizeof new_vector.name) == NULL || new_vector.name[0] == '\0')
    {
        return -1;
    }

   index = findvect(new_vector.name);

   if (index >= 0)
   {
    vectors [index] = new_vector;
    return 0;
   }

   for (int i = 0; i < MAX_VECTORS; i++)
   {
    if (vectors[i].name[0] == '\0')
    {
        vectors[i] = new_vector;
        return 0;
    }
   }

   return -1;

}

int get_vector(const char *name, Vector *result)
{

    if (name == NULL || result == NULL)
    {
        return -1;
    }

    int index = findvect(name);
    
    if (index < 0)
    {
        return -1;
    }

    *result = vectors[index];

    return 0;
}

void clear(void) 
{
    for (int i = 0; i < MAX_VECTORS; i++)
    {
        vectors[i].name[0] = '\0';
    }
}

// Vector operations

void addvect(Vector *a, Vector *b, Vector *result) 
{ 
    result->x = a->x + b->x;
    result->y = a->y + b->y;
    result->z = a->z + b->z;
}

void subvect(Vector *a, Vector *b, Vector *result) 
{
    result->x = a->x - b->x;
    result->y = a->y - b->y;
    result->z = a->z - b->z;
}

void scalmult(Vector *a, double scalar, Vector *result) 
{
    result->x = a->x * scalar;
    result->y = a->y * scalar;
    result->z = a->z * scalar;
}

double dotprod(Vector *a, Vector *b)
{
    return (a->x * b->x) 
    + (a->y * b->y)
    + (a->z * b->z);
}

void crossprod(Vector *a, Vector *b, Vector *result)
{
    result->x = (a->y * b->z) - (a->z * b->y);
    result->y = (a->z * b->x) - (a->x * b->z);
    result->z = (a->x * b->y) - (a->y * b->x);
}

// Operation helper

int resopvect(Vector *a, Vector *b, char operation, double scalar, Vector *result, double *scalar_result)
{
    switch(operation)
    {
        case OP_ADD:
            if (a == NULL || b == NULL || result == NULL)
            {
                return -1;
            }
        addvect(a, b, result);
            break;

        case OP_SUB:
             if (a == NULL || b == NULL || result == NULL)
            {
                return -1;
            }
            subvect(a, b, result);
            break;
    
        case OP_MULT:
             if (a == NULL || result == NULL)
            {
                return -1;
            }
            scalmult(a, scalar, result);
            break;
        
        case OP_DOT:
             if (a == NULL || b == NULL || scalar_result == NULL)
            {
                return -1;
            }
            *scalar_result = dotprod (a, b);
            break;
        
        case OP_CROSS:
             if (a == NULL || b == NULL || result == NULL)
            {
                return -1;
            }
            crossprod(a, b, result);
            break;
        
        default:
            return -1;
    }

    return 0;

    }

// Vector display functions

void disvect(Vector vector) 
{
    printf("%s = %f %f %f\n",
    vector.name,
    vector.x,
    vector.y,
    vector.z);
}

void list(void) 
{
    for (int i = 0; i < MAX_VECTORS; i++)
    {
        if (vectors[i].name[0] != '\0')
        {
            disvect(vectors[i]);
        }
    }
}

// UI Layer

int tokenize(char *input, char *tokens[])
{
    int count = 0;
    char *token;

    token = strtok(input, " ,\n");

    while(token != NULL)
    {
        if (count >= 5)
        {
            return 6;
        }
        tokens[count] = token;
        count++;

        token = strtok(NULL, " ,\n");
    }

    return count;
}

int is_number(const char *token)
{
    char *endptr;

    double value;

    if (token == NULL || token[0] == '\0')
    {
        return 0;
    }

    errno = 0;
    value = strtod(token, &endptr);

    return endptr != token && *endptr == '\0' && errno != ERANGE && isfinite(value);
}

Command get_command(const char *input)
{
    char command[20];

    if (sscanf(input, "%19s", command) != 1)
    {
        return CMD_OPERATION;
    }

    switch (command[0])
    {
        case 'q':
            if (strcmp(command, "quit") == 0)
            {
                return CMD_QUIT;
            }
            break;

        case 'h':
            if (strcmp(command, "help") == 0)
            {
                return CMD_HELP;
            }
            break;
        
        case 'l':
            if (strcmp(command, "list") == 0)
            {
                return CMD_LIST;
            }
            break;
        
        case 'c':
            if (strcmp(command, "clear") == 0)
            {
                return CMD_CLEAR;
            }
            break;

        case '-':
            if (strcmp(command, "-h") == 0)
            {
                return CMD_HELP;
            }
            break;

        default:
            break;
    }

    return CMD_OPERATION;
}

void getvect(char *input)
{
    char *tokens[5];
    int count = tokenize (input, tokens);

    switch(count)
    {
        //case # = tokens handled
        case 1:
            display_vector_command(tokens);
            break;

        case 3:
            handle_operation(tokens);
            break;

        case 4:
        case 5:
            handle_assignment(tokens, count);
            break;

        default:
            error();
            break;
    }
}

static void display_vector_command(char *tokens[])
{
    int index = findvect(tokens[0]);

    if (index == -1)
    {
        printf("Error: Vector not found.\n");
        return;
    }

    disvect(vectors[index]);
}
 

static void handle_operation(char *tokens[])
{
    Vector *a = NULL;
    Vector *b = NULL;

    double scalar = 0.0;
    double scalar_result = 0.0;

    int index_a;
    int index_b;
    int result;

    switch(tokens[1][0])
    {
        case '+':
        {
            if (is_number(tokens[0]) || is_number(tokens[2]))
            {
                error();
                return;
            }

            index_a = findvect(tokens[0]);
            index_b = findvect(tokens[2]);

            if (index_a == -1)
            {
                printf("Error: Vector %s not found.\n", tokens[0]);
                return;
            }

            if (index_b == -1)
            {
                printf("Error: Vector %s not found.\n", tokens[2]);
                return;
            }

            a = &vectors[index_a];
            b = &vectors[index_b];

            result = resopvect(a, b, OP_ADD, 0.0, &new_vect[new_vect_index], NULL);

            if (result != 0)
            {
                error();
                return;
            }

            break;
        }

        case '-':
        {
            if (is_number(tokens[0]) || is_number(tokens[2]))
            {
                error();
                return;
            }

            index_a = findvect(tokens[0]);
            index_b = findvect(tokens[2]);

            if (index_a == -1)
            {
                printf("Error: Vector %s not found.\n", tokens[0]);
                return;
            }

            if (index_b == -1)
            {
                printf("Error: Vector %s not found.\n", tokens[2]);
                return;
            }

            a = &vectors[index_a];
            b = &vectors[index_b];

            result = resopvect(a, b, OP_SUB, 0.0, &new_vect[new_vect_index], NULL);

            if (result != 0)
            {
                error();
                return;
            }

            break;
        }
        
        case '.':
        {
            if (is_number(tokens[0]) || is_number(tokens[2]))
            {
                error();
                return;
            }

            index_a = findvect(tokens[0]);
            index_b = findvect(tokens[2]);

            if (index_a == -1)
            {
                printf("Error: Vector %s not found.\n", tokens[0]);
                return;
            }

            if (index_b == -1)
            {
                printf("Error: Vector %s not found.\n", tokens[2]);
                return;
            }

            a = &vectors[index_a];
            b = &vectors[index_b];

            result = resopvect(a, b, OP_DOT, 0.0, NULL, &scalar_result);

            if (result != 0)
            {
                error();
                return;
            }

            printf("%f\n", scalar_result);
            return;
        }

        case 'x':
        {
            if (is_number(tokens[0]) || is_number(tokens[2]))
            {
                error();
                return;
            }

            index_a = findvect(tokens[0]);
            index_b = findvect(tokens[2]);

            if (index_a == -1)
            {
                printf("Error: Vector %s not found.\n", tokens[0]);
                return;
            }

            if (index_b == -1)
            {
                printf("Error: Vector %s not found.\n", tokens[2]);
                return;
            }

            a = &vectors[index_a];
            b = &vectors[index_b];

            result = resopvect(a, b, OP_CROSS, 0.0, &new_vect[new_vect_index], NULL);

            if (result != 0)
            {
                error();
                return;
            }

            break;
        }

        case '*':
        {
            // a * 2
            if (!is_number(tokens[0]) && is_number(tokens[2]))
            {
                index_a = findvect(tokens[0]);

                if (index_a == -1)
                {
                    printf("Error: Vector %s not found.\n", tokens[0]);
                    return;
                }

                scalar = strtod(tokens[2], NULL);

                result = resopvect(&vectors[index_a], NULL, OP_MULT, scalar, &new_vect[new_vect_index], NULL);

            }
            // 2 * a
            else if (is_number(tokens[0]) && !is_number(tokens[2]))
            {
                index_a = findvect(tokens[2]);

                if (index_a == -1)
                {
                    printf("Error: Vector %s not found.\n", tokens[2]);
                    return;
                }

                scalar = strtod(tokens[0], NULL);

                result = resopvect (&vectors[index_a], NULL, OP_MULT, scalar, &new_vect[new_vect_index], NULL);

            }
            // 2 * 3 or a * b
            else
            {
                error();
                return;
            }

            if (result != 0)
            {
                error();
                return;
            }

            break;
        }

        default:
            error();
            return;
    }

    strcpy(new_vect[new_vect_index].name, "ans");

    disvect(new_vect[new_vect_index]);

    new_vect_index++;

    if (new_vect_index >= MAX_VECTORS)
    {
        new_vect_index = 0;
    }
}

static void handle_assignment(char *tokens[], int count)
{
    Vector assignment;

    if (strcmp(tokens[1], "=") != 0)
    {
        error();
        return;
    }

    if (!valid_vector_name(tokens[0]))
    {
        printf("Error: Vector name must be 1 to %zu characters long.\n", sizeof assignment.name -1);
        return;
    }

    /*
    * Four tokens:
    * a = 1 2
    */
    if (count == 4)
    {
        if (!is_number(tokens[2]) || !is_number(tokens[3]))
        {
            error();
            return;
        }

        strcpy(assignment.name, tokens[0]);

        assignment.x = strtod(tokens[2], NULL);
        assignment.y = strtod(tokens[3], NULL);
        assignment.z = 0.0;

        if (add_vector(assignment) == -1)
        {
            printf("Error: Vector storage full.\n");
            return;
        }

        disvect(assignment);
        return;
    }

    /*
    * Five tokens:
    * a = 1 2 3
    * c = a + b
    * c = a - b
    * c = a * 2
    c = 2 * a
    c = a . b
    c = a x b
    */
    if (count == 5)
    {
        // Vector creation
        if (is_number(tokens[2]) &&
            is_number(tokens[3]) &&
            is_number(tokens[4]))
            {
                strcpy(assignment.name, tokens[0]);

                assignment.x = strtod(tokens[2], NULL);
                assignment.y = strtod(tokens[3], NULL);
                assignment.z = strtod(tokens[4], NULL);

                if (add_vector(assignment) == -1)
                {
                    printf("Error: Vector storage full.\n");
                    return;
                }

                disvect(assignment);
                return;
            }

            // All other cases are assigned operations or errors
            handle_assignment_operation(tokens);
            return;
    }

    error();
}

static void handle_assignment_operation(char *tokens[])
{
    Vector *a;
    Vector *b;

    double scalar = 0.0;
    double scalar_result = 0.0;

    int index_a;
    int index_b;
    int result;

    switch(tokens[3][0])
    {
        case '+':
        {
            if (is_number(tokens[2]) || is_number(tokens[4]))
            {
                error();
                return;
            }

            index_a = findvect(tokens[2]);
            index_b = findvect(tokens[4]);

            if (index_a == -1)
            {
                printf("Error: Vector %s not found.\n", tokens[2]);
                return;
            }

            if (index_b == -1)
            {
                printf("Error: Vector %s not found.\n", tokens[4]);
                return;
            }

            a = &vectors[index_a];
            b = &vectors[index_b];

            result = resopvect(a, b, OP_ADD, 0.0, &new_vect[new_vect_index], NULL);

            if (result != 0)
            {
                error();
                return;
            }

            break;
        }

        case '-':
        {
             if (is_number(tokens[2]) || is_number(tokens[4]))
        {
            error();
            return;
        }

        index_a = findvect(tokens[2]);
        index_b = findvect(tokens[4]);

        if (index_a == -1)
        {
            printf("Error: Vector %s not found.\n", tokens[2]);
            return;
        }

        if (index_b == -1)
        {
            printf("Error: Vector %s not found.\n", tokens[4]);
            return;
        }

        a = &vectors[index_a];
        b = &vectors[index_b];

        result = resopvect(a, b, OP_SUB, 0.0, &new_vect[new_vect_index], NULL);

        if (result != 0)
        {
            error();
            return;
        }

        break;

        }

        case 'x':
        {
             if (is_number(tokens[2]) || is_number(tokens[4]))
        {
            error();
            return;
        }

        index_a = findvect(tokens[2]);
        index_b = findvect(tokens[4]);

        if (index_a == -1)
        {
            printf("Error: Vector %s not found.\n", tokens[2]);
            return;
        }

        if (index_b == -1)
        {
            printf("Error: Vector %s not found.\n", tokens[4]);
            return;
        }

        a = &vectors[index_a];
        b = &vectors[index_b];

        result = resopvect(a, b, OP_CROSS, 0.0, &new_vect[new_vect_index], NULL);

        if (result != 0)
        {
            error();
            return;
        }

        break;
        
        }

        case '.':
        {
             if (is_number(tokens[2]) || is_number(tokens[4]))
        {
            error();
            return;
        }

        index_a = findvect(tokens[2]);
        index_b = findvect(tokens[4]);

        if (index_a == -1)
        {
            printf("Error: Vector %s not found.\n", tokens[2]);
            return;
        }

        if (index_b == -1)
        {
            printf("Error: Vector %s not found.\n", tokens[4]);
            return;
        }

        a = &vectors[index_a];
        b = &vectors[index_b];

        result = resopvect (a, b, OP_DOT, 0.0, NULL, &scalar_result);

        if (result != 0)
        {
            error();
            return;
        }

        new_vect[new_vect_index].x = scalar_result;
        new_vect[new_vect_index].y = 0.0;
        new_vect[new_vect_index].z = 0.0;

        break;

        }

        case '*':
        {
            // c = a * 2
            if (!is_number(tokens[2]) && is_number(tokens[4]))
            {
                index_a = findvect(tokens[2]);

                if (index_a == -1)
                {
                    printf("Error: Vector %s not found.\n", tokens[2]);
                    return;
                }

                scalar = strtod(tokens[4], NULL);
                
                result = resopvect(&vectors[index_a], NULL, OP_MULT, scalar, &new_vect[new_vect_index], NULL);
            }

            // c = 2 * a
            else if (is_number(tokens[2]) && !is_number(tokens[4]))
            {
                index_a = findvect(tokens[4]);

                if (index_a == -1)
                {
                    printf("Error: Vector %s not found.\n", tokens[4]);
                    return;
                }

                scalar = strtod(tokens[2], NULL);
                
                result = resopvect(&vectors[index_a], NULL, OP_MULT, scalar, &new_vect[new_vect_index], NULL);
            }

            // c = 2 * 3 or c = a * b
            else
            {
                error();
                return;
            }

            if (result != 0)
            {
                error();
                return;
            }

        break;
        }

    default:
        error();
        return;
    }
    // All vector-producing assigned operations use the requested destination name
    strcpy(new_vect[new_vect_index].name, tokens[0]);

    if (add_vector(new_vect[new_vect_index]) == -1)
    {
        printf("Error: Vector storage full.\n");
        return;
    }

    disvect(new_vect[new_vect_index]);

    new_vect_index++;

    if (new_vect_index >= MAX_VECTORS)
    {
        new_vect_index = 0;
    }
}

void quit(void)
{
    printf("Exiting Vector Calculator.\n");
}

void help(void)
{
    printf("\nVector Calculator Commands:\n");
    printf("> a = 1 2 3     Create a vector\n");
    printf("> a             Display a vector\n");
    printf("> a + b         Add two vectors\n");
    printf("> c = a + b     Add two vectors and assign result\n");
    printf("> c = a - b     Subtract two vectors and assign result\n");
    printf("> c = a * 2     Scalar multiplication and assign result\n");
    printf("> c = a . b     Dot product of two vectors and assign result\n");
    printf("> c = a x b     Cross product of two vectors and assign result\n");
    printf("> list          Displays all stored vectors\n");
    printf("> clear         Clear all vectors\n");
    printf("> -h            Display help menu\n");
    printf("> help          Display help menu\n");
    printf("> quit          Exit program\n\n");
}

void error(void)
{
    printf("Error: Invalid command.\n");
}




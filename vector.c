/*************************************************************************
* Filename: vector.c
* Author: Zach Campbell
* Description: Function definitions for updated vector calculator
* Date: 10/14/26
* Version: 2.1
* Notes:
*   -
*   - gcc -Wall -Wextra -Wpedantic -Werror -o main vectop.c vector.c -lm
* Run:
*   - ./main
*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include "vector.h"
#include "vectlib.h"

// Permanant vector storage
static Vector *vectors = NULL;
static size_t vector_count = 0;
static size_t vector_capacity = 0;

// Temporary vector storage for operation results
static Vector new_vect[MAX_VECTORS];
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
    for (size_t i = 0; i < vector_count; i++)
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
   Vector *new_storage;

    if (memchr(new_vector.name, '\0', sizeof new_vector.name) == NULL || new_vector.name[0] == '\0')
    {
        return -1;
    }

   if(!isfinite(new_vector.x) ||
      !isfinite(new_vector.y) ||
      !isfinite(new_vector.z))
      {
        return -1;
      }

    index = findvect(new_vector.name);

    if (index >= 0)
    {
        vectors[index] = new_vector;
        return 0;
    }

   if (vector_count == vector_capacity)
   {
    size_t new_capacity = (vector_capacity == 0) ? 10 : vector_capacity * 2;
    if (new_capacity < vector_capacity || new_capacity > (size_t)-1 / sizeof *vectors)
    {
        return -1;
    }

    new_storage = realloc(vectors, new_capacity * sizeof *vectors);
    if (new_storage == NULL)
    {
        return -1;
    }

    vectors = new_storage;
    vector_capacity = new_capacity;

   }

   vectors[vector_count++] = new_vector;
   return 0;
}

int get_vector(const char *name, Vector *result)
{
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
    free(vectors);
    vectors = NULL;
    vector_count = 0;
    vector_capacity = 0;
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
    for (size_t i = 0; i < vector_count; i++)
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
        count ++;

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


    if (sscanf (input, "%19s", command) != 1)
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
            if (strcmp(command, "load") == 0)
            {
                return CMD_LOAD;
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

        case 's':
            if (strcmp(command, "save") == 0)
            {
                return CMD_SAVE;
            }
            break;

        case 'f':
            if (strcmp(command, "fill") == 0)
            {
                return CMD_FILL;
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
    int count = tokenize(input, tokens);

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

        memset(&new_vect[new_vect_index], 0, sizeof new_vect[new_vect_index]);
        strcpy(new_vect[new_vect_index].name, tokens[0]);
        new_vect[new_vect_index].x = scalar_result;

        if (add_vector(new_vect[new_vect_index]) == -1)
        {
            printf("Error: Unable to store result vector.\n");
            return;
        }

        disvect(new_vect[new_vect_index]);
        new_vect_index = (new_vect_index + 1) % MAX_VECTORS;
        return;

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

// Load one CSV vector per line: name,x,y,z. Existing names are updated.
int load_vectors(const char *filename)
{
    FILE *file;
    char line[512];
    int loaded = 0;

    if (filename == NULL || filename[0] == '\0')
    {
        return -1;
    }

    file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return -1;
    }

    while (fgets(line, sizeof line, file) != NULL)
    {
        Vector v = {0};
        char *name, *sx, *sy, *sz, *extra;
        char *endptr;
        double x, y, z;
        size_t len = strlen(line);

        if (len > 0 && line[len - 1] != '\n' && feof(file))
        {
            int ch;
            while ((ch = fgetc(file)) != '\n' && ch != EOF) { }
            fprintf(stderr, "Error: CSV line is too long; skipped.\n");
            continue;
        }

        name = strtok(line, ",\r\n");
        sx = strtok(NULL, ",\r\n");
        sy = strtok(NULL, ",\r\n");
        sz = strtok(NULL, ",\r\n");
        extra = strtok(NULL, ",\r\n");
        
        if (name == NULL || sx == NULL || sy == NULL || sz == NULL || extra != NULL || strlen(name) == 0 || strlen(name) >= sizeof v.name)
        {
            fprintf(stderr, "Error: Invalid CSV row; skipped.\n");
            continue;
        }

        errno = 0; 
        x = strtod(sx, &endptr);
        if (errno == ERANGE || endptr == sx || *endptr != '\0' || !isfinite(x)) 
        {
            goto bad_row;
        }

        errno = 0; 
        y = strtod(sy, &endptr);
        if (errno == ERANGE || endptr == sy || *endptr != '\0' || !isfinite(y))
        { 
            goto bad_row;
        }

        errno = 0; 
        z = strtod(sz, &endptr);
        if (errno == ERANGE || endptr == sz || *endptr != '\0' || !isfinite(z))
        {
            goto bad_row;
        }

        strcpy(v.name, name);
        v.x = x; 
        v.y = y; 
        v.z = z;
        if (add_vector(v) != 0)
        {
            fprintf(stderr, "Error: Could not store vector from CSV.\n");
            fclose(file);
            return -1;
        }
        loaded++;
        continue;

    bad_row:
        fprintf(stderr, "Error: Invalid numeric value in CSV row; skipped.\n");
    }

    if (ferror(file))
    {
        perror("Error reading file");
        fclose(file);
        return -1;
    }
    fclose(file);
    printf("Loaded %d vector(s).\n", loaded);
    return 0;
}

// save name.csv to save vectors to .csv file for later reference
int save_vectors(const char *filename)
{
    FILE *file;
    size_t i;

    if (filename == NULL || filename[0] == '\0')
    {
        return -1;
    }

    file = fopen(filename, "w");
    if (file == NULL)
    {
        perror("Error opening file");
        return -1;
    }

    for (i = 0; i < vector_count; i++)
    {
        if (fprintf(file, "%s,%.17g,%.17g,%.17g\n", vectors[i].name,
                    vectors[i].x, vectors[i].y, vectors[i].z) < 0)
        {
                        perror("Error writing file");
                        fclose(file);
                        return -1;
        }
    }

    if (fclose(file) != 0)
    {
        perror("Error closing file");
        return -1;
    }

    printf("Saved %zu vector(s).\n", vector_count);
    return 0;
}
// > fill # where # is the number of vectors you wish to be filled in
int fill_vectors(size_t count)
{
    static size_t next_index = 0;
    size_t i;
    for (i = 0; i < count; i++)
    {
        Vector v = {0};
        int written = snprintf(v.name, sizeof v.name, "F%08lu", (unsigned long)next_index);

        if (written < 0 || (size_t)written >= sizeof v.name)
        {
            return -1;
        }

        v.x = (double)next_index;
        v.y = (double)next_index + 1.0;
        v.z = (double)next_index + 2.0;
        if (add_vector(v) != 0)
        {
            fprintf(stderr, "Error: Unable to add generated vector.\n");
            return -1;
        }

        next_index++;
    }

    printf("Added %zu vector(s).\n", count);
    return 0;
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
    printf("> load <file>   Load vectors from CSV file\n");
    printf("> save <file>   Save vectors to CSV file\n");
    printf("> fill <count>  Generate test vectors\n");
    printf("> -h            Display help menu\n");
    printf("> help          Display help menu\n");
    printf("> quit          Exit program\n\n");
}

void error(void)
{
    printf("Error: Invalid command.\n");
}

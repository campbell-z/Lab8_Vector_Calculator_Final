/*********************************************************
* Filenmae: vectop.c
* Author: Zach Campbell
* Description: Vector Calculator UI program
* Date: 10/1/26
* Version: 2.2 
* Notes:
*   - 
* Compile:
*   - gcc -Wall -Wextra -Wpedantic -Werror -o main vectop.c vector.c -lm
* Run:
*   - ./main
*********************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "vector.h"

#define INPUT_SIZE 100

// Return nonzero when the line contains only white spaces
static int is_blank_line(const char *line)
{
    while (*line != '\0')
    {
        if (!isspace((unsigned char) *line))
        {
            return 0;
        }
        line++;
    }

    return 1;
}



int main (void)
{
    char input[100];
    Command command;

    printf("Vector Calculator\n");
    printf("Enter -h or help for help.\n");

    while(1)
    {
        int ch;
        size_t length;

        printf("> ");
        fflush (stdout);

        if (fgets(input, sizeof (input), stdin) == NULL)
        {
            break;
        }

        length = 0;
        while(input[length] != '\0')
        {
            length++;
        }
        // Reject an overly long line rather than parsing it in fragments
        if (length > 0 && input[length-1] != '\n' && !feof(stdin))
        {
            while((ch = getchar()) != '\n' && ch != EOF)
            {
                // Discard the remainder of the oversized input line
                break;
            }
            printf("Error: Command is too long.\n");
            continue;
        }

        if (is_blank_line (input))
        {
            continue;
        }
        
        command = get_command (input);

     switch(command)
     {
        case CMD_QUIT:
            quit();
            return 0;
        
        case CMD_HELP:
            help();
            break;

        case CMD_LIST:
            list();
            break;

        case CMD_CLEAR:
            clear();
            break;

        case CMD_OPERATION:
            getvect (input);
            break;

        default:
            error();
            break;
        }
    }
    return 0;
}
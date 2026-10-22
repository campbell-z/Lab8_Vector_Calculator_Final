/**************************************************************************
* Filenmae: vectop.c
* Author: Zach Campbell
* Description: Upgraded Vector Calculator main program
* Date: 10/14/26
* Version: 2.1
* Notes:
*   - 
* Compile:
*   - gcc -Wall -Wextra -Wpedantic -Werror -o main vectop.c vector.c -lm
* Run:
*   - ./main
**************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>

#include "vector.h"
#include "vectlib.h"

#define INPUT_SIZE 512

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
            clear();
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
        
        case CMD_LOAD:
        case CMD_SAVE:
        {
            char command_word[20];
            char filename[256];
            char extra[2];

            if (sscanf(input, "%19s %255s %1s", command_word, filename, extra) != 2)
            {
                printf("Usage: %s <filename>\n", command_word);
                break;
            }

            if (command == CMD_LOAD)
            {
                (void)load_vectors(filename);
            }
            else 
            {
                (void)save_vectors(filename);
            }
            break;
        }

        case CMD_FILL:
            {
                char command_word[20];
            char count_text[64];
            char extra[2];
            char *endptr;
            unsigned long long count;

            if (sscanf(input, "%19s %63s %1s", command_word, count_text, extra) != 2)
            {
                printf("Usage: fill <count>\n");
                break;
            }

            errno = 0;
            count = strtoull(count_text, &endptr, 10);

            // Error checking for fill
            if (count_text[0] == '-' || 
                errno == ERANGE || 
                endptr == count_text || 
                *endptr != '\0' || 
                count > (unsigned long long)((size_t)-1))
            {
                printf("Error: Invalid fill count.\n");
                break;
            }
            (void)fill_vectors((size_t)count);
            break;
            }

        case CMD_OPERATION:
            getvect (input);
            break;

        default:
            error();
            break;
        }
    }
    clear();
    return 0;
}

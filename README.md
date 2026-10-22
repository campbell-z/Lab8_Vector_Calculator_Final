# Lab8_Vector_Calculator_Final
## About The Project
    This program is a vector calculator that takes user input of 3 specified numbers assigned to a vector
    to perform vector operations. User input is stored in a permanent vector storage array, and whenever 
    the user enters a specific operrand command, the previously inputted numbers are sent to a temporary 
    working array where they are used in the function corresponding to the operation command. If the 
    operation is unassigned, the result is not assigned to the permanent vector storage array and is 
    cycled out as the temporary working array is reused. The user may save or load vectors using the 
    corresponding command with .csv files. The program then the either writes the vectors into the 
    file or reads the vectors and assigns them to the permanent vector storage array. Version 2.0
    uses a dynamic array to support these operations and a large number of vectors.
## How To Build The Program
    Utilize the included Makefiles for the respective versions or on the command line enter:
    > $make
    
    or

    > $gcc -Wall -Wextra -Wpedantic -Werror -o main vectop.c vector.c -lm
## How To Run The Program
    On the command line enter:
    > $./main
## Supported Commands
### Version 1.0
    *> a = 1 2 3     Create a vector
    *> a             Display a vector 
    *> a + b         Add two vectors 
    *> c = a + b     Add two vectors and assign result 
    *> c = a - b     Subtract two vectors and assign result 
    *> c = a * 2     Scalar multiplication and assign result 
    *> c = a . b     Dot product of two vectors and assign result 
    *> c = a x b     Cross product of two vectors and assign result 
    *> list          Displays all stored vectors 
    *> clear         Clear all vectors 
    *> -h            Display help menu
    *> help          Display help menu
    *> quit          Exit program
### Version 2.0
    *> a = 1 2 3     Create a vector 
    *> a             Display a vector 
    *> a + b         Add two vectors 
    *> c = a + b     Add two vectors and assign result 
    *> c = a - b     Subtract two vectors and assign result 
    *> c = a * 2     Scalar multiplication and assign result 
    *> c = a . b     Dot product of two vectors and assign result 
    *> c = a x b     Cross product of two vectors and assign result 
    *> list          Displays all stored vectors 
    *> clear         Clear all vectors 
    *> load <file>   Load vectors from CSV file 
    *> save <file>   Save vectors to CSV file 
    *> fill <count>  Generate test vectors 
    *> -h            Display help menu 
    *> help          Display help menu 
    *> quit          Exit program\n 
## Dynamic Memory Use
    The program uses dynamic memory to store vectors in a dynamically sized array. When the array reaches 
    capacity, realloc() increases it's capacity by doubling the available storage. add_vector() uses a 
    temporary pointer to safely handle reallocation and updates the pointer only upon successful 
    reallocation. clear() frees the allocated memory and resets the associated variables, allowing the 
    program to reuse storage safely. clear() is called when the user quits to ensure memory is released 
    before the program exists.

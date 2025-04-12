//Created by Romsev Charles

/*
    TODO: 
        work on the stdin input path being read line by line

        makes sure the program array actually has the chars

        error check the fgetc and other commands

        add brackets

        check if program is valid


*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//--------Fun Terminal Coloring---------------
#define RED     "\x1B[31m"
#define GREEN   "\x1B[32m"
#define MAGENTA "\x1B[35m"
#define CYAN    "\x1B[36m"
#define RESET   "\x1B[0m"
//-------------------------------------------


#define TRUE    1
#define FALSE   0



const char valid_tokens[] = {'>', '<', '+', '-', '[', ']', ',', '.'};
size_t tokenlist_size = sizeof(valid_tokens)/sizeof(char);


typedef struct Program
{
    unsigned char *program;
    size_t program_size;
}Program;


//Function Declarations
Program readProgram(FILE *fp); //reads valid tokens from file and return a program struct
short isValidToken(char token); //returns TRUE if token is valid, FALSE otherwise
void printTape(Program tape);
void printProgram(char Program[], size_t program_size);
//--------------------


int main(int argc, char const *argv[])
{
    
    FILE *inputfile;

    //read in file or command line
    if ( argc > 1){

        inputfile = fopen(argv[1], "r");
        if(inputfile == NULL){

            fprintf(stderr, RED "Problem opening the input file %s" RESET, argv[1]);

            return EXIT_SUCCESS;

        }

    }
    else{
        
        printf("Input EOF when done writing program in command prompt.\n");
        printf(CYAN "\tPress Ctrl + D on Linux to exit program.\n\n" RESET);
        inputfile = stdin;

    }
    
    //read in program and initialize tape

    Program program;
    program = readProgram(inputfile);


    Program tape;
    tape.program_size = 30000; //default tape size, based on brainfuck "standard"
    tape.program = calloc(sizeof(char), tape.program_size);


    printf("Validated Program:\n");
    printTape(program);


    printf("\n\n" MAGENTA "Start of program output:" RESET "\n\n");

    //brainfuck logic
    /*
    > = increases memory pointer, or moves the pointer to the right 1 block.
    < = decreases memory pointer, or moves the pointer to the left 1 block.
    + = increases value stored at the block pointed to by the memory pointer
    - = decreases value stored at the block pointed to by the memory pointer
    [ = like c while(cur_block_value != 0) loop.
    ] = if block currently pointed to's value is not zero, jump back to [
    , = like c getchar(). input 1 character.
    . = like c putchar(). print 1 character to the console
*/
    
    size_t instruction_pointer = 0; //where we are in the tape
    size_t program_pointer = 0; //where we are while reading the program
    for (program_pointer = 0; (program_pointer < program.program_size); program_pointer++)
    {   
        
        switch (program.program[program_pointer])
        {
        case '>':
            // printf("incrementing pointer\n");
            
            if (instruction_pointer >= tape.program_size){
                fprintf(stderr, RED "Instruction pointer has gone above and out of tape bounds it is currently at " RESET "%ld\n", instruction_pointer);
                return EXIT_FAILURE;
            }
            instruction_pointer += 1;
            break;
        
        case '<':
            printf("decrementing pointer\n");
            
            if (instruction_pointer <= 0)
            {
                fprintf(stderr, RED "Instruction pointer has gone below and out of tape bounds it is currently at " RESET "%ld\n" , instruction_pointer);
                return EXIT_FAILURE;
            }    
            instruction_pointer -= 1;
            break;
        
        case '+':
            printf("incrementing value at %ld\n", instruction_pointer);
    
            if(tape.program[instruction_pointer] >= 255){
                tape.program[instruction_pointer] = 0;
            }
            else{
                tape.program[instruction_pointer]++;
            }
            
            break;
        
        case '-':
            printf("decrementing value at %ld\n", instruction_pointer);

            if (tape.program[instruction_pointer] <= 0)
            {
                tape.program[instruction_pointer] = 255;
            }
            else
            {
                tape.program[instruction_pointer]--;
            }

            break;
        
        case '.':
            printf("outputting char\n");
            putchar( (char)tape.program[instruction_pointer] );
            break;

        case ',':
            printf("inputting char\n");
            tape.program[instruction_pointer] = fgetc(stdin);
            break;

        default:
            continue;
        }
        

    }
    
    printf("\nTape:\n");
    printTape(tape);
   
   //clean up
   
   free(tape.program);
   tape.program = NULL;

   free(program.program);
    program.program = NULL;
    
    fclose(inputfile);
    
    return 0;
}


Program readProgram(FILE *fp){

    
    size_t current_allocated_size = 10000;


    Program program;
    program.program_size = 0;
    program.program = NULL;

    program.program = malloc(sizeof(char) * current_allocated_size);
    if (program.program == NULL)
    {
        fprintf(stderr, RED "Error allocating enough memory for program" RESET);
        exit(EXIT_FAILURE);
    }

    while (!feof(fp)){

        char c = fgetc(fp);

        if(isValidToken(c)){

            //check if program still has enough allocated space
            
            if (program.program_size >= current_allocated_size){

                current_allocated_size *= 2;
                program.program = realloc(program.program, sizeof(char) * current_allocated_size);
                if (program.program == NULL)
                {
                    free(program.program);
                    fprintf(stderr, RED "Error reallocating enough memory for program" RESET);
                    exit(EXIT_FAILURE);
                }
                
            }
            
            program.program[program.program_size] = c;
            program.program_size++;

        }

    }

    

    return program;
}



short isValidToken(char token){

    for (size_t i = 0; i < tokenlist_size; i++)
    {
        if(token == valid_tokens[i]){
            printf("%c is valid token\n", token);
            return TRUE;
        }
    }
    

    return FALSE;   

}

//prints until there are 5 empty blocks found in a row
void printTape(Program tape){

    printf("tape size: %ld\n", tape.program_size);

    int num_empty = 0;
    const int max_empty = 5;

    for (size_t i = 0; i < tape.program_size; i++)
    {
        
        if(num_empty >= max_empty){
            break;
        }
        
        
        printf("[%d]", tape.program[i]);
        
 
        if(tape.program[i] == 0){
            num_empty++;
        }
        else{
            num_empty = 0;
        }
        

    }

    printf("\n");

}


void printProgram(char program[], size_t program_size){


    printf("program size: %ld\n", program_size);
    for (size_t i = 0; i < program_size; i++)
    {
        
        printf("%c ", program[i]);

    }

    printf("\n");

}
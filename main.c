//Created by Romsev Charles

/*
    TODO: 
        work on the stdin input path being read line by line

        makes sure the program array actually has the chars

        implement the brainfuck logic

        error check the fgetc and other commands
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


//Function Declarations
size_t readProgram(FILE *fp, char *program, size_t *program_size); //returns the predicted size of tape needed for program
short isValidToken(char token); //returns TRUE if token is valid, FALSE otherwise
void printTape(__uint8_t tape[], size_t tape_size);
void printProgram(char program[], size_t program_size);
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
        printf(CYAN "\tCtrl + D for Linux at the moment.\n\n" RESET);
        inputfile = stdin;

    }
    
    //figure out how much space we need on the tape (we add more as needed, for example in a loop)
    size_t program_size;

    char *program = malloc(sizeof(int)); 
    size_t tape_size = readProgram(inputfile, program, &program_size);

    __uint8_t *tape = calloc(sizeof(__uint8_t), tape_size);

    printf("Tape:\n");
    printTape(tape, tape_size);


    if(program != NULL){
        printf("Program:\n");
        printProgram(program, program_size);
    }

    
   

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
    
    size_t ip = 0; //instruction pointer
    size_t pp = 0; //program pointer
    for (pp = 0; (pp < program_size); pp++)
    {   
        
        switch (program[pp])
        {
        case '>':
            ip += 1;
            break;
        
        case '<':
            ip -= 1;
            if (ip < tape_size)
            {
                fprintf(stderr, "");
            }
            
            break;
        case '+':
            tape[ip] += 1;
            if(tape[ip] > 255){
                tape[ip] = 0;
            }
            break;
        
        case '-':
            tape[ip] -= 1;
            if (tape[ip] < 0)
            {
                tape[ip] = 255;
            }
            break;
        
        default:
            continue;
        }
        

    }
    


   
   //clean up
   
    tape = NULL;
    free(tape);

    program = NULL;
    free(program);
    
    fclose(inputfile);
    
    return 0;
}


size_t readProgram(FILE *fp, char *program, size_t *program_size){

    // program = NULL;

    *program_size = 1;
    int tape_size = 0;

    char c;
    while( !feof(fp) ){
  
        program = realloc(program, *program_size);
        
        c = fgetc(fp);    

        //ignore everything that isn't a valid token
        if(isValidToken(c)){
            // printf("token: %c\n", c);
        
            if (c == '>'){
                tape_size++;
            }
        
            program[(*program_size) - 1] = c;
            (*program_size) += 1;
        }
        
    }

    *program_size -= 1; //overcounted program size
    return tape_size;
}



short isValidToken(char token){

    for (size_t i = 0; i < tokenlist_size; i++)
    {
        if(token == valid_tokens[i]){
            return TRUE;
        }
    }
    

    return FALSE;   

}


void printTape(__uint8_t tape[], size_t tape_size){

    for (size_t i = 0; i < tape_size; i++)
    {
        
        printf("[%d]", tape[i]);

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
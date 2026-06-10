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
#include <stdint.h>


//--------Fun Terminal Coloring---------------
#define RED     "\x1B[31m"
#define GREEN   "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define MAGENTA "\x1B[35m"
#define CYAN    "\x1B[36m"
#define RESET   "\x1B[0m"
//-------------------------------------------


#define TRUE    1
#define FALSE   0



const char valid_tokens[] = {'>', '<', '+', '-', '[', ']', ',', '.'};
size_t tokenlist_size = sizeof(valid_tokens)/sizeof(char);
size_t total_bracket_count = 0; // counted in "readProgram"


int8_t trace_flag = FALSE; // should we print trace messages for user
int8_t memory_dump_flag = FALSE; // dumps memory (tape) after execution
int8_t memory_trace_flag = FALSE; // dumps memory after each instruction
int8_t show_program_flag = FALSE; // shows program with only validated tokens
int8_t useFile_flag = FALSE; // internal, keeps track if theres's a file being read


typedef struct Token
{
    char token;
    int matching_bracket_index; //instruction pointer of matching bracket
}Token;

typedef struct Program
{
    Token *program;
    size_t program_size;
}Program;

typedef struct Tape
{
    unsigned char *tape;
    size_t tape_size;
}Tape;
 

//Function Declarations
Program readProgram(FILE *fp); //reads valid tokens from file and return a program struct
short isValidToken(char token); //returns TRUE if token is valid, FALSE otherwise
void printTape(Tape tape);
void printProgram(Program program);
//--------------------


Program validateProgram(Program program){
    
    //quick implementation of a stack
    //to check if the brackets are valid
    typedef struct Bracket{
        char token;
        size_t matching_bracket_index; //instruction pointer of matching bracket
        size_t self_index; //index of the token in the program
    }Bracket;

    Bracket stack[total_bracket_count];
    int stack_pointer = -1;


    if(program.program_size == 0){
        printf(YELLOW "[WARNING]: " RESET "No tokens were recognized in source code, nothing will run.\n");
        exit(EXIT_FAILURE);
    }
       

    for (size_t i = 0; i < program.program_size; i++){

        unsigned char curr_token = program.program[i].token;

        if(curr_token == '['){
            //push to stack
            stack_pointer++;
            stack[stack_pointer].token = curr_token;
            stack[stack_pointer].self_index = i;
            stack[stack_pointer].matching_bracket_index = i; //we dont know what it is yet so just give it itself
        }

        else if(curr_token == ']'){
            if(stack_pointer < 0){
                fprintf(stderr, RED "[ERROR]: " RESET "Can't run program, there is an unmatched ']' \n");
                exit(EXIT_FAILURE);
            }
            
            //pop the stack
            Bracket matchedBracket= stack[stack_pointer];
            
            program.program[i].matching_bracket_index = matchedBracket.self_index; // tell ']' where its matched '[' index is
            program.program[matchedBracket.self_index].matching_bracket_index = i; //tell matched '[' what current ']' index is
            

            stack_pointer--;
        }

    }

    //should be empty if we matched everything
    if(stack_pointer >= 0){
        fprintf(stderr, RED "[ERROR]: " RESET "Can't run program, there is an unmatched '[' \n");
        exit(EXIT_FAILURE);
    }

    return program;
    
}


int main(int argc, char const *argv[])
{
    
    FILE *inputfile;
    char inputfile_name[100];
    inputfile_name[0] = 0;

    for (short i = 1; i < argc; i++)
    {
        if(strcmp(argv[i] , "-h") == 0 || strcmp(argv[i] , "--help") == 0){

            printf(CYAN "=============BRAINFUCK COMPILER=============\n" RESET);
            printf("USAGE: bf [OPTIONS] ... [FILE]\n");
            printf("Uses a 30,000 long virtual memory tape to run your brainfuck program.");
            printf("\n");
            printf(" -t,\t--trace \t\t\t Log out what your program is doing with each instruction\n");
            printf(" -md,\t--memory-dump \t\t\t Show the memory (aka tape) after program execution\n");
            printf(" -mt,\t--memory-trace \t\t\t Show the memory (aka tape) after each instruction is executed\n");
            printf(" -p,\t--show-program \t\t\t Print out the program after it got parsed\n");

            printf("\n\n");
            printf("Thanks for using my implementation of the brainfuck compiler.\n");
            printf(" -Sev <www.romsev.dev>\n");

            return EXIT_SUCCESS;
        }

        if(strcmp(argv[i] , "-t") == 0 || strcmp(argv[i] , "--trace") == 0){
            trace_flag = TRUE;
        }

        if(strcmp(argv[i] , "-md") == 0 || strcmp(argv[i] , "--memory-dump") == 0){
            memory_dump_flag = TRUE;
        }

        if(strcmp(argv[i] , "-mt") == 0 || strcmp(argv[i] , "--memory-trace") == 0){
            memory_trace_flag = TRUE;
        }

        if(strcmp(argv[i] , "-p") == 0 || strcmp(argv[i] , "--show-program") == 0){
            show_program_flag = TRUE;
        }

        if(strstr(argv[i], ".bf") != NULL){ //regex would be better here but in spirit of keeping brainfuck compilers light, ill just do a simple check
            strcpy(inputfile_name, argv[i]);
            printf("File Name: %s \n", inputfile_name);
        }
    }
    

    //read in file for processing or look at the command line
    if (inputfile_name[0] != 0){ //if char array not empty

        inputfile = fopen(inputfile_name, "r");
        if(inputfile == NULL){

            fprintf(stderr, RED "[ERROR]: " RESET "Problem opening or finding the input file %s\n", inputfile_name);

            return EXIT_SUCCESS;

        }

    }
    else{
        
        printf("Input EOF when done writing program in command prompt.\n");
        printf(CYAN "\tPress Ctrl + D on Linux when done writing program.\n\n" RESET);
        inputfile = stdin;

    }
    
    //read in program and initialize tape
    Program program;
    program = readProgram(inputfile);

    program = validateProgram(program); //possible exit point if invalid program

    Tape tape;
    tape.tape_size = 30000; //default tape size, based on brainfuck "standard"
    tape.tape = calloc(sizeof(char), tape.tape_size);

    if(show_program_flag){
        printf("Validated Program:\n");
        printProgram(program);
    }
    
    //good seperator
    if(trace_flag || memory_dump_flag || memory_trace_flag || show_program_flag){
        printf("\n\n" MAGENTA "Start of program output:" RESET "\n\n");
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
    
    size_t tape_pointer = 0; //where we are in the tape
    size_t program_pointer = 0; //where we are while reading the program
    for (program_pointer = 0; (program_pointer < program.program_size); program_pointer++)
    {   
        
        switch (program.program[program_pointer].token)
        {
        case '>':
            if(trace_flag){
                printf("Incrementing pointer to %ld\n", tape_pointer + 1);
            }
            
            if (tape_pointer >= tape.tape_size){
                fprintf(stderr, RED "Tape pointer has gone above and out of bounds it is currently at " RESET "%ld\n", tape_pointer);
                return EXIT_FAILURE;
            }
            tape_pointer += 1;
            break;
        
        case '<':
            if(trace_flag){    
                printf("Decrementing pointer to %ld \n", tape_pointer - 1);
            }
            
            if (tape_pointer <= 0)
            {
                fprintf(stderr, RED "Tape pointer has gone below and out of bounds it is currently at " RESET "%ld\n" , tape_pointer);
                return EXIT_FAILURE;
            }    
            tape_pointer -= 1;
            break;
        
        case '+':
            if(trace_flag){
                printf("Incrementing value at %ld\n", tape_pointer);
            }
    
            if(tape.tape[tape_pointer] >= 255){
                tape.tape[tape_pointer] = 0;
            }
            else{
                tape.tape[tape_pointer]++;
            }
            
            break;
        
        case '-':
            if(trace_flag){
                printf("Decrementing value at %ld\n", tape_pointer);
            }

            if (tape.tape[tape_pointer] <= 0)
            {
                tape.tape[tape_pointer] = 255;
            }
            else
            {
                tape.tape[tape_pointer]--;
            }

            break;
        
        case '.':
            if(trace_flag){
                printf("Outputting at cell %ld\n", tape_pointer);
            }
            putchar( (char)tape.tape[tape_pointer] );
            break;

        case ',':
            if(trace_flag){
                printf("Waiting for input into cell %ld\n", tape_pointer);
            }
            tape.tape[tape_pointer] = fgetc(stdin);
            break;
        
        case '[':
            if(trace_flag){
                printf("Beginning loop\n");
            }
            //check if current pointer is 0
            if(tape.tape[tape_pointer] == 0){
                //Skip forward to matched bracket
                if(program.program[program_pointer].matching_bracket_index == -1){
                    printf(RED "[FATAL ERROR]: " RESET "A matching bracket for '[' is not referenced");
                    return EXIT_FAILURE;
                }

                program_pointer = program.program[program_pointer].matching_bracket_index;

                break;
            }

            break;

        case ']':
            
            if(tape.tape[tape_pointer] == 0){
                if(trace_flag){
                    printf("Continuing loop\n");
                }
                //continue forward in program      
                break;
            }
            
            //Go back to matched bracket
            if(program.program[program_pointer].matching_bracket_index == -1){
                printf(RED "[FATAL ERROR]: " RESET "A matching bracket for ']' is not referenced");
                return EXIT_FAILURE;
            }

            program_pointer = program.program[program_pointer].matching_bracket_index; //goes back to matched bracket
            if(trace_flag){
                printf("Looping pack to %ld", program_pointer);
            }
            break;
            

        default:
            continue;
        }
        
        if(memory_trace_flag){
            printTape(tape);
        }
    }
    
    if(memory_dump_flag || memory_trace_flag){
        printf("\nTape after execution:\n");
        printTape(tape);
    }
    
    //clean up
    free(tape.tape);
    tape.tape = NULL;

    free(program.program);
    program.program = NULL;
    
    fclose(inputfile);
    
    printf("\n");

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
            
            if(c == '[' || c == ']'){
                total_bracket_count++;
            }

            program.program[program.program_size].token = c;
            program.program[program.program_size].matching_bracket_index = -1; //we wont know it until we validate program
            program.program_size++;

        }

    }

    

    return program;
}



short isValidToken(char token){

    for (size_t i = 0; i < tokenlist_size; i++)
    {
        if(token == valid_tokens[i]){
            // printf("%c is valid token\n", token);
            return TRUE;
        }
    }
    

    return FALSE;   

}

//prints until there are 5 empty blocks found in a row
void printTape(Tape tape){

    printf("tape size: %ld\n", tape.tape_size);

    int num_empty = 0;
    const int max_empty = 5;

    for (size_t i = 0; i < tape.tape_size; i++)
    {
        
        if(num_empty >= max_empty){
            break;
        }
        
        
        printf("[%d]", tape.tape[i]);
        
 
        if(tape.tape[i] == 0){
            num_empty++;
        }
        else{
            num_empty = 0;
        }
        

    }

    printf("\n");

}


void printProgram(Program program){


    printf("Number of validated characters: %ld\n", program.program_size);
    for (size_t i = 0; i < program.program_size; i++)
    {
        
        printf("%c", program.program[i].token);
    }

    printf("\n");

}
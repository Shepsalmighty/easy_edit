#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SED_PATTERN "'s/\\b%s\\b/%s/g'"
#define COMMAND_PATTERN "sed -e %s < %s > %s"
// when called we will make a copy (filaname~) --- mv -i FILENAME FILENAME~
// replace search term with new term --- sed -e 's/\bSEARCH\b/REPLACE/g' <
// FILENAME~ > FILENAME
//
// example use ezedit search replace filename(s)
int main(int argc, char *argv[])
{
    char * program_name = argv[0];

    if( argc < 3 ){   
        fprintf(stderr, "not enough args -- example: < %s search replace filename >\n", program_name);
        exit(EXIT_FAILURE);
        }
    
    char * search = argv[1];
    char * replace = argv[2];
        
    size_t sed_len = snprintf(NULL, 0, SED_PATTERN, search, replace) + 1; //find the size in bytes of our search/replace args +1 as "\0" not incl
    char * sed = malloc(sed_len);
    if(!sed){
        perror(program_name);
        exit(EXIT_FAILURE);
    }
    snprintf(sed, sed_len, SED_PATTERN, search, replace); //write the sed command to the malloc'd memory

    for (int file = 3; file < argc; file++){

        char *file_name = argv[file];

        // find length of old file name and allocate memory
        size_t new_file_name_size = strlen(file_name) + 2; // +2 since strlen() doesn't count terminator \0 character and we are adding ~ so 2 characters
        char * new_file_name = malloc(new_file_name_size);
        if( !new_file_name ){
            perror(program_name);
            free(sed);
            exit(EXIT_FAILURE);
        }

        // create new file name
        size_t bytes_written = snprintf(new_file_name, new_file_name_size, "%s~", file_name);
        if( bytes_written +1 != new_file_name_size){
            perror(program_name);
            free(new_file_name);
            free(sed);
            exit(EXIT_FAILURE);
        }

        // rename original file (step 1)
        int errors = rename(file_name, new_file_name);
        if( errors ){
            perror(program_name);
            free(new_file_name);
            free(sed);
            exit(EXIT_FAILURE);
        }

        // search for term, replace and write to original file name (so we can use 'make' to verify it works - if not pre-edited code exists in FILENAME~) psuedocode sed -e 's/\bsearch\b/replace/g' < new_file_name > file_name;
        size_t command_len = snprintf(NULL, 0, COMMAND_PATTERN, sed, new_file_name, file_name) + 1;
        char * command = malloc(command_len);
        if( !command ){
            perror(program_name);
            free(new_file_name);
            free(sed);
            exit(EXIT_FAILURE);
        }
        snprintf(command, command_len, COMMAND_PATTERN, sed, new_file_name, file_name);
        printf("command = %s\n", command);
        
        int ret = system(command);

        if( WIFSIGNALED(ret) && (WTERMSIG(ret) == SIGINT || WTERMSIG(ret) == SIGQUIT )){
            perror(program_name);
            fprintf(stderr, "system(\"%s\") call failed status: %i\n", command, ret);
            free(new_file_name);
            free(sed);
            free(command);
            exit(EXIT_FAILURE);
        }

        printf("Doing stuff with \"%s\"\n ret (system call) status: %d\n", file_name, ret);
        free(new_file_name);
        free(command);
        }
    free(sed);
    exit(EXIT_SUCCESS);
}

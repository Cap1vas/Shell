#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#define MAXLETTERS 10000
#define MAXARGS 100

void whichCommand(char *command, char **args, int argc);
void cdCommand(char ** args, int argc);
void parseCommand(char *input, char **args, int *argc);

int readingInput(char* str) {
    char* buffer;
    buffer = readline("\n>>> ");
    
    if(strlen(buffer) != 0) {
        add_history(buffer);
        strcpy(str, buffer);
        
        char **args = malloc(MAXARGS * sizeof(char*));
        int argc = 0;
        
        
        parseCommand(buffer, args, &argc);
        
        
        if(argc > 0) {
            whichCommand(args[0], args, argc);
        }
        
        
        for(int i = 0; i < argc; i++) {
            free(args[i]);
        }
        free(args);
        free(buffer);
        return 0;
    } else {
        free(buffer);
        return 1;
    }
}

void parseCommand(char *input, char **args, int *argc) {
    char *token;
    char *input_copy = strdup(input);
    *argc = 0;
    
    token = strtok(input_copy, " \t\n");
    while(token != NULL && *argc < MAXARGS - 1) {
        args[*argc] = strdup(token);
        (*argc)++;
        token = strtok(NULL, " \t\n");
    }
    args[*argc] = NULL;
    free(input_copy);
}

void pwdCommand(char **args, int argc){
    char *cwd = getcwd(NULL,0);
    if(cwd != NULL){
        printf("%s\n", cwd);
        free(cwd);
    }else{
        perror("pwd:Error finding current working directory");
    }
}

void cdCommand(char ** args, int argc){
    if(argc < 2) {
        printf("cd: Few arguments\n");
        return;
    }
    if(chdir(args[1]) != 0) {
        perror("cd: Could not change to the desired directory");
    }
}

void lsCommand(char **args, int argc){
      DIR *dir;
    struct dirent *arg;
    char *dir_name= "."; 

    dir = opendir(dir_name);

    if (dir == NULL) {
        perror("Couldn't open the desired dir");
        return;
    }

    while ((arg = readdir(dir)) != NULL) {
        printf("%s\n",arg->d_name);
    }

    if (errno != 0) {
        perror("Error to show files");
    }

    closedir(dir);
    return;
}

void whichCommand(char *command, char **args, int argc){
    
    if(strcmp(command, "cd") == 0){
        cdCommand(args, argc);
    } else if(strcmp(command, "pwd") == 0){
        pwdCommand(args, argc);
    } else if(strcmp((command),"ls")==0){
        lsCommand(args,argc);
    }else {
        printf("Unknown command: %s\n", command);
    }
}

int main(int argc, char * argv[]){
    char inputString[MAXLETTERS];
    printf("Mini Shell iniciado. Digite comandos:\n");
    
    while(1){
        if(readingInput(inputString)){
            continue;
        }
    }
    return 0;
}
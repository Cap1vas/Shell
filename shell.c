#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <grp.h>

#define MAXLETTERS 10000
#define MAXARGS 100
#define MAX_FILES 300


int getGidCurrentUser(){
    gid_t gid = getgid();
    printf("GID %d", gid);
    return gid;  

}
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

void mkdirCommand(char **args, int argc){
    if(argc<2){
        printf("mkdir: few arguments. usage mkdir <dir-name>\n");
        return;
    }else if(strcmp(args[1],"--help)")==0){
        printf("mkdir: usage mkdir <dir-name>\n -p --> create a dir at a specific location");
    }else if(strcmp(args[1],"-p")==0){
        int status = mkdir(args[2],0777);
        if(status == 0){
            printf("Dir created");
        }else{
            perror("Could not create dir");
        }
    }else{
        int status = mkdir(args[1],0777);
        if(status == 0){
            printf("Dir created");
        }else{
            perror("Could not create dir");
        } 
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
    struct stat file_stat;
    errno = 0;
    DIR *dir;
    struct dirent *arg;    
    int is_la = (args[1] != NULL && strcmp(args[1], "-la") == 0);
    char *dir_name = is_la ? (args[2]==NULL? "." : args[2]) : (args[1]==NULL? "." : args[1]);
    char ** files = malloc(MAX_FILES * sizeof(char*));
    int count=0;
    dir = opendir(dir_name);
    if (dir == NULL) {
        perror("Couldn't open the desired dir");
        return;
    }

    while ((arg = readdir(dir)) != NULL) {
        files[count] = strdup(arg->d_name);
        count++;
    }

    for(int i = 0; i < count-1; i++) {
        for(int j = 0; j < count-i-1; j++) {
            if(strcmp(files[j], files[j+1]) > 0) {
                char *temp = files[j];
                files[j] = files[j+1];
                files[j+1] = temp;
            }
        }
    }
    if(is_la){
        char full_path[1024];
        for(int i=0; i<count; i++){
            if (strcmp(dir_name, ".") == 0) {
                strcpy(full_path, files[i]);  
            } else {
                snprintf(full_path, sizeof(full_path), "%s/%s", dir_name, files[i]);
            }
            if(lstat(full_path, &file_stat)==0){
            char permissions[11];
            //determines the file type (symlink, directory or regular file)
            if(S_ISDIR(file_stat.st_mode)){
                permissions[0] = 'd';
            }else if(S_ISLNK(file_stat.st_mode)) permissions[0] = 'l';
            else permissions[0] = '-';
            //File_stat.st_mode is a bit string indicating permission bits of a file
            //The & operator is a bit comparator, he compares if the file has a specific bit active
            //User permissions
            permissions[1] = (file_stat.st_mode & S_IRUSR) ? 'r' : '-'; 
            permissions[2] = (file_stat.st_mode & S_IWUSR) ? 'w' : '-';
            permissions[3] = (file_stat.st_mode & S_IXUSR) ? 'x' : '-';
            //Group permissions
            permissions[4] = (file_stat.st_mode & S_IRGRP) ? 'r' : '-';
            permissions[5] = (file_stat.st_mode & S_IWGRP) ? 'w' : '-';
            permissions[6] = (file_stat.st_mode & S_IXGRP) ? 'x' : '-';
            //Others permissions
            permissions[7] = (file_stat.st_mode & S_IROTH) ? 'r' : '-';
            permissions[8] = (file_stat.st_mode & S_IWOTH) ? 'w' : '-';
            permissions[9] = (file_stat.st_mode & S_IXOTH) ? 'x' : '-';
            //end string
            permissions[10] = '\0';
            printf("%s %3ld %8ld %s\n", permissions, file_stat.st_nlink, file_stat.st_size, files[i]);
            }
        }    
       
    }else{
        for(int i=0; i<count; i++){
            printf("%s\n", files[i]);
        }
    }

    for(int i=0; i<count; i++){
        free(files[i]);
    }


    if (errno != 0) {
        perror("Error to show files");
    }
    
    free(files);
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
    }else if(strcmp((command),"mkdir")==0){
        mkdirCommand(args,argc);
    }else {
        printf("Unknown command: %s\n", command);
    }
}

int main(int argc, char * argv[]){
    char inputString[MAXLETTERS];
    printf("Shell initiated, ready to run commands:\n");
    
    while(1){
        if(readingInput(inputString)){
            continue;
        }
    }
    return 0;
}
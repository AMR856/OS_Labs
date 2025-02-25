#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MAX_INPUT_TOKENS 20
#define MAX_BUFFER_SIZE 255

void shell_builtin(char **, int);
void shell();
void execute_command(char **);
void setup_environment();
void parse_input(char *, char**);
void read_input(char **);
int get_length(char **);
void my_echo(char **);
void my_cd(char **, int input_length);
void remove_newline(char **, int);

void main(void){
    setup_environment();
    shell();
}


void shell(){
    char *user_input = NULL;
    char *user_input_array[MAX_INPUT_TOKENS] = {NULL};
    int input_length; 
    char s[100]; 
    while (true){
        printf("Current directory: %s\n", getcwd(s, 100)); 
        read_input(&user_input);
        parse_input(user_input, user_input_array);
        input_length = get_length(user_input_array);
        remove_newline(user_input_array, input_length);
        if (!strcmp(user_input_array[0], "echo") ||
        !strcmp(user_input_array[0], "cd") ||
        !strcmp(user_input_array[0], "exit"))
            shell_builtin(user_input_array, input_length);
        else
            execute_command(user_input_array);
        memset(user_input_array, 0, sizeof(user_input_array));
    }
}

int get_length(char **user_input_array){
    int input_length = -1;
    while (user_input_array[++input_length] != NULL){}
    return input_length;
}

void read_input(char **user_input){
    size_t input_size = 255;
    *user_input = (char *)malloc(input_size * sizeof(char));
    if (*user_input == NULL){
        printf("User input wasn't allocated successfully");
        exit(1);
    }
    getline(user_input, &input_size, stdin);
}


void parse_input(char *user_input, char** user_input_array){
    int count = 0;
    char s[] = " ";
    char *tok = strtok(user_input, s);
    while (tok != NULL && count < MAX_INPUT_TOKENS) {
        user_input_array[count++] = tok; 
        tok = strtok(NULL, s);
    }
}

void setup_environment(){
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) chdir(cwd);
    else {
        printf("Error happended while executing getcwd()");
        exit(1);
    }
}

void shell_builtin(char **user_input_array, int input_length){
    if (!strcmp(user_input_array[0], "echo"))
        my_echo(user_input_array);
    else if (!strcmp(user_input_array[0], "exit"))
        exit(atoi(user_input_array[1]));
    else if (!strcmp(user_input_array[0], "cd"))
        my_cd(user_input_array, input_length);
}

void execute_command(char **user_input_array){
    pid_t pid;
    int status;
    pid = fork();
    if (pid == 0){
        printf("Child is here\n");
        status = execvp(user_input_array[0], user_input_array);
        if (status == -1){
            printf("Terminated Incorrectly\n");
            exit(1);
        }
    }
    printf("Parent is waiting\n");
    waitpid(pid, &status, 0);
    printf("Parent finished waiting\n");
}

void my_echo(char **user_input_array){
    int index = 1;
    while(user_input_array[index + 1] != NULL){
        printf("%s ", user_input_array[index]);
        index++;
    }
    printf("%s", user_input_array[index]);
}

void my_cd(char **user_input_array, int input_length){
    char cwd[PATH_MAX];
    // if (getcwd(cwd, sizeof(cwd)) != NULL)
    //     printf("Current working Directory: %s\n", cwd);
    if (input_length == 1 || !strcmp(user_input_array[1], "~"))
        chdir(getenv("HOME"));
    else
        chdir(user_input_array[1]);
}

void remove_newline(char **user_input_array, int length){
    for (int i = 0; i < length; i++) {
        if (user_input_array[i][strlen(user_input_array[i]) - 1] == '\n')
            user_input_array[i][strlen(user_input_array[i]) - 1] = '\0';
    }
}

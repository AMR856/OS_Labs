#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <ctype.h>

#define MAX_INPUT_TOKENS 20
#define MAX_BUFFER_SIZE 255
#define MAX_EXPORTED_VARIABLES_NUM 255

void shell_builtin(char **, int);
void shell();
void execute_command(char **, int);
void setup_environment();
void parse_input(char *, char**, bool *);
void read_input(char **);
int get_length(char **);
void my_echo(char **);
void my_cd(char **, int);
void remove_newline(char **, int);
void register_child_signal();
void handle_child_exit();
void my_export(char *);
void replace_variables(char *);

void main(void){
    register_child_signal();
    setup_environment();
    shell();
}

void handle_child_exit(){
    int status, pid;
    FILE *fptr;
    fptr = fopen("log.txt","a");
    if(fptr == NULL)
    {
        printf("Error while opening the file");   
        exit(1);             
    }
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
    }
    fprintf(fptr, "%s\n" , "Child process was terminated");
    fclose(fptr);
}

void register_child_signal(){
    signal(SIGCHLD, handle_child_exit);
}

void shell(){
    char *user_input = NULL;
    char *user_input_array[MAX_INPUT_TOKENS] = {NULL};
    int input_length;
    bool is_export = false;
    char s[100]; 
    while (true){
        printf("amralnas@simpleshell:%s$ ", getcwd(s, 100)); 
        read_input(&user_input);
        replace_variables(user_input);
        parse_input(user_input, user_input_array, &is_export);
        if (is_export) {
            is_export = false;
            continue;
        }
        input_length = get_length(user_input_array);
        remove_newline(user_input_array, input_length);
        if (!strcmp(user_input_array[0], "echo") ||
        !strcmp(user_input_array[0], "cd") ||
        !strcmp(user_input_array[0], "exit"))
            shell_builtin(user_input_array, input_length);
        else
            execute_command(user_input_array, input_length);
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

void parse_input(char *user_input, char** user_input_array, bool *is_exported){
    int count = 0;
    char s[] = " ";
    char *original;
    strcpy(original, user_input);
    char *tok = strtok(user_input, s);
    if (!strcmp(tok, "export")){
        my_export(original);
        *is_exported = true;
        return;
    }
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
    {
        if (user_input_array[1])
            exit(atoi(user_input_array[1]));
        else
            exit(0);
    }

    else if (!strcmp(user_input_array[0], "cd"))
        my_cd(user_input_array, input_length);
}

void execute_command(char **user_input_array, int input_length){
    pid_t pid;
    int status;
    pid = fork();
    if (pid < 0){
        printf("Process failed to be created");
        exit(1);
    }
    else if (pid == 0){
        // printf("Child is here\n");
        status = execvp(user_input_array[0], user_input_array);
        if (status == -1){
            printf("Terminated Incorrectly\n");
            exit(1);
        }
    }
    if (!strcmp(user_input_array[input_length - 1], "&"))
        waitpid(pid, &status, WNOHANG);
    else 
        waitpid(pid, &status, 0);
}

void my_echo(char **user_input_array){
    int index = 1;
    while(user_input_array[index + 1] != NULL){
        if (user_input_array[index][0] == '"')
            user_input_array[index]++;
        if (user_input_array[index][strlen(user_input_array[index]) - 1] == '"')
            user_input_array[index][strlen(user_input_array[index]) - 1] = '\0';
        printf("%s ", user_input_array[index]);
        index++;
    }
    printf("%s\n", user_input_array[index]);
}

void my_cd(char **user_input_array, int input_length){
    char cwd[PATH_MAX];
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

// export x="hello world"
void my_export(char *user_input) {
    char *tok = strtok(user_input, " ");
    char *var_name;
    if (tok == NULL || strcmp(tok, "export") != 0) {
        // Not an export command
        return;
    }

    tok = strtok(NULL, "=");
    if (tok == NULL) {
        // No variable name
        return;
    }
    var_name = tok;

    tok = strtok(NULL, "=");
    if (tok != NULL) {
        tok[strlen(tok) - 1] = '\0';
        if (tok[0] == '"' && tok[strlen(tok) - 1] == '"') {
            tok[strlen(tok) - 1] = '\0';
            tok++;
        }
        setenv(var_name, tok, 1);
    } else {
        setenv(var_name, "", 1);
    }
}

void replace_variables(char *user_input) {
    char new_input[MAX_BUFFER_SIZE];
    int i = 0, j = 0;

    while (user_input[i] != '\0') {
        if (user_input[i] == '$') {
            i++;
            char var_name[128] = {0};
            int k = 0;

            while (user_input[i] != '\0' && (isalnum(user_input[i]) || user_input[i] == '_')) {
                var_name[k++] = user_input[i++];
            }
            var_name[k] = '\0';
            char *value = getenv(var_name);
            if (value) {
                strcpy(&new_input[j], value);
                j += strlen(value);
            }
        } else {
            new_input[j++] = user_input[i++];
        }
    }

    new_input[j] = '\0';
    strcpy(user_input, new_input);
}

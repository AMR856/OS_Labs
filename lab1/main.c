/** Including the libraries that will be used in the program */
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

/** Some macros to determine the buffer size, and the number of tokens in the input */
#define MAX_INPUT_TOKENS 20
#define MAX_BUFFER_SIZE 255
#define MAX_EXPORTED_VARIABLES_NUM 255

/** All the functions declrations */
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

/**
 * main - The starting point of the program
 * it takes no arguments. In it, we call the function `register_child_signal` in which
 * we added a signal handler for the SIGCHID signals.
 * we also call the `setup_envionment` function that we use to change the starting
 * directory to the current directory
 * At the end, we call the `shell` function in which we have the main logic of the program.
 * Return: void
*/

void main(void){
    register_child_signal();
    setup_environment();
    shell();
}


/**
 * handle_child_exit - A function to handle the SIGCHID signal
 * from the child, and in the function we just wait for the zombie processes using
 * waitpid with WHOHANG, and when the parent finishes waiting for the child a message
 * `Child process was terminated` will be written to a log file
 * Return: void
*/

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

/**
 * register_child_signal - A function in which we register a handler called 
 * `handle_child_exit` to SIGCHID signal
 * Return: void
*/

void register_child_signal(){
    signal(SIGCHLD, handle_child_exit);
}

/**
 * shell - This function is the heart of the shell program
 * and most of the functions are used in it, first, we initialize the variables
 * that is going to be used to store the input (Normal or tokenized) and a flag to check if the command is
 * 'export' or not, after that, we enter a loop in which we read the input and replace any variables
 * (anything that has '$' in it), and after we tokenize the input with the ' ' delimiter if the command
 * is 'export', we set reset its flag and if not, we check if the command is built-in or a binary
 * file that should be utilized using exec family and, in the end, we null the array that contains the tokenized input
 * Return: void
*/

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

/**
 * register_child_signal - A function to get the length of a string
 * @user_input_array: A double pointer that points to an array of strings
 * which represents the tokenized input
 * Return: integer representing the length
*/

int get_length(char **user_input_array){
    int input_length = -1;
    while (user_input_array[++input_length] != NULL){}
    return input_length;
}

/**
 * read_input- A function to read the input of the user using getline
 * and it dynamically allocates the space for the input
 * @user_input_array: A double pointer that points to an array of strings
 * which represents the tokenized input
 * Return: void
*/

void read_input(char **user_input){
    size_t input_size = 255;
    *user_input = (char *)malloc(input_size * sizeof(char));
    if (*user_input == NULL){
        printf("User input wasn't allocated successfully");
        exit(1);
    }
    getline(user_input, &input_size, stdin);
}

/**
 * parse_input - A function to tokenize the input with the ' ' delimiter
 * and in the middle, it checks if the command is 'export' or not, if so, it gives it
 * the input without tokenization and set a flag to true
 * @user_input: A pointer to the original input of the user before being tokenized
 * @user_input_array: A double pointer that points to an array of strings
 * which represents the tokenized input
 * @is_exported: A pointer to a boolean value which is used to determine
 * if the command is 'export' or not
 * which represents the tokenized input
 * Return: void
*/

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

/**
 * setup_enviorment- A function to prepare the environment to start the shell and, all it does
 * is changing the directory to the current working directory
 * Return: void
*/

void setup_environment(){
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) chdir(cwd);
    else {
        printf("Error happended while executing getcwd()");
        exit(1);
    }
}

/**
 * shell_builtin - A function that is called; when the user is using a built-in command like
 * ['echo'; 'exit';  'cd'] ('export' will be handled differently) and after checking the command
 * and it calls the required function
 * @user_input_array: A double pointer that points to an array of strings
 * which represents the tokenized input
 * @input_length: An integer that stores the number of the tokens in the input
 * Return: void
*/

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


/**
 * execute_command - A function that is called when the user wants to execute commands that could be
 * found in the binary files and an example for these commands is 'ls' or 'mv'
 * and to handle executing the binaries we make a child process using 'fork' and we call execvp in the child
 * because if we don't do so, the content of our shell program will be overwritten
 * ['echo', 'exit',  'cd'] ('export' will be handled differently)  and after checking the command
 * @user_input_array: A double pointer that points to an array of strings
 * which represents the tokenized input
 * @input_length: An integer that stores the number of the tokens in the input
 * Return: void
*/

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

/**
 * my_echo - A function that is used to handle the 'echo' command and most of its logic
 * is removing the quotations that are around the input, and printing the rest.
 * @user_input_array: A double pointer that points to an array of strings
 * which represents the tokenized input
 * Return: void
*/

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

/**
 * my_cd - A function that is used to handle the 'cd' command. First it checks if
 * no paths were given. After that, it checks for the '~' and, each of them will get you
 * to the home directory, and if something else is given, the directory will be changed
 * to it
 * @user_input_array: A double pointer that points to an array of strings
 * which represents the tokenized input
 * @input_length: An integer that stores the number of the tokens in the input
 * Return: void
*/

void my_cd(char **user_input_array, int input_length){
    char cwd[PATH_MAX];
    if (input_length == 1 || !strcmp(user_input_array[1], "~"))
        chdir(getenv("HOME"));
    else
        chdir(user_input_array[1]);
}

/**
 * remove_newline - A functoion that is used to remove the newline from the input
 * after beign tokenized
 * @user_input_array: A double pointer that points to an array of strings
 * which represents the tokenized input
 * @input_length: An integer that stores the number of the tokens in the input
 * Return: void
*/

void remove_newline(char **user_input_array, int input_length){
    for (int i = 0; i < input_length; i++) {
        if (user_input_array[i][strlen(user_input_array[i]) - 1] == '\n')
            user_input_array[i][strlen(user_input_array[i]) - 1] = '\0';
    }
}

/**
 * my_export - A function to handle the 'export' command by first tokenizing the input
 * with the ' ' delimiter, and take the second half of it like in this example 'export x=5'
 * We take the x=5, and we tokenize it again with '=' delimiter.
 * After being tokenized we will have the variable name and the value separated in this case 'x' is the name
 * and we will give it the value '5' using the 'setenv' function
 * @user_input: A pointer to the original input of the user before being tokenized
 * Return: void
*/

void my_export(char *user_input) {
    // export x="hello world"
    char *tok = strtok(user_input, " ");
    char *var_name;
    if (tok == NULL || strcmp(tok, "export") != 0) return;
    tok = strtok(NULL, "=");
    if (tok == NULL) return;
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

/**
 * replace_variables - A functoion in which we replace the variables that we entered by
 * the user like $x and it does this by looping on the input, using the 'i' iterator
 * and if it finds a '$' sign, it incremanse the i by 1 to get to the name of the variable, after that
 * it starts looping on the name of the variable using the 'i' iterator for the original input
 * and 'k' iterator to store the value of the name in the 'var_name' variable, after finising looping
 * it puts a null termination char at the end of the input, and we use the 'var_name' to get the value
 * using 'getenv' function, and we copy it the 'new_input' varible which represents the input which contains
 * the replaced variables. We continue doing the same until we reach the end of the original input.
 * @user_input: A pointer to the original input of the user before being tokenized
 * Return: void
*/

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

#include <unistd.h>
#include <stdio.h>

int main(void){
    int status;
    char *arguments[] = {"ls", "ls -a -h", NULL}; 
    status = execvp("ls", arguments);
}
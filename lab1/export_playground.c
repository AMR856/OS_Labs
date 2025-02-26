#include <stdio.h>
#include <string.h>

#define MAX_EXPORTED_VARIABLES 100

typedef struct {
    char *name;
    char *value;
} ExportedVariable;

ExportedVariable exported_variable_array[MAX_EXPORTED_VARIABLES];
int exported_variables_count = 0;

void my_export(char *user_input) {
    char *tok = strtok(user_input, " ");
    if (tok == NULL || strcmp(tok, "export") != 0) {
        // Not an export command
        return;
    }

    tok = strtok(NULL, "=");
    if (tok == NULL) {
        // No variable name
        return;
    }
    exported_variable_array[exported_variables_count].name = tok;

    tok = strtok(NULL, "");
    if (tok != NULL) {
        if (tok[0] == '"' && tok[strlen(tok) - 1] == '"') {
            tok[strlen(tok) - 1] = '\0';
            tok++;
        }
        exported_variable_array[exported_variables_count].value = tok;
    } else {
        exported_variable_array[exported_variables_count].value = "";
    }
    exported_variables_count++;
}

int main() {
    char input[] = "export x=\"hello world\"";
    my_export(input);

    printf("Exported Variable:\n");
    printf("Name: %s\n", exported_variable_array[0].name);
    printf("Value: %s\n", exported_variable_array[0].value);

    return 0;
}
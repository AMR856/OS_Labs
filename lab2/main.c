#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_SIZE 255


typedef struct {
    int row_count;
    int cols_count;
    int **matrix;
} matrix_struct;

typedef struct{
    int row_num;
    int col_num;
    int **matrix_a;
    int **matrix_b;
    int **matrix_c;
} mul_matrix_data;

void read_matrix(char*, matrix_struct*, char *);
void print_matrix(matrix_struct *);
void free_matrix(matrix_struct *);
void write_matrix(matrix_struct *, char *);


int main(int args, char **argv){
    matrix_struct* a = (matrix_struct *)malloc(sizeof(matrix_struct));
    matrix_struct* b = (matrix_struct *)malloc(sizeof(matrix_struct));
    char *matrix_name, *filename;
    if (args == 1){
        matrix_name = (char *)malloc(2 * sizeof(char));
        strcpy(matrix_name, "a");
        read_matrix(filename, a, matrix_name);
        print_matrix(a);
        write_matrix(a, matrix_name);
        strcpy(matrix_name, "b");
        read_matrix(filename, b, matrix_name);
        print_matrix(b);
        write_matrix(b, matrix_name);
    } else {
        
    }
    free(matrix_name);
    free_matrix(a);
    free_matrix(b);
}

void read_matrix(char* filename, matrix_struct* full_matrix, char *matrix_name){
    FILE *fptr;
    filename = (char *)malloc(5 + strlen(matrix_name));
    strcpy(filename, matrix_name);
    strcat(filename, ".txt");
    fptr = fopen(filename, "r");
    char line[MAX_LINE_SIZE];
    if(fptr == NULL)
    {
        printf("Error while opening the file\n");   
        exit(1);             
    }
    if (fgets(line, MAX_LINE_SIZE, fptr) != NULL) {
        if (sscanf(line, "row=%d col=%d", &full_matrix->row_count, &full_matrix->cols_count) != 2){
            printf("Error while reading the file");
            free(filename);   
            exit(1);      
        }
        full_matrix->matrix = (int**)malloc(full_matrix->row_count * sizeof(int*));
        if (full_matrix->matrix == NULL) {
            printf("Memory allocation failed!\n");
            free(filename); 
            exit(1);
        }
        for (int i = 0; i < full_matrix->row_count; i++) {
            full_matrix->matrix[i] = (int*)malloc(full_matrix->cols_count * sizeof(int));
            if (full_matrix->matrix[i] == NULL) {
                printf("Memory allocation failed!\n");
                free(filename); 
                exit(1);
            }
        }
    }
    for (int i = 0; i < full_matrix->row_count; i++) {
        if (fgets(line, MAX_LINE_SIZE, fptr) != NULL) {
            int j = 0;
            char *tok = strtok(line, "\t");
            while (tok != NULL) {
                full_matrix->matrix[i][j++] = atoi(tok); 
                tok = strtok(NULL, "\t");
            }
        }
    }
    free(filename);
    fclose(fptr);
}
void print_matrix(matrix_struct *full_matrix){
    for (int i = 0; i < full_matrix->row_count; i++) {
        for (int j = 0; j < full_matrix->cols_count; j++) {
            printf("%d\t", full_matrix->matrix[i][j]);
        }
        printf("\n");
    }
}

void free_matrix(matrix_struct *full_matrix) {
    for (int i = 0; i < full_matrix->row_count; i++) {
        free(full_matrix->matrix[i]);
    }
    free(full_matrix->matrix);
}

void write_matrix(matrix_struct *full_matrix, char* matrix_name){
    FILE *fptr;
    char *filename = (char*)malloc(strlen(matrix_name) + 12);
    strcpy(filename, matrix_name);
    strcat(filename, "_output.txt");
    fptr = fopen(filename, "w");
    // row=10 col=5
    fprintf(fptr, "rows=%d col=%d\n", full_matrix->row_count, full_matrix->cols_count);
    for (int i = 0; i < full_matrix->row_count; i++) {
        for (int j = 0; j < full_matrix->cols_count; j++) {
            // printf("%d\t", full_matrix->matrix[i][j]);
            fprintf(fptr, "%d\t", full_matrix->matrix[i][j]);
        }
        fprintf(fptr, "\n");
    }
    free(filename);
    fclose(fptr);
}

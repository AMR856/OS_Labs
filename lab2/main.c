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
    int currnet_row;
    int currnet_col;
    matrix_struct *matrix_a;
    matrix_struct *matrix_b;
    matrix_struct *matrix_c;
} mul_matrix_data;

void read_matrix(char*, matrix_struct*, char *);
void print_matrix(matrix_struct *);
void free_matrix(matrix_struct *);
void write_matrix(matrix_struct *, char *, char *);
void multiplication(
    matrix_struct *,
    matrix_struct *,
    matrix_struct *,
    char *);
void threads_handle_multi_per_matrix(mul_matrix_data *);
void threads_handle_multi_per_row(mul_matrix_data *);
void threads_handle_multi_per_element(mul_matrix_data *);
void *multi_per_matrix(void *ptr);
void *multi_per_row(void *ptr);
void *multi_per_element(void *ptr);
mul_matrix_data* copy_multi_data(mul_matrix_data *);

void freeing_function(
    char *,
    matrix_struct *,
    matrix_struct *,
    matrix_struct *,
    char *
);

int main(int argc, char **argv){
    matrix_struct* a = (matrix_struct *)malloc(sizeof(matrix_struct));
    matrix_struct* b = (matrix_struct *)malloc(sizeof(matrix_struct));
    matrix_struct* c = (matrix_struct *)malloc(sizeof(matrix_struct));
    
    char *matrix_name = (char *)malloc(2 * sizeof(char));
    char *filename = (char *)malloc(20 * sizeof(char));

    if (argc == 1){
        strcpy(matrix_name, "a");
        read_matrix(filename, a, matrix_name);
        strcpy(matrix_name, "b");
        read_matrix(filename, b, matrix_name);
        multiplication(a, b, c, "c");
    } else {
        if (argc != 4){
            printf("Wrong number of inputs\n");
            exit(1);
        }
        read_matrix(filename, a, argv[1]);
        read_matrix(filename, b, argv[2]);
        multiplication(a, b, c, argv[3]);
    }
    freeing_function(matrix_name, a, b, c, filename);
}

void read_matrix(char* filename, matrix_struct* full_matrix, char *matrix_name){
    FILE *fptr;
    sprintf(filename, "%s.txt", matrix_name);
    fptr = fopen(filename, "r");
    char line[MAX_LINE_SIZE];
    if(fptr == NULL) {
        printf("Error while opening the file\n");
        free(filename);
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
            // char *tok = strtok(line, " ");
            char *tok = strtok(line, "\t");
            while (tok != NULL) {
                full_matrix->matrix[i][j++] = atoi(tok); 
                tok = strtok(NULL, "\t");
                // tok = strtok(NULL, " ");
            }
        }
    }
    memset(filename, '\0', strlen(filename));
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

void write_matrix(matrix_struct *full_matrix, char* matrix_name, char* file_type){
    FILE *fptr;
    char *filename = (char*)malloc(strlen(matrix_name) + strlen(file_type) + 5);
    strcpy(filename, matrix_name);
    if (!strcmp(file_type, "_per_matrix.txt"))
        strcat(filename, "_per_matrix.txt");
    else if(!strcmp(file_type, "_per_row.txt"))
        strcat(filename, "_per_row.txt");
    else if (!strcmp(file_type, "_per_element.txt"))
        strcat(filename, "_per_element.txt");
    else
        strcat(filename, ".txt");
    fptr = fopen(filename, "w");
    fprintf(fptr, "rows=%d col=%d\n", full_matrix->row_count, full_matrix->cols_count);
    for (int i = 0; i < full_matrix->row_count; i++) {
        for (int j = 0; j < full_matrix->cols_count; j++) {
            fprintf(fptr, "%d\t", full_matrix->matrix[i][j]);
        }
        fprintf(fptr, "\n");
    }
    fclose(fptr);
}


void multiplication(matrix_struct *a,
    matrix_struct *b,
    matrix_struct *c,
    char *output_matrix_name){
    c->row_count = a->row_count, c->cols_count = b->cols_count;
    c->matrix = (int**)calloc(c->row_count, sizeof(int *));
    if (c->matrix == NULL) {
        printf("Memory allocation failed!\n");
        free(c);
        exit(1);
    }
    for (int i = 0; i < c->row_count; i++) {
        c->matrix[i] = (int*)calloc(c->cols_count,  sizeof(int));
        if (c->matrix[i] == NULL) {
            printf("Memory allocation failed!\n");
            free_matrix(c);
            exit(1);
        }
    }
    mul_matrix_data *multi = (mul_matrix_data*)malloc(sizeof(mul_matrix_data));
    multi->matrix_a = a, multi->matrix_b = b, multi->matrix_c = c;
    threads_handle_multi_per_matrix(multi);
    // printf("Matrix C Using Per Matrix Method: \n");
    // print_matrix(c);
    write_matrix(c, output_matrix_name, "_per_matrix.txt");
    threads_handle_multi_per_row(multi);
    // printf("Matrix C Using Per Row Method: \n");
    // print_matrix(c);
    write_matrix(c, output_matrix_name, "_per_row.txt");
    threads_handle_multi_per_element(multi);
    // printf("Matrix C Using Per Element Method: \n");
    // print_matrix(c);
    write_matrix(c, output_matrix_name, "_per_element.txt");
    free(multi);
}


void threads_handle_multi_per_matrix(mul_matrix_data *multi){
    pthread_t thread;
    if (pthread_create(&thread, NULL, &multi_per_matrix, (void *)multi)) exit(1);
    if (pthread_join(thread, NULL)) exit(1);
}

void threads_handle_multi_per_row(mul_matrix_data *multi){
    int rows_count = multi->matrix_a->row_count;
    pthread_t threads[rows_count];
    for (int i = 0; i < rows_count; i++){
        mul_matrix_data *multi_copy = copy_multi_data(multi);
        multi_copy->currnet_row = i, multi_copy->currnet_col = 0;
        if (pthread_create(&threads[i], NULL, &multi_per_row, (void *)multi_copy)) exit(1);
    }
    for (int  i= 0; i < rows_count; i++){
        if (pthread_join(threads[i], NULL)) exit(1);
    }
}

void threads_handle_multi_per_element(mul_matrix_data *multi){
    int rows_count = multi->matrix_a->row_count;
    int cols_count = multi->matrix_b->cols_count;
    pthread_t threads[rows_count][cols_count];
    for (int i = 0; i < rows_count; i++){
        for (int j = 0; j < cols_count; j++){
            mul_matrix_data *multi_copy = copy_multi_data(multi);
            multi_copy->currnet_row = i, multi_copy->currnet_col = j;
            if (pthread_create(&threads[i][j], NULL, &multi_per_element, (void *)multi_copy)) exit(1);
        }
    }
    for (int  i= 0; i < rows_count; i++){
        for (int j = 0; j < cols_count; j++){
            if (pthread_join(threads[i][j], NULL)) exit(1);
        }
    }
}

void *multi_per_matrix(void *ptr){
    mul_matrix_data *data = (mul_matrix_data *)ptr;
    for (int i = 0; i < data->matrix_a->row_count; i++){
        for (int j = 0; j < data->matrix_b->cols_count; j++){
            int sum = 0;
            for (int k = 0; k < data->matrix_b->row_count; k++)
                sum += data->matrix_a->matrix[i][k] * data->matrix_b->matrix[k][j];
            data->matrix_c->matrix[i][j] = sum;
        }
    }
    pthread_exit(NULL);
}

void *multi_per_row(void *ptr){
    mul_matrix_data *data = (mul_matrix_data *)ptr;
    for (int j = 0; j < data->matrix_b->cols_count; j++){
        int sum = 0;
        for (int k = 0; k < data->matrix_a->cols_count; k++){
            sum += data->matrix_a->matrix[data->currnet_row][k] * data->matrix_b->matrix[k][j];
        }
        data->matrix_c->matrix[data->currnet_row][j] = sum;
    }
    free(data);
    pthread_exit(NULL);
}

void *multi_per_element(void *ptr){
    mul_matrix_data *data = (mul_matrix_data *)ptr;
    int sum = 0;
    for (int k = 0; k < data->matrix_a->cols_count; k++){
        sum +=
        data->matrix_a->matrix[data->currnet_row][k] *
        data->matrix_b->matrix[k][data->currnet_col];
    }
    data->matrix_c->matrix[data->currnet_row][data->currnet_col] = sum;
    free(data);
    pthread_exit(NULL);
}

mul_matrix_data* copy_multi_data(mul_matrix_data *src){
    mul_matrix_data *dest = (mul_matrix_data *)malloc(sizeof(mul_matrix_data));
    dest->matrix_a = src->matrix_a, dest->matrix_b = src->matrix_b, dest->matrix_c = src->matrix_c;
}

void freeing_function(
    char *matrix_name,
    matrix_struct *a,
    matrix_struct *b,
    matrix_struct *c,
    char *filename
){
    free(matrix_name);
    free_matrix(a);
    free(a);
    free_matrix(b);
    free(b);
    free_matrix(c);
    free(c);
    free(filename);
}

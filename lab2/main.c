/** Including the libraries that will be used in the program */
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

/** Some macros to determine the buffer size, and the number of tokens in the input */
#define MAX_LINE_SIZE 255


/** matrix_struct
 * A struct that contains all the information about the matrix
 * @row_count: The number of rows in the matrix
 * @col_count: The number of cols in the matrix
 * @matrix: A pointer to a poniter which we use to store the matrix
 * */

typedef struct {
    int row_count;
    int cols_count;
    int **matrix;
} matrix_struct;

/** matrix_struct
 * A struct that contains all the information we need while performing the multiplication
 * @row_count: The current row in the maltiplciation process
 * @col_count: The current col in the maltiplciation process
 * @matrix: A pointer to matrix A
 * @matrix_b: A pointer to matrix B
 * @matrix_c: A pionter to matrix C
 * */

typedef struct{
    int currnet_row;
    int currnet_col;
    matrix_struct *matrix_a;
    matrix_struct *matrix_b;
    matrix_struct *matrix_c;
} mul_matrix_data;

/** All the functions declrations */
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

/**
 * main - Entry point of the matrix multiplication program.
 * @argc: The number of command-line arguments.
 * @argv: An array of strings representing command-line arguments.
 *
 * This function initializes three matrix structures and reads matrices
 * from files to perform multiplication. If no command-line arguments
 * are provided, it reads default matrices "a" and "b" from a predefined file.
 * Otherwise, it expects three arguments: the names of matrices A, B, and the result C.
 * The program performs matrix multiplication and frees allocated memory before exiting.
 *
 * Return: 0 on successful execution, exits with status 1 on failure.
 */

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

/**
 * read_matrix - Reads a matrix from a file and initializes its structure.
 * @filename: The name of the file to read from.
 * @full_matrix: A pointer to the matrix_struct where the matrix will be stored.
 * @matrix_name: The name of the matrix to read (used to construct the filename).
 *
 * This function opens a file named "<matrix_name>.txt", reads the matrix dimensions,
 * allocates memory for the matrix, and populates it with values from the file.
 * If the file cannot be opened or memory allocation fails, the program exits with an error 1.
 */

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
        for (int j = 0; j < full_matrix->cols_count; j++) {
            if (fscanf(fptr, "%d", &full_matrix->matrix[i][j]) != 1) {
                printf("Error reading the file: %s\n", filename);
            }
        }
    }
    memset(filename, '\0', strlen(filename));
    fclose(fptr);
}


/**
 * print_matrix - A function to print the matrix to the terminal
 * @full_matrix: A pointer to the matrix_struct where the matrix will be stored.
 * This function justs loops through the matrix and prints its elements
 * separting them with tabs and a newilne at the end of the file.
 * Return: void
 */

void print_matrix(matrix_struct *full_matrix){
    for (int i = 0; i < full_matrix->row_count; i++) {
        for (int j = 0; j < full_matrix->cols_count; j++) {
            printf("%d\t", full_matrix->matrix[i][j]);
        }
        printf("\n");
    }
}

/**
 * free_matrix - A function to free the allocated memory for the matrix in the heap
 * @full_matrix: A pointer to the matrix_struct where the matrix will be stored.
 * This function frees all the rows of the matrix first and after that it frees
 * the pointer to it
 * Return: void
 */

void free_matrix(matrix_struct *full_matrix) {
    for (int i = 0; i < full_matrix->row_count; i++) {
        free(full_matrix->matrix[i]);
    }
    free(full_matrix->matrix);
}

/**
 * write_matrix - Writes a matrix to a file
 * @full_matrix: A pointer to the matrix_struct where the matrix will be stored.
 * @matrix_name: The base name of the output file.
 * @file_type: The format of the output file (_per_matrix.txt, _per_row.txt, _per_element.txt, or default).
 *
 * This function creates a filename based on the matrix name and it writes the
 * matrix dimensions followed by the matrix values into the file.
 * The function ensures that memory is allocated for the filename and properly freed.
 */

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
    fprintf(fptr, "row=%d col=%d\n", full_matrix->row_count, full_matrix->cols_count);
    for (int i = 0; i < full_matrix->row_count; i++) {
        for (int j = 0; j < full_matrix->cols_count; j++) {
            if (i == full_matrix->row_count - 1 && j == full_matrix->cols_count - 1)
                fprintf(fptr, "%d", full_matrix->matrix[i][j]);
            else
                fprintf(fptr, "%d ", full_matrix->matrix[i][j]);
        }
        fprintf(fptr, "\n");
    }
    free(filename);
    fclose(fptr);
}

/**
 * multiplication - Performs matrix multiplication using multithreading.
 * @a: A pointer to the first matrix (multiplicand).
 * @b: A pointer to the second matrix (multiplier).
 * @c: A pointer to the result matrix.
 * @output_matrix_name: The base name for the output matrix files.
 *
 * This function initializes matrix C, allocates memory for it, and performs
 * matrix multiplication using three different threading approaches:
 *   - One Thread Per Matrix
 *   - One Thread Per Row
 *   - One Thread Per Element
 *
 * It records execution time for each approach and writes the resulting matrices
 * to files in different files. If memory allocation fails, the function exits with an error.
 */

void multiplication(
    matrix_struct *a,
    matrix_struct *b,
    matrix_struct *c,
    char *output_matrix_name){
    struct timeval stop, start;

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

    // One Thread Per Matrix Approach
    gettimeofday(&start, NULL);
    threads_handle_multi_per_matrix(multi);
    gettimeofday(&stop, NULL);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    write_matrix(c, output_matrix_name, "_per_matrix.txt");

    // One Thread Per Row Approach
    gettimeofday(&start, NULL);
    threads_handle_multi_per_row(multi);
    gettimeofday(&stop, NULL);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    write_matrix(c, output_matrix_name, "_per_row.txt");


    // One Thread Per Element Approach
    gettimeofday(&start, NULL);
    threads_handle_multi_per_element(multi);
    gettimeofday(&stop, NULL);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    write_matrix(c, output_matrix_name, "_per_element.txt");

    free(multi);
}

/**
 * threads_handle_multi_per_matrix - Creates a single thread for matrix multiplication.
 * @multi: A pointer to the mul_matrix_data structure containing matrix data.
 *
 * This function spawns a single thread to perform the entire matrix multiplication
 * using the `multi_per_matrix` function and waits for it to complete.
 */

void threads_handle_multi_per_matrix(mul_matrix_data *multi){
    pthread_t thread;
    if (pthread_create(&thread, NULL, multi_per_matrix, (void *)multi)) exit(1);
    pthread_join(thread, NULL);
}

/**
 * threads_handle_multi_per_row - Creates a thread for each row in the result matrix.
 * @multi: A pointer to the mul_matrix_data structure containing matrix data.
 *
 * This function spawns a thread for each row in the result matrix to perform multiplication.
 * Each thread executes the `multi_per_row` function and is responsible for computing
 * a single row of the resulting matrix.
 */

void threads_handle_multi_per_row(mul_matrix_data *multi){
    int rows_count = multi->matrix_a->row_count;
    pthread_t threads[rows_count];
    for (int i = 0; i < rows_count; i++){
        mul_matrix_data *multi_copy = copy_multi_data(multi);
        multi_copy->currnet_row = i, multi_copy->currnet_col = 0;
        if (pthread_create(&threads[i], NULL, multi_per_row, (void *)multi_copy)) exit(1);
    }
    for (int  i= 0; i < rows_count; i++){
        pthread_join(threads[i], NULL);
    }
}

/**
 * threads_handle_multi_per_element - Creates a thread for each element in the result matrix.
 * @multi: A pointer to the mul_matrix_data structure containing matrix data.
 *
 * This function spawns a separate thread for each element in the result matrix.
 * Each thread executes the `multi_per_element` function to compute a single element.
 */

void threads_handle_multi_per_element(mul_matrix_data *multi){
    int rows_count = multi->matrix_a->row_count;
    int cols_count = multi->matrix_b->cols_count;
    pthread_t threads[rows_count][cols_count];
    for (int i = 0; i < rows_count; i++){
        for (int j = 0; j < cols_count; j++){
            mul_matrix_data *multi_copy = copy_multi_data(multi);
            multi_copy->currnet_row = i, multi_copy->currnet_col = j;
            if (pthread_create(&threads[i][j], NULL, multi_per_element, (void *)multi_copy)) exit(1);
        }
    }
    for (int  i= 0; i < rows_count; i++){
        for (int j = 0; j < cols_count; j++){
            pthread_join(threads[i][j], NULL);
        }
    }
}

/**
 * multi_per_matrix - Performs matrix multiplication using a single thread.
 * @ptr: A pointer to the mul_matrix_data structure containing matrix data.
 *
 * This function iterates over each row and column of the matrices and computes
 * the multiplication result for the entire matrix. The results are stored in `matrix_c`.
 * Once completed, the thread exits.
 */

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

/**
 * multi_per_row - Computes a single row of the resulting matrix using a thread.
 * @ptr: A pointer to the mul_matrix_data structure containing matrix data.
 *
 * This function calculates the matrix multiplicatoin for a specific row in the result matrix.
 * Once the row computation is complete, the allocated memory for the thread's copy
 * of `mul_matrix_data` is freed before exiting.
 */

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

/**
 * multi_per_element - Computes a single element in the result matrix using a thread.
 * @ptr: A pointer to the mul_matrix_data structure containing matrix data.
 *
 * This function calculates the matrix multplication for a specific element in the result matrix.
 * After storing the computed value in `matrix_c`, the allocated memory for the thread's
 * copy of `mul_matrix_data` is freed before exiting.
 */

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
    return dest;
}

/**
 * freeing_function - Frees allocated memory for matrices and filenames.
 * @matrix_name: A pointer to the allocated memory for the matrix name.
 * @a: A pointer to the first matrix (multiplicand).
 * @b: A pointer to the second matrix (multiplier).
 * @c: A pointer to the result matrix.
 * @filename: A pointer to the allocated memory for the filename.
 *
 * This function releases all dynamically allocated memory associated with 
 * the matrices and their structures. It calls `free_matrix` to free the 
 * matrix data before freeing the matrix structure itself. Additionally, 
 * it frees allocated memory for `matrix_name` and `filename`.
 */

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

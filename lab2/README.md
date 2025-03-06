# Matrix Multiplication Using Multithreading

This project implements matrix multiplication using multithreading in C. It provides three different approaches:

1. **One Thread Per Matrix** - A single thread handles the entire matrix multiplication.
2. **One Thread Per Row** - Each row of the resultant matrix is computed in a separate thread.
3. **One Thread Per Element** - Each element of the resultant matrix is computed in a separate thread.

## Features
- Reads matrices from text files.
- Supports multithreaded matrix multiplication.
- Writes output to separate files for each approach.
- Measures execution time for performance comparison.

## Compilation
To compile the program, use:
```sh
gcc -o matrix_mul matrix_mul.c -pthread
```

## Usage
Run the program using:
```sh
./matrix_mul [matrixA] [matrixB] [output_matrix]
```
- `matrixA` - Filename of the first matrix.
- `matrixB` - Filename of the second matrix.
- `output_matrix` - Filename for storing results.

If no arguments are provided, the program defaults to reading matrices `a.txt` and `b.txt` and storing results in `c.txt`.

## Input Format
Each matrix file should have the following format:
```
row=<number_of_rows> col=<number_of_columns>
element11 element12 ... element1N
element21 element22 ... element2N
...
elementM1 elementM2 ... elementMN
```
Example (`a.txt`):
```
row=2 col=3
1 2 3
4 5 6
```

## Output
The program writes results in three different files:
- `<output_matrix>_per_matrix.txt`
- `<output_matrix>_per_row.txt`
- `<output_matrix>_per_element.txt`

Each file follows the same matrix format as input.

## Performance Measurement
The execution time for each approach is displayed in microseconds.

## Memory Management
The program dynamically allocates memory for matrices and ensures proper deallocation to prevent memory leaks.

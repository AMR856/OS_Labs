import numpy as np
from typing import List
import sys
def read_matrix(filename: str) -> List[List[int]]:
    matrix: List[List[int]] = []
    with open(filename, 'r', encoding='utf8') as file:
        file_lines = file.readlines()
        # splitted_first_line = file_lines[0].split()
        # rows = int(splitted_first_line[0].split('=')[2])
        # cols = int(splitted_first_line[1].split('=')[2])
        for line in file_lines[1:]:
            matrix.append(list(map(int, line.split())))
    return matrix

if __name__ == '__main__':
    A = np.array(read_matrix(sys.argv[1] + '.txt'))
    B = np.array(read_matrix(sys.argv[2] + '.txt'))
    print(np.dot(A, B))

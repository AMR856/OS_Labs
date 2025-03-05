import sys
import hashlib
import difflib

def hash_file(filename):
    """Returns the SHA256 hash of the file"""
    hasher = hashlib.sha256()
    with open(filename, 'rb') as f:
        while chunk := f.read(8192):
            hasher.update(chunk)
    return hasher.hexdigest()

def compare_files(file1, file2):
    """Compares two files and prints whether they are identical or different"""
    if hash_file(file1) == hash_file(file2):
        print("Files are identical.")
    else:
        print("Files are different.\n")
        find_differences(file1, file2)

def find_differences(file1, file2):
    """Finds and prints the differences between two files line by line"""
    with open(file1, 'r', encoding='utf-8') as f1, open(file2, 'r', encoding='utf-8') as f2:
        f1_lines = f1.readlines()
        f2_lines = f2.readlines()

    diff = difflib.unified_diff(f1_lines, f2_lines, fromfile=file1, tofile=file2, lineterm='')
    
    print("Differences:")
    for line in diff:
        print(line)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python compare_files.py <file1> <file2>")
        sys.exit(1)

    file1, file2 = sys.argv[1], sys.argv[2]
    compare_files(file1, file2)

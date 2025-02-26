# Simple Shell

A lightweight custom shell implemented in C that supports basic commands, variable substitution, and process handling.

## Features
- **Basic Shell Commands**: Supports running external commands (e.g., `ls`, `pwd`)
- **Built-in Commands**:
  - `echo`: Prints text to the console
  - `cd`: Changes the current directory
  - `exit`: Exits the shell with an optional status code
- **Variable Expansion**: Replaces variables like `$VAR` with their values
- **Exporting Variables**: Allows defining environment variables using `export VAR="value"`
- **Process Management**: Supports both foreground and background execution using `&`
- **Signal Handling**: Handles child process termination properly

## Compilation & Execution
To compile the shell, use:
```sh
gcc -o shell main.c
```

Run the shell with:
```sh
./shell
```

## Usage
Once inside the shell, you can:
- Execute commands like `ls`, `pwd`, etc.
- Change directories using `cd <directory>`
- Print text using `echo "Hello World"`
- Export variables with `export x="Hello"` and use them with `echo $x`
- Run commands in the background using `command &`
- Exit using `exit` or `exit <status_code>`

## Code Overview
### `main()`
- Registers the child process signal handler
- Sets up the environment
- Starts the shell loop

### `shell()`
- Reads user input
- Expands variables
- Parses input
- Determines if a built-in command or external command should be executed

### `execute_command()`
- Forks a new process and executes the given command using `execvp()`
- Waits for completion unless running in the background

### `replace_variables()`
- Replaces `$VAR` with its value from the environment

### `my_export()`
- Handles `export VAR=value` by storing variables in the environment

### `handle_child_exit()`
- Handles terminated child processes and logs to `log.txt`

# Implementing a Primitive Shell

To implement a very primitive shell that knows how to launch new programs in the foreground and the background. If a command is followed by `&`, the command is executed in the background. That is, the shell starts the execution of the command and return to prompt the user for the next input without waiting for the previous command to complete. The output of the command can get printed to the shell as and when it appears.

## Internal Commands

The shell recognize the following internal commands:

- `clear`: clear the screen
- `env`: display all environment parameters
- `cd <dir>`: change to the specified directory
- `pwd`: print the current directory
- `mkdir <dir>`: create a directory called "dir"
- `rmdir <dir>`: remove the directory called "dir"
- `ls`: list files in the current directory (`ls -l` option also needs to be supported)
- `history`: display the last commands the user ran, with an offset next to each command. Last commands can be stored in a file and may be displayed to the user when the command is issued.
- `history <argument>`: display the given number of commands as specified
- `exit`: exit the shell

## Running External Programs

If the command line does not indicate any internal commands, it should be in the following form:
`<program name> <arg1> <arg2> .... <argN>`

Your shell should invoke the program, passing it the list of arguments from the command line. The shell must wait until the started program completes.

To allow users to pass arguments, you need to parse the input line into words separated by whitespace (spaces and '\t' tab characters).

## Running the Program

To run the program, follow these steps:

1. Download the provided zip folder.
2. Extract all the files along with the folder.
3. Open the folder and open the terminal in that folder.
4. Run the following commands:
   - `$ gcc lab1.c -o lab1`
   - `./lab1 <file_name>` (make sure the file exists in the same directory)

*Note: The `test.txt` file contains some sample commands for running in batch mode. After running the file, it asks for input from the user. There are two types of commands that can be run: internal commands implemented for input and general UNIX commands as external commands. Errors such as wrong commands, wrong directories with the `cd` command, fork failures, `execvp()` failures, and `dup2()` failures are handled.*

To run a command in the background, use `&` after the command.


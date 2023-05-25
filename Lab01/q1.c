#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <signal.h>

#define DELIMITER " \t\r\n\a"
#define MAXARGNUM 32
#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

void sigsegvHandler(int sig_num)
{
    signal(SIGSEGV, sigsegvHandler);
    fflush(stdout);
    exit(0);
}

char **tokenize(char *line)
{
	char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
	char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
	int i, tokenIndex = 0, tokenNo = 0;

	for (i = 0; i < strlen(line); i++)
	{
		char readChar = line[i];
		if (readChar == ' ' || readChar == '\n' || readChar == '\t')
		{
			token[tokenIndex] = '\0';
			if (tokenIndex != 0)
			{
				tokens[tokenNo] = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
				strcpy(tokens[tokenNo++], token);
				tokenIndex = 0;
			}
		}
		else
		{
			token[tokenIndex++] = readChar;
		}
	}
	free(token);
	tokens[tokenNo] = NULL;
	return tokens;
}

int pipeCounter(char *line)
{
	int i =0;
	int numpipes=0;
	while (line[i] != '\0')
	{
		if (i > 0)
		{
			if (line[i] == '|' && line[i + 1] != '|' && line[i - 1] != '|')
            {
				numpipes++;
            }
        }
		i++;
	}
	return numpipes;
}

void pipeCmd(char *line)
{
	int i, commandc = 0, numpipes = 0;
	pid_t pid;
	char **args;
	i = 0;
	numpipes = pipeCounter(line);
	int *pipefds = (int *)malloc((2 * numpipes) * sizeof(int));
	char *token = (char *)malloc((128) * sizeof(char));
	token = strtok_r(line, "|", &line);
	for (i = 0; i < numpipes; i++)
	{
		if (pipe(pipefds + i * 2) < 0)
		{
			perror("pipe creation failed");
			return;
		}
	}
	do
	{
		pid = fork();
		if (pid == 0)
		{
			//child process
			if (commandc != 0)
			{
				if (dup2(pipefds[(commandc - 1) * 2], 0) < 0)
				{
					perror("child couldnt get input");
					exit(1);
				}
			}
			if (commandc != numpipes)
			{
				if (dup2(pipefds[commandc * 2 + 1], 1) < 0)
				{
					perror("child couldnt output");
					exit(1);
				}
			}
			for (i = 0; i < 2 * numpipes; i++)
			{
				close(pipefds[i]);
			}
			args = tokenize(token);
			if (execvp(args[0], args) < 0)
			{
				printf("exec failed");
				exit(1);
			}
		}
		else if (pid < 0)
		{
			perror("fork() failed");
			return;
		} //fork error
		commandc++; //parent process
	} while (commandc < numpipes + 1 && (token = strtok_r(NULL, "|", &line)));
	i = 0;
	while (i < 2 * numpipes)
	{
		close(pipefds[i]);
		i++;
	}
	free(pipefds);
	return;
}

void pwd()
{
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	printf("\n%s\n", cwd);
}

// Function where the system command is executed
void simpleCmd(char **tokens, int an)
{
	// Forking a child
	pid_t pid = fork();
	// catch the errors
	if (pid == -1)
	{
		printf("\nFailed forking child..");
	}
	else if (pid == 0)
	{
		if (execvp(tokens[0], tokens) < 0)
		{
			printf("\nCould not execute command..\n");
			return;
		}
		exit(0);
	}
	else
	{
		//if background process, then do not wait for child process to terminate
		if (an == 1)
			return;
		// waiting for child to terminate in foreground
		wait(NULL);
		return;
	}
}

/* Splits the string by space and returns the array of tokens */

////////////////////////  MAIN FUNCTION  ////////////////////////

int main(int argc, char *argv[])
{
	signal(SIGSEGV, sigsegvHandler); 
	// for taking input
	char line[MAX_INPUT_SIZE];
	char **tokens;
	int i;
	FILE *fp;
	if (argc == 2)
	{
		fp = fopen(argv[1], "r");
		if (fp < 0)
		{
			printf("File doesn't exists.");
			return -1;
		}
	}
	while (1)
	{
		// variables for type of command
		int isPipe = 0;
		int isBuiltin = 0;
		bzero(line, sizeof(line));
		if (argc == 2)
		{
			// batch mode
			// read every line from the opened file
			if (fgets(line, sizeof(line), fp) == NULL)
			{
				// file reading finished
				break;
			}
			if (line[strlen(line) - 1] == '\n')
				line[strlen(line) - 1] = '\0';
		}
		else
		{
			// interactive mode
			printf("$ ");
			scanf("%[^\n]", line);
			getchar();
		}
		// if no command is given
		if (strcmp(line, "") == 0)
			continue;
		printf("Command entered: %s\n", line);
		line[strlen(line)] = '\n'; //terminate with new line
		//make tokens
		tokens = tokenize(line);

		////////////////////////////////////// INTERNAL COMMANDS //////////////////////////////////////
		// checking whether the first token is a builtin command
		if (strcmp(tokens[0], "exit") == 0)
		{
			isBuiltin = 1;
			printf("\nGoodbye\n");
			exit(0);
		}
		else if (strcmp(tokens[0], "pwd") == 0)
		{
			isBuiltin = 1;
			pwd();
		}
		else if (strcmp(tokens[0], "cd") == 0)
		{
			isBuiltin = 1;
			if (chdir(tokens[1]) != 0)
			{
				printf("Please enter a valid directory");
				pwd();
				continue;
			};
			pwd();
		}
		else if (strcmp(tokens[0], "ls") == 0 && !tokens[1])
		{
			isBuiltin = 1;
			struct dirent *de;
			DIR *dr = opendir(".");
			if (dr == NULL)
			{
				printf("Could not open");
				continue;
			}
			while ((de = readdir(dr)) != NULL)
				printf("%s  ", de->d_name);
			printf("\n");
			closedir(dr);
		}
		// if command is builtin, then already exectued
		if (isBuiltin == 1)
			continue;

		////////////////////////////////////// EXTERNAL COMMANDS //////////////////////////////////////
		//Pipe check
		for (i = 0; tokens[i] != NULL; i++)
		{
			if (strcmp(tokens[i], "|") == 0)
			{
				isPipe = 1;
				break;
			}
		}
		int an = 0;
		if (isPipe == 0)
		{
			// simple process
			// check for background process
			if (strcmp(tokens[i - 1], "&") == 0)
			{
				an = 1;
				tokens[i - 1] = NULL;
			}
			simpleCmd(tokens, an);
		}
		else
		{
			//piped process
			pipeCmd(line);
			sleep(1);
		}
		// Freeing the allocated memory
		for (i = 0; tokens[i] != NULL; i++)
		{
			free(tokens[i]);
		}
		free(tokens);
	}
	return 0;
}
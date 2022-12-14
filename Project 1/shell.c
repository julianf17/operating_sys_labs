#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
//<Author> Julian Forbes @julianf17
// collaborated with Kyndall Jones and Zoe Carter
#define MAX_COMMAND_LINE_LEN 1024
#define MAX_COMMAND_LINE_ARGS 128 
char prompt[] = ">";
char delimiters[] = " \t\r\n";
extern char ** environ;
int time = 0;

int main() {
    char command_line[MAX_COMMAND_LINE_LEN];
    char cmd_bak[MAX_COMMAND_LINE_LEN];
  
   
    char *arguments[MAX_COMMAND_LINE_ARGS];  // Stores tokenized command line input.
    
    char s[100];

    while (true) {
      
        do{ 
            printf("%s%s", getcwd(s, 100), prompt);
            fflush(stdout);

            // Read input from stdin and store it in command_line. If there's an
            // error, exit immediately. (If you want to learn more about this line,
            // you can Google "man fgets")
        
            if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
                fprintf(stderr, "fgets error");
                exit(0);
            }
 
        }while(command_line[0] == 0x0A);  // while just ENTER pressed

      
        // If the user input was EOF (ctrl+d), exit the shell.
        if (feof(stdin)) {
            printf("\n");
            fflush(stdout);
            fflush(stderr);
            return 0;
        }

        // TODO:
        // 
        
        // 0. Modify the prompt to print the current working directory
			
        // 1. Tokenize the command line input (split it on whitespace)
        
        int count = 0;
        char *token = strtok(command_line, delimiters);
        char *tokens[MAX_COMMAND_LINE_ARGS] = {};

        char last[10];
        int lastindex;
        
        while (token != NULL) {
            if (strncmp("$", token, 1) == 0) {
                char *copy = token + 1;
                tokens[count] = getenv(copy);
            }
            else {
                tokens[count] = token;
            }

            strcpy(last, token);
            lastindex = count;
            count++;
            token = strtok(NULL, delimiters);
        }

        // 2. Implement Built-In Commands
        
        // pwd
        if (strcmp(tokens[0], "pwd") == 0) {
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) == NULL) perror("getcwd() error");
            else printf("%s\n", cwd);
        }

        // cd
        else if (strcmp(tokens[0], "cd") == 0) {
            if (chdir(tokens[1]) != 0)
                perror("chdir() error()");
        }

        // echo
        else if (strcmp(tokens[0], "echo") == 0) {
            int i;
            for (i = 1; i<count; i++) {
                if (tokens[i] != NULL)
                printf("%s ", tokens[i]);
            }
            printf("\n");
        }
        
        // exit
        else if (strcmp(tokens[0], "exit") == 0) {
            exit(1);
        }

        // env
        else if (strcmp(tokens[0], "env") == 0) {
            if (tokens[1] != NULL) {
                if (getenv(tokens[1]) != 0) printf("%s\n", getenv(tokens[1]));
                else printf("%s\n", tokens[1]);
            }
           
            else {
                extern char **environ;
                char **current;
                for(current = environ; *current; current++) {
                    puts(*current);
                }
            }
        }

       
        else if (strcmp(tokens[0], "setenv") == 0) {
            if (tokens[1] != NULL) {
                char dash[] = "=";
                char *temp = strtok(tokens[1], dash);
                char var[20], val[20];
                strcpy(var, temp);
                strcpy(val, strtok(NULL, dash));
                setenv(var, val, 1);
            }
        }
    
        // 3. Create a child process which will execute the command line input

        else {
            
            void killer(int signum) {
              if (getpid() == 0) kill(getpid(), SIGKILL);
            }
            void timer(int signum) {
              kill(getpid(), SIGKILL);
            }
            signal(SIGINT, killer);
            signal(SIGALRM, timer);
            
            int status;
            pid_t pid;
            int runinbackground = strcmp(last, "&") == 0;
            pid = fork();

            if (pid < 0) printf("Error: Fork Op\n");
            else if (pid == 0) {
                char cmd[10];
                strcpy(cmd, tokens[0]);
                if (runinbackground) {
                    tokens[lastindex] = NULL;
                }

                alarm(10);
                status = execvp(cmd, tokens);
                
                if (status == -1) {
                    printf("execvp() failed: No such file or directory\nAn error occurred.\n");
                    exit(1);
                }
            }

        // 4. The parent process should wait for the child to complete unless its a background process
            
            else if (!runinbackground) {
                wait(&status);
            }
        }
      
        // Hints (put these into Google):
        // man fork
        // man execvp
        // man wait
        // man strtok
        // man environ
        // man signals
        
        // Extra Credit
        // man dup2
        // man open
        // man pipes
    }
    // This should never be reached.
    return -1;
}

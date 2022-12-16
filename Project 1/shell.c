#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
//<Author> Julian Forbes @julianf17
// Collaborated with Kyndall Jones and Zoe Carter 
//References: https://www.geeksforgeeks.org/making-linux-shell-c/
// http://www.dmulholl.com/lets-build/a-command-line-shell.html
//https://www.cs.purdue.edu/homes/grr/SystemsProgrammingBook/Book/Chapter5-WritingYourOwnShell.pdf

#define MAX_COMMAND_LINE_LEN 1024
#define MAX_COMMAND_LINE_ARGS 128
char prompt[] = "> ";
char delimiters[] = " \t\r\n";
extern char **environ;
int cmd_pid = -1;

void codeExit(int code);
void exeCmd(char* arguments[]);
void quoteRemoval(char* token);
void processTime(int time, int pid);
void sig_handler(int signum);

int main() {
  int a;
  char command_line[MAX_COMMAND_LINE_LEN];
  char cmd_bak[MAX_COMMAND_LINE_LEN];
  size_t wd_size = 400;
  char wd[wd_size];
  char *wdp;
  char *arguments[MAX_COMMAND_LINE_ARGS];

  signal(SIGINT,sig_handler);

  while (true) {
    do { 
      fflush(stdout);
      wdp = getcwd(wd, wd_size);
      printf("%s %s", wdp, prompt);
      fflush(stdout);
      if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
        fprintf(stderr, "fgets error");
        exit(0);
      }

    } while(command_line[0] == 0x0A);
    if (feof(stdin)) {
      codeExit(0);
    }
    arguments[0] = strtok(command_line, delimiters);
    a=0;
    while(arguments[a] != NULL) {
      quoteRemoval(arguments[a]);
      a++;
      arguments[a]= strtok(NULL, delimiters);
    }
    
    char* last_token = arguments[a-1];
    bool run_in_background = false;
    if (strcmp(last_token,"&")==0){
      run_in_background = true;
      arguments[a-1] = NULL;
    }
    
    if (strcmp(arguments[0],"cd")==0){
      chdir(arguments[1]);
    } else if (strcmp(arguments[0],"pwd")==0){
      printf("%s\n",getcwd(wd, wd_size));
    } else if (strcmp(arguments[0],"echo")==0){
      a = 1;
      while(arguments[a] != NULL){
        if (arguments[a][0] == '$'){
          printf("%s ", getenv(arguments[a]+1));
        }
        else{
          printf("%s ", arguments[a]);
        }
        a++;
      }
      printf("\n");
    } else if (strcmp(arguments[0],"exit")==0){
      exit(0);
    } else if (strcmp(arguments[0],"env")==0){
      if (arguments[1] != NULL){
        printf("%s\n",getenv(arguments[1]));
      } else {
        char** env = environ;
        for (; *env; env++)
        printf("%s\n", *env);
      }
    }

    else if (strcmp(arguments[0],"setenv")==0) 
    {
      char* temp[2];
      temp[0] = strtok(arguments[1], "=");
      a = 0;
      while(temp[a] != NULL || a == 2) {
        a++;
        temp[a] = strtok(NULL, "=");
      }

      if (temp[0] == NULL || temp[1] == NULL)
      {
        codeExit(1);

      }
      setenv(temp[0], temp[1], 1);
    }
    // Executing CLI commands
    else {
      int pid = fork();
      if (pid == 0)
      {
        signal(SIGINT, SIG_DFL);
        exeCmd(arguments);
        exit(0);
      } else 
      {
        if (run_in_background)
        {
          cmd_pid = -1;
        } else {
          cmd_pid = pid;
          int kpid = fork();
          if (kpid == 0){
            signal(SIGINT, SIG_DFL);
            processTime(10000, pid);
            exit(0);
          } else 
          {
            waitpid(pid, NULL, 0);
            kill(kpid, SIGINT);
            waitpid(kpid, NULL, 0);
          }
        }
      } 
    }
  }
  return -1;
}

void processTime(int time, int pid) 
{
  sleep(time);
  printf("Foreground process timed out.\n");
  kill(pid, SIGINT);
}

void quoteRemoval(char* token) 
{
  bool quotes = false;
  char first;
  char last;
  if (token[0] == '\"' || token[0]=='\'')
  {
    first = token[0];
    quotes = true;
  }
  if (quotes){
    int a = 0;
    while (token[a]!='\0')
    {
      last = token[a];
      a++;
    }
    if (first == last) 
    { 
      a=1;
      while (token[a] != '\0') {
        token[a-1] = token[a];
        a++;
      }
      token[a - 1]='\0';
      token[a - 2]='\0';
    }
  }  
}

void exeCmd(char* arguments[]) {
  char* args_by_pipe[MAX_COMMAND_LINE_ARGS][MAX_COMMAND_LINE_ARGS];
  int a = 0;
  int cmds = 0;
  int cmdtokens = 0;
  while (arguments[a]!= NULL) {
    if (strcmp(arguments[a],"|")==0) {
      if (cmdtokens == 0) {
        printf("Invalid pipe command\n");
        return;
      }
      args_by_pipe[cmds][cmdtokens] = NULL;
      cmds++;
      cmdtokens = 0;
    } else {
      args_by_pipe[cmds][cmdtokens] = arguments[a];
      cmdtokens++;
    }
    a++;
  }
  int num_pipes = cmds;
  cmds++;
  if (num_pipes == 0) {
    execvp(args_by_pipe[0][0],args_by_pipe[0]);
  } else {
    int pipes[num_pipes][2];
    for (a = 0;a < num_pipes;a++) {
      pipe(pipes[a]);
    }
    int pids[cmds];
    for (a = 0;a < cmds;a++) {
      pids[a]=fork();
      if (pids[a] == 0) {
        int j;
        if (a == 0) {
          dup2(pipes[0][1], 1);
          close(pipes[0][0]);
          for (j=1; j < num_pipes; j++) {
            close(pipes[j][0]);
            close(pipes[j][1]);
          }
        } else if (a == num_pipes) {
          dup2(pipes[num_pipes-1][0], 0);
          close(pipes[num_pipes-1][1]);
          for (j=0; j < num_pipes-1; j++) {
            close(pipes[j][0]);
            close(pipes[j][1]);
          }
        } else {
          dup2(pipes[a-1][0],0);
          dup2(pipes[a][1],1);
          for (j=0; j < num_pipes; j++) {
            if (j== a-1) {
              close(pipes[j][1]);
            } else if (j == a) {
              close(pipes[j][0]);
            } else {
              close(pipes[j][0]);
              close(pipes[j][1]);
            }
          }
        }
        execvp(args_by_pipe[a][0],args_by_pipe[a]);
      } else if (pids[a] < 0) {
        printf("An error occured\n");
        return;
      }
    }
    for (a=0; a<num_pipes; a++){
      close(pipes[a][0]);
      close(pipes[a][1]);
    }
    for(a = 0; a < cmds; a++) {
      wait(NULL);
    }    
  }
}

void codeExit(int code) {
  printf("\n");
  fflush(stdout);
  fflush(stderr);
  exit(code);
}

void sig_handler(int signum) { 
  if (cmd_pid != -1) {
    kill(cmd_pid, SIGINT);
  }
}

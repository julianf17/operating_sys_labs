#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
// <Author> Julian Forbes @julianf17
// collaborated with Kyndall Jones and Zoe Carter  
/**
 * Executes the command "cat scores | grep Lakers".  In this quick-and-dirty
 * implementation the parent doesn't wait for the child to finish and
 * so the command prompt may reappear before the child terminates.
 *
 */

int main(int argc, char **argv)
{
  int pipefd[2];
  int pid;

  char *cat_args[] = {"cat", "scores", NULL};
  char *grep_args[] = {"grep", "Lakers", NULL};

  
  pipe(pipefd); // creates a pipe 
  pid = fork();

  if (pid == 0)
    {
      dup2(pipefd[0], 0); 
      close(pipefd[1]); // close part of pipe that is unused
      execvp("grep", grep_args); // execute grep
    }
  else
    {
      dup2(pipefd[1], 1); // standard output is replaced with output 
      close(pipefd[0]); // close unused unput half of pipe
      execvp("cat", cat_args); // execute cat
    }
}

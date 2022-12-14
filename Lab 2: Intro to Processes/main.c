#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#define   MAX_COUNT  2

void ChildProcess(int);
void ParentProcess(int, pid_t);

void main(void) {
    pid_t pid1, pid2;
    int status;
    srand(time(0));
    int random_num = rand()%30;
    pid1 = fork();
    
    if(pid1 < 0) printf("Error:Fork Op\n");
    
    else if(pid1 == 0) ChildProcess(random_num);
    
    else{
      pid2 = fork();
      if(pid2 < 0) printf("Error: Fork Op\n");
      else if(pid2 == 0) ChildProcess(random_num);
      else{
        wait(&status);
        ParentProcess(status, pid2);
      }
      wait(&status);
      ParentProcess(status, pid1);
    }
}

void ChildProcess(int random_num) {
    int i;
    pid_t pid = getpid();
    pid_t ppid = getppid();
    int duration;

    printf("Child Pid:<%d> is going to sleep!\n", pid);
    
    for(i = 1; i <= random_num; i++) {
      duration = rand() % 10;
      sleep(duration);
    }
    
    printf("Child Pid: is awake!\nWhere is my Parent: <%d>?\n", ppid);

    exit(0);
}

void ParentProcess(int stat, pid_t pid) {
    printf("Child Pid:<%d> has completed\n", pid);
}

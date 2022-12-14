/* hello_signal.c */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

bool signaled = true;
int alarms;
time_t start, ending;

void handler(int signum)//handles signaler
{ 
  printf("Hello World!\n");
  alarms++;
  signaled = false;
  alarm(1);
}

void handler2(int signum) {
  time(&ending);
  double elapsed = difftime(ending, start);
  printf("\nAlarms: %d, Duration: %fs\n", alarms, elapsed);
  exit(1);
}

int main(int argc, char * argv[])
{
  signal(SIGALRM, handler); //register; handles SIGALRM
  signal(SIGINT, handler2);
  alarm(1); //Schedule a 1 sec SIGALRM 

  time(&start);
  while (1) {
    signaled = true;
    while (signaled);
    printf("Turing was right!\n");
  }; //time for signal to get delivered
  
  return 0; 
}

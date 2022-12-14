#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
// <Author> Julian Forbes @julianf17
// collaborated with Kyndall Jones and Zoe Carter
void ChildProcess(int []);
void ParentProcess(int []);

int  main(int  argc, char *argv[])
{
  int ShmID, *ShmPTR, status;
  pid_t pid;
  ShmID = shmget(IPC_PRIVATE, 2*sizeof(int), IPC_CREAT | 0666);
  
  if (ShmID < 0) {
    printf("*** shmget error (server) ***\n");
    exit(1);
  }
  
  printf("Process has received a shared memory of two integers...\n");
  
  ShmPTR = (int *) shmat(ShmID, NULL, 0);
  
  if (*ShmPTR == -1) {
    printf("*** shmat error (server) ***\n");
    exit(1);
  }
  
  printf("Process has attached the shared memory...\n");

  ShmPTR[0] = 0; 
  ShmPTR[1] = 0; 

  printf("Original Bank Account = %d\n", ShmPTR[0]);

  pid = fork();
  int i;
  
  for (i = 0; i < 25; i++) {
    if (pid < 0) {
      printf("*** fork error (server) ***\n");
      exit(1);
    }
    else if (pid == 0) {
      ChildProcess(ShmPTR);
      exit(0);
    }
    ParentProcess(ShmPTR);
    pid = fork();
  }
}

void ParentProcess(int SharedMem[]) {
  sleep(rand()%6);
  while (SharedMem[1] != 0);
  int account = SharedMem[0];
  if (account <= 100) {
    int balance = rand()%101;
    if (balance % 2 == 0) {
      account += balance;
      printf("Dear old Dad: Deposits $%d / Balance = $%d\n", balance, account);
    }
    else printf("Dear old Dad: Doesn't have money to give\n");
  }
  else printf("Dear old Dad: Thinks student has enough cash ($%d)\n", account);
  
  SharedMem[0] = account;
  SharedMem[1] = 1;
}

void ChildProcess(int SharedMem[]) {
  sleep(rand()%6);
  
  while (SharedMem[1] != 1);
  int account = SharedMem[0];
  int balance = rand()%51;
  printf("Poor Student needs $%d\n", balance);
  
  if (balance <= account) {
    account -= balance;
    printf("Poor Student: Withdraws $%d / Balance = $%d\n", balance, account);
  }
  
  else printf("Poor Student: Not Enough Cash ($%d)\n", account );

  SharedMem[0] = account;
  SharedMem[1] = 0;
}

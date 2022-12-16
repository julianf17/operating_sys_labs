#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

// <Author> Julian Forbes @julianf17
// collaborated with Kyndall Jones and Zoe Carter
//references: https://leonardoaraujosantos.gitbook.io/opencl/matrix_multiply_problem
//https://www.tutorialspoint.com/parallel_algorithm/matrix_multiplication.htm
int MAX;

int main() {
  srand(time(0));  
  char input[3];
  printf("Enter the size of matrix you want to initialize: ");
  fgets(input, 3, stdin);
  MAX = atoi(input);
  
  int matA[MAX][MAX]; 
  int matB[MAX][MAX]; 
  int matSumResult[MAX][MAX];
  int matDiffResult[MAX][MAX]; 
  int matProductResult[MAX][MAX]; 

  struct args_t {
    int (*matA)[MAX];
    int (*matB)[MAX];
    int (*res)[MAX];
    int row;
  };

  void fillMatrix(int matrix[MAX][MAX]) {
      for(int i = 0; i<MAX; i++) {
          for(int j = 0; j<MAX; j++) {
              matrix[i][j] = rand()%10+1;
          }
      }
  }

  void printMatrix(int matrix[MAX][MAX]) {
      for(int i = 0; i<MAX; i++) {
          for(int j = 0; j<MAX; j++) {
              printf("%5d", matrix[i][j]);
          }
          printf("\n");
      }
      printf("\n");
  }

  
  void* computeSum(void* args) { 
    struct args_t* arg = (struct args_t*) args;
    int offset = arg->row * MAX;
    for (int i = 0; i < MAX; i++) {
      int added = arg->matA[0][i+offset] + arg->matB[0][i+offset];
      arg->res[0][i+offset] = added;
    }
  }


  void* computeDiff(void* args) { 
    struct args_t* arg = (struct args_t*) args;
    int offset = arg->row * MAX;
    for (int i = 0; i < MAX; i++) {
      int added = arg->matA[0][i+offset] - arg->matB[0][i+offset];
      arg->res[0][i+offset] = added;
    }
  }

  void* computeProduct(void* args) { 
    struct args_t* arg = (struct args_t*) args;
    int offset = arg->row * MAX;
    for (int i = 0; i < MAX; i++) {
      int added = arg->matA[0][i+offset] * arg->matB[0][i+offset];
      arg->res[0][i+offset] = added;
    }
  }

  fillMatrix(matA);
  fillMatrix(matB);

  printf("Matrix A:\n");
  printMatrix(matA);
  printf("Matrix B:\n");
  printMatrix(matB);

  pthread_t threads[MAX];
  struct args_t args[MAX];
 
  for (int i=0; i<MAX; i++) {
    args[i].matA = matA;
    args[i].matB = matB;
    args[i].res = matSumResult;
    args[i].row = i;
    int error = pthread_create(&threads[i], NULL, &computeSum, (void *) &args[i]);
    if (error) {
      printf("Thread creation failed!");
      exit(1);
    }
  }
  
  
  for (int i=0; i<MAX; i++) {
    pthread_join(threads[i], NULL);
  }
  
  
  for (int i=0; i<MAX; i++) {
    args[i].matA = matA;
    args[i].matB = matB;
    args[i].res = matDiffResult;
    args[i].row = i;
    int error = pthread_create(&threads[i], NULL, &computeDiff, (void *) &args[i]);
    if (error) {
      printf("Thread creation failedd!");
      exit(1);
    }
  }
  
  // waiting for the threads
  for (int i=0; i<MAX; i++) {
    pthread_join(threads[i], NULL);
  }
  
  // Product
  for (int i=0; i<MAX; i++) {
    args[i].matA = matA;
    args[i].matB = matB;
    args[i].res = matProductResult;
    args[i].row = i;
    int error = pthread_create(&threads[i], NULL, &computeProduct, (void *) &args[i]);
    if (error) {
      printf("Thread creation failed!");
      exit(1);
    }
  }
  
  for (int i=0; i<MAX; i++) {
    pthread_join(threads[i], NULL);
  }

  printf("A + B:\n");
  printMatrix(matSumResult);
  printf("A - B:\n");
  printMatrix(matDiffResult);
  printf("A * B:\n");
  printMatrix(matProductResult);
  return 0;
}

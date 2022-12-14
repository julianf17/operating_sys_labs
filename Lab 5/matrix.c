#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX 2
// <Author> Julian Forbes @julianf17
// collaborated with Kyndall Jones and Zoe Carter
int matA[MAX][MAX]; 
int matB[MAX][MAX]; 
int matSumResult[MAX][MAX];
int matDiffResult[MAX][MAX]; 
int matProductResult[MAX][MAX]; 

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

// Fetches the appropriate coordinates from the argument, and sets
// the cell of matSumResult at the coordinates to the sum of the
// values at the coordinates of matA and matB.
void* computeSum(void * args) { // pass in the number of the ith thread
  int * data = (int *)args;
  int k = 0;
  for (int i=0; i < MAX; i++) {
    for (int j=0; j < MAX; j++) {
      matSumResult[i][j] = data[k] + data[k+MAX+2];
      k++;
    }
  }
  return NULL;
}

// Fetches the appropriate coordinates from the argument, and sets
// the cell of matSumResult at the coordinates to the difference of the
// values at the coordinates of matA and matB.
void* computeDiff(void* args) { // pass in the number of the ith thread
  int * data = (int *)args;
  int k = 0;
  for (int i = 0; i < MAX; i++) {
    for (int j = 0; j < MAX; j++) {
      matDiffResult[i][j] = data[k] - data[k+MAX+2];
      k++;
    }
  }
  return NULL;
}

// Fetches the appropriate coordinates from the argument, and sets
// the cell of matSumResult at the coordinates to the inner product
// of matA and matB.
void* computeProduct(void* args) { // pass in the number of the ith thread
  int * data = (int *)args;
  for (int i = 0; i < MAX; i++) {
    for (int j = 0; j < MAX; j++) {
      for (int k = 0; k < MAX; k++) {
        matProductResult[i][j] += matA[i][k] * matB[k][j];
      }
    }
  }
  return NULL;
}

// Spawn a thread to fill each cell in each result matrix.
// How many threads will you spawn?
int main() {
    srand(time(0));  // Do Not Remove. Just ignore and continue below.
    
    // 0. Get the matrix size from the command line and assign it to MAX
    printf("Enter matrix size: ");
    // scanf("%d", &MAX);
    
    // 1. Fill the matrices (matA and matB) with random values.
    fillMatrix(matA);
    fillMatrix(matB);
    
    // 2. Print the initial matrices.
    printf("Matrix A:\n");
    printMatrix(matA);
    printf("Matrix B:\n");
    printMatrix(matB);
    
    // 3. Create pthread_t objects for our threads.
    pthread_t * threads;
    threads = (pthread_t*)malloc(MAX*sizeof(pthread_t));

    // 4. Create a thread for each cell of each matrix operation.
    // 
    // You'll need to pass in the coordinates of the cell you want the thread
    // to compute.
    // 
    // One way to do this is to malloc memory for the thread number i, populate the coordinates
    // into that space, and pass that address to the thread. The thread will use that number to calcuate 
    // its portion of the matrix. The thread will then have to free that space when it's done with what's in that memory.
    
    int* data = NULL;
    data = (int *)malloc((20)*sizeof(int));
    int i = 0;
    for (int j = 0; j < MAX; j++) {
      for (int k = 0; k < MAX; k++) {
        data[i] = matA[j][k];
        data[i+MAX+2] = matB[j][k];
        i++;
      }
    }

    pthread_create(&threads[0], NULL, computeSum, (void *) data);
    pthread_create(&threads[1], NULL, computeDiff, (void *) data);
    pthread_create(&threads[2], NULL, computeProduct, (void *) data);
    
    // 5. Wait for all threads to finish.
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    pthread_join(threads[2], NULL);

    // 6. Print the results.
    printf("Results:\n");
    printf("Sum:\n");
    printMatrix(matSumResult);
    printf("Difference:\n");
    printMatrix(matDiffResult);
    printf("Product:\n");
    printMatrix(matProductResult);
    return 0;
  
}

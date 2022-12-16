#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "BENSCHILLIBOWL.h"
//<Author> Julian Forbes @julianf17
// collaborated with Kyndall Jones

// Feel free to play with these numbers! This is a great way to
// test your implementation.
#define BENSCHILLIBOWL_SIZE 100
#define NUM_CUSTOMERS 90
#define NUM_COOKS 10
#define ORDERS_PER_CUSTOMER 3
#define EXPECTED_NUM_ORDERS NUM_CUSTOMERS * ORDERS_PER_CUSTOMER

BENSCHILLIBOWL *bcb;


void* BENSCHILLIBOWLCustomer(void* tid) {
  int cust_id = *((int*) tid);
  
  for (int i=0; i<ORDERS_PER_CUSTOMER; i++) {
    Order* order = (Order*) malloc(sizeof(Order));
    order->menu_item = PickRandomMenuItem();
    order->customer_id = cust_id;
    order->next = NULL;
    AddOrder(bcb, order);
  }
}

void* BENSCHILLIBOWLCook(void* tid) {
  int cook_id = *((int*) tid);
	int orders_fulfilled = 0;
  
  while (bcb->orders_handled != bcb->expected_num_orders) {
    Order* order = GetOrder(bcb);
    if (order) {
      orders_fulfilled++;
      printf("Cook #%d fulfilled Customer %d's order\n", cook_id, order->customer_id);
      free(order);
    }
  }
	printf("Cook #%d fulfilled %d orders\n", cook_id, orders_fulfilled);
}

int main() {
  bcb = OpenRestaurant(BENSCHILLIBOWL_SIZE, EXPECTED_NUM_ORDERS);
  
  pthread_t customer[NUM_CUSTOMERS];
  int cust_id[NUM_CUSTOMERS];
  pthread_t cook[NUM_COOKS];
  int cook_id[NUM_COOKS];
  
  
  for (int i=0; i<NUM_COOKS; i++) {
    cook_id[i] = i+1;
    pthread_create(&cook[i], NULL, &BENSCHILLIBOWLCook, (void*) &(cook_id[i]));
  }
  for (int i=0; i<NUM_CUSTOMERS; i++) {
    cust_id[i] = i+1;
    pthread_create(&customer[i], NULL, &BENSCHILLIBOWLCustomer, (void*) &(cust_id[i]));
  }
  for (int i=0; i<NUM_CUSTOMERS; i++) {
    pthread_join(customer[i], NULL);
  }
  for (int i=0; i<NUM_COOKS; i++) {
    pthread_join(cook[i], NULL);
  }
  CloseRestaurant(bcb);
  return 0;
}

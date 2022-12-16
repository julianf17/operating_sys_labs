#include "BENSCHILLIBOWL.h"
#include <assert.h>
#include <stdlib.h>
#include <sys/time.h>
#include <errno.h>
//<Author> Julian Forbes @julianf17
// collaborated with Zoe Carter and Kyndall Jones

bool IsEmpty(BENSCHILLIBOWL* bcb);
bool IsFull(BENSCHILLIBOWL* bcb);

void AddOrderToBack(Order **orders, Order *order);
void recursive(Order* order);

MenuItem BENSCHILLIBOWLMenu[] = { 
    "BensChilli", 
    "BensHalfSmoke", 
    "BensHotDog", 
    "BensChilliCheeseFries", 
    "BensShake",
    "BensHotCakes",
    "BensCake",
    "BensHamburger",
    "BensVeggieBurger",
    "BensOnionRings",
};
int BENSCHILLIBOWLMenuLength = 10;

/* Select a random item from the Menu and return it */
MenuItem PickRandomMenuItem() {
  int idx = rand() % BENSCHILLIBOWLMenuLength;
  return BENSCHILLIBOWLMenu[idx];
}

/* Allocate memory for the restaurant, then create the mutex and condition variables needed to instantiate the Restaurant */

BENSCHILLIBOWL* OpenRestaurant(int max_size, int expected_num_orders) {
  printf("Restaurant is open!\n");
  
  BENSCHILLIBOWL* bcb = (BENSCHILLIBOWL*) malloc(sizeof(BENSCHILLIBOWL));
  bcb->orders = NULL;
  bcb->max_size = max_size;
  bcb->current_size = 0;
  bcb->next_order_number = 1;
  bcb->orders_handled = 0;
	bcb->expected_num_orders = expected_num_orders;
  
  if (pthread_mutex_init(&bcb->mutex, NULL)) {
    perror("Mutex Creation Failed!");
    exit(1);
  }
  if (pthread_cond_init(&bcb->can_add_orders, NULL)) {
    perror("Creation of Var Failed!");
    exit(1);
  } 
  if (pthread_cond_init(&bcb->can_get_orders, NULL)) {
    perror("Creatoin of Var Failed!");
    exit(1);
  }
  
  return bcb;
}

void recursive(Order* order) {
  if (order) {
    recursive(order->next);
    free(order);
  }
}

void CloseRestaurant(BENSCHILLIBOWL* bcb) {
  printf("Restaurant is closed\n");
  recursive(bcb->orders);
  pthread_mutex_destroy(&bcb->mutex);
  pthread_cond_destroy(&bcb->can_add_orders);
  pthread_cond_destroy(&bcb->can_get_orders);
  free(bcb);
}

bool IsFull(BENSCHILLIBOWL* bcb) {
  return bcb->current_size == bcb->max_size;
}

void AddOrderToBack(Order **orders, Order *order) {
  Order *curr = *orders;
  if (!curr) {
    *orders = order;
    return;
  }
  while (curr->next) {
    curr = curr->next;
  }
  curr->next = order;
  order->next = NULL;
}

int AddOrder(BENSCHILLIBOWL* bcb, Order* order) {
  pthread_mutex_lock(&bcb->mutex);
  int order_num = bcb->next_order_number;
  if (!IsFull(bcb)) {
    order->order_number = bcb->next_order_number;
    AddOrderToBack(&bcb->orders, order);
    bcb->next_order_number++;
    bcb->current_size++;
  } else {
    while (IsFull(bcb)) {
      pthread_cond_wait(&bcb->can_add_orders, &bcb->mutex);
    }
    order->order_number = bcb->next_order_number;
    AddOrderToBack(&bcb->orders, order);
    bcb->next_order_number++;
    bcb->current_size++;
  }
  pthread_cond_signal(&bcb->can_get_orders);
  pthread_mutex_unlock(&bcb->mutex);
  return order_num;
}

bool IsEmpty(BENSCHILLIBOWL* bcb) {
  return bcb->current_size == 0;
}

Order *GetOrder(BENSCHILLIBOWL* bcb) {
  pthread_mutex_lock(&bcb->mutex);
  Order* order;
  struct timespec wait_time;
  struct timeval curr_time;
  
  if (!IsEmpty(bcb)) {
    order = bcb->orders;
    bcb->orders = bcb->orders->next;
    order->next = NULL;
    bcb->current_size--;
    bcb->orders_handled++;
  } else {
    while (IsEmpty(bcb)) {
      pthread_cond_wait(&bcb->can_get_orders, &bcb->mutex);

    }
    order = bcb->orders;
    bcb->orders = bcb->orders->next;
    order->next = NULL;
    bcb->current_size--;
    bcb->orders_handled++;
  }
  pthread_cond_signal(&bcb->can_add_orders);
  pthread_mutex_unlock(&bcb->mutex);
  return order;
}

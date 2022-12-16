// list/list.c
// 
// Implementation for linked list.
//<Author> Julian Forbes @julianf17
// collaborated with Zoe Carter and Kyndall Jones
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

list_t *list_alloc() { 
  
  list_t* list = (list_t*)malloc(sizeof(list_t));
  list->head = NULL;
  
  return list; 
}

node_t *node_alloc(block_t *blk) {   
  node_t* node = malloc(sizeof(node_t));
  
  node->next = NULL;
  node->blk = blk;
  
  return node; 
}

void list_free(list_t *l){
  free(l);
  return;
}

void node_free(node_t *node){
  free(node);
  return;
}

void list_print(list_t *l) {
  node_t *curr = l->head;
  block_t *b;
  
  if (curr == NULL){
    printf("list is empty\n");
  }
  while (curr != NULL){
    b = curr->blk;
    printf("PID=%d START:%d END:%d", b->pid, b->start, b->end);
    curr = curr->next;
  }
}

int list_length(list_t *l) { 
  node_t *curr = l->head;
  int i = 0;
 
  while (curr != NULL){
    i++;
    curr = curr->next;
  }
  
  return i; 
}

void list_add_to_back(list_t *l, block_t *blk){  
  
  node_t* newNode = node_alloc(blk);
  newNode->next = NULL;
 
  if(l->head == NULL){
    l->head = newNode;
  }
  else{
    node_t *curr = l->head;
    while(curr->next != NULL){
      curr = curr->next;
    }
    curr->next = newNode;
  }
}

void list_add_to_front(list_t *l, block_t *blk){  
  node_t* newNode = node_alloc(blk);
  newNode->next = l->head;
  l->head = newNode;
}

void list_add_at_index(list_t *l, block_t *blk, int index){
  int i = 0;
  
  node_t *newNode = node_alloc(blk);
  node_t *curr = l->head;

  if(index == 0){
    newNode->next = l->head->next;
    l->head = newNode;
  }
  else if(index > 0){
    while(i < index && curr->next != NULL){
      curr = curr->next;
      i++;
    }
  newNode->next = curr->next;
  curr->next = newNode;
  }
}

void list_add_ascending_by_address(list_t *l, block_t *newblk){
  
  node_t* curr;
  node_t* prev;
  node_t* new_node = node_alloc(newblk);
  
  if (!l->head) {
    l->head = new_node;
    return;
  }
  curr = prev = l->head;
  
  if (!curr->next) {
    if (new_node->blk->start <= curr->blk->start) {
      new_node->next = l->head;
      l->head = new_node;
    } else {
      new_node->next = NULL;
      curr->next = new_node;
    }
  } else {
    if (new_node->blk->start <= curr->blk->start) {
      new_node->next = l->head;
      l->head = new_node;
    } else {
      while(curr && new_node->blk->start > curr->blk->start) {
        prev = curr;
        curr = curr->next;
      }
      prev->next = new_node;
      new_node->next = curr;
    }
  }
}


void list_add_ascending_by_blocksize(list_t *l, block_t *newblk){

  node_t* curr;
  node_t* prev;
  node_t* new_node = node_alloc(newblk);
  
  int newblk_size = new_node->blk->end - new_node->blk->start;
  int currblk_size;
  
  if (!l->head) {
    l->head = new_node;
    return;
  }
  curr = prev = l->head;
  currblk_size = curr->blk->end - curr->blk->start + 1;
  
  if (!curr->next) {
    if (newblk_size <= currblk_size) {
      new_node->next = l->head;
      l->head = new_node;
    } else {
      new_node->next = NULL;
      curr->next = new_node;
    }
  } else {
    if (newblk_size <= currblk_size) {
      new_node->next = l->head;
      l->head = new_node;
    } else {
      while(curr && newblk_size > currblk_size) {
        prev = curr;
        curr = curr->next;
        if (curr) {
          currblk_size = curr->blk->end - curr->blk->start;
        }
      }
      prev->next = new_node;
      new_node->next = curr;
    }
  }
}


void list_add_descending_by_blocksize(list_t *l, block_t *blk){
  
  node_t *curr;
  node_t *prev;
  node_t *newNode = node_alloc(blk);
  
  int newblk_size = blk->end - blk->start;
  int curblk_size;
  
  if(l->head == NULL){
    l->head = newNode;
  }
  else{
    prev = curr = l->head;
    
    curblk_size = curr->blk->end - curr->blk->start + 1;
    
    if(curr->next == NULL) {  //only one node in list
       if(newblk_size >= curblk_size) {  // place in front of curr node
          newNode->next = l->head;
          l->head = newNode;
       }
       else {   // place behind curr node
          curr->next = newNode;
          newNode->next = NULL;
       }
    }
    else {  // two or more nodes in list
      
       if(newblk_size >= curblk_size) {  // place in front of curr node
          newNode->next = l->head;
          l->head = newNode;
       }
       else {
      
          while(curr != NULL && newblk_size <= curblk_size) {
               prev = curr;
               curr = curr->next;
               
               if(curr != NULL)  // the last one in the list
                     curblk_size = curr->blk->end - curr->blk->start;
          }
          prev->next = newNode;
          newNode->next = curr;
       }
    }
  }
}

void list_coalese_nodes(list_t *l){ 

  node_t* curr = l->head;
	node_t* prev = curr;
	
	if (curr->next) {
		curr = curr->next;
	} else {
		return;
	}
	
	while (curr) {
		if (prev->blk->end + 1 == curr->blk->start) {
			prev->blk->end = curr->blk->end;
			prev->next = curr->next;
			node_free(curr);
			curr = prev->next;
		} else {
			prev = curr;
			curr = curr->next;
		}
	}
}


block_t* list_remove_from_back(list_t *l){
  
  block_t *value = NULL;
  node_t *curr = l->head;

  if(l->head != NULL){
    
    if(curr->next == NULL) { // one node
         l->head->next = NULL;
         value = curr->blk;
         node_free(curr);
    }
    else {
         while (curr->next->next != NULL){
            curr = curr->next;
         }
         value = curr->blk;
         node_free(curr->next);
         curr->next = NULL;
    }
  }
  return value;
}

block_t* list_get_from_front(list_t *l) {
  block_t *value = NULL;
  if(l->head == NULL){
    return value;
  }
  else{
    node_t *curr = l->head;
    value = curr->blk;
  }
  return value; 
}


block_t* list_remove_from_front(list_t *l) { 
  block_t *value = NULL;
  if(l->head == NULL){
    return value;
  }
  else{
    node_t *curr = l->head;
    value = curr->blk;
    l->head = l->head->next;
    node_free(curr);
  }
  return value; 
}

block_t* list_remove_at_index(list_t *l, int index) { 
  int i;
  block_t* value = NULL;
  bool found = false;
  
  if(l->head == NULL){
    return value;
  }
  else if (index == 0){
    return list_remove_from_front(l);
  }
  else if (index > 0){
    node_t *curr = l->head;
    node_t *prev = curr;
    
    i = 0;
    while(curr != NULL && !found){
      if(i == index)
          found = true;
      else {
         prev = curr;
         curr = curr->next;
         i++;
      }
    }
    
    if(found) {
      value = curr->blk; 
      prev->next = curr->next;
      node_free(curr);
    }
  }
  return value; 
}

bool compareBlks(block_t* a, block_t *b) {
  
  if(a->pid == b->pid && a->start == b->start && a->end == b->end)
     return true;
  
  return false;
}

bool compareSize(int a, block_t *b) {  // greater or equal
  
  if(a <= (b->end - b->start) + 1)
     return true;
  
  return false;
}

bool comparePid(int a, block_t *b) {
  
  if(a == b->pid)
     return true;
  
  return false;
}


bool list_is_in(list_t *l, block_t* value) { 
  node_t *curr = l->head;
  while(curr != NULL){
    if(compareBlks(value, curr->blk)){
      return true;
    }
    curr = curr->next;
  }
return false; 
}

block_t* list_get_elem_at(list_t *l, int index) { 
 int i;
  block_t* value = NULL;
  if(l->head == NULL){
    return value;
  }
  else if (index == 0){
    return list_get_from_front(l);
  }
  else if (index > 0){
    node_t *curr = l->head;
    
    i = 0;
    while(curr != NULL){
      if(i == index)
          return(curr->blk);
      else {
         curr = curr->next;
         i++;
      }
    }
  }
  return value; 
}

int list_get_index_of(list_t *l, block_t* value){
 int i = 0;
 node_t *curr = l->head;
 if(l->head == NULL){
    return -1;
  }
  
  while (curr != NULL){
   if (compareBlks(value,curr->blk)){
     return i;
    }
    curr = curr->next;
    i++;
  }
  return -1; 
}

/* Checks to see if block of Size or greater exists in the list. */
bool list_is_in_by_size(list_t *l, int Size){ 
  node_t *curr = l->head;
  while(curr != NULL){
    if(compareSize(Size, curr->blk)){
      return true;
    }
    curr = curr->next;
  }
return false; 
}

/* Checks to see if pid of block exists in the list. */
bool list_is_in_by_pid(list_t *l, int pid){ 
  
  /* Iterate through the list to find a node with a blk that has blk->pid = pid
   * 
   * USE the comparePID()
   * 
   * Look at list_is_in_by_size() */
   node_t* curr = l->head;
	while (curr) {
		if (comparePid(pid, curr->blk)) {
			return true;
		}
		curr = curr->next;
	}
	return false;
}


/* Returns the index at which the given block of Size or greater appears. */
int list_get_index_of_by_Size(list_t *l, int Size){
 int i = 0;
 node_t *curr = l->head;
 
  if(l->head == NULL){
    return -1;
  }
  
  while (curr != NULL){
   if (compareSize(Size,curr->blk)){
     return i;
    }
    curr = curr->next;
    i++;
  }

  return -1; 
}
                   
/* Returns the index at which the pid appears. */
int list_get_index_of_by_Pid(list_t *l, int pid){
 int i = 0;
 node_t *curr = l->head;
 if(l->head == NULL){
    return -1;
  }
  
  while (curr != NULL){
   if (comparePid(pid,curr->blk)){
     return i;
    }
    curr = curr->next;
    i++;
  }

  return -1; 
}

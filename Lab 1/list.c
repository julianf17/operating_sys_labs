// list/list.c
//
// Implementation for linked list.
//
// <Author> Julian Forbes @julianf17
#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include <stddef.h>

list_t * list_alloc() {
    list_t * mylist;
    node_t * head = NULL;
    head = (node_t *)malloc(sizeof(node_t));
    head -> value = 0;
    head -> next = NULL;
    mylist -> head = head;
    return mylist;
};

void list_free(list_t * l) {
    free(l -> head);
};

void list_print(list_t * l) {
    node_t * current = l -> head;
    while(current != NULL) {
        printf("%d\n", current -> value);
        current = current -> next;
    }
};

int list_length(list_t * l) {
    node_t * current = l -> head;
    int count = 0;
    while(current != NULL) {
        count++;
        current = current -> next;
    }
    return count;
};

void list_add_to_back(list_t * l, elem value) {
    node_t * current = l -> head;
    if (l -> head == NULL) {
        current -> value = value;
        current -> next = NULL;
    }
    else {
        while(current -> next != NULL) {
            current = current -> next;
        }
        current -> next = (node_t *)malloc(sizeof(node_t));
        current -> next -> value = value;
        current -> next -> next = NULL;
    }
};

void list_add_to_front(list_t * l, elem value) {
    node_t * latest_node;
    latest_node = (node_t *)malloc(sizeof(node_t));
    latest_node -> value = value;
    latest_node -> next = l -> head;
    l -> head = latest_node;
};

void list_add_at_index(list_t *l, elem value, int index) {
  if (l -> head == NULL) {
    l -> head -> value = value;
    l -> head -> next = NULL;
  }
  else {
    node_t * latest_node;
    latest_node = (node_t *)malloc(sizeof(node_t));
    latest_node -> value = value;

    int current_index = 0;
    node_t * current = l->head;
    while(current_index < index - 1) {
        if(current -> next == NULL) {
            current -> next = latest_node;
            latest_node -> next = NULL;
            return ;
        }
        current = current -> next;
        current_index++;
    }
    latest_node -> next = current -> next;
    current -> next = latest_node;
    }
};

elem list_remove_from_back(list_t * l) {
    node_t * current = l->head;
    while(current -> next -> next != NULL) {
        current = current -> next;
    }
    int res = current -> next -> value;
    free(current -> next);
    current -> next = NULL;
    return res;
};

elem list_remove_from_front(list_t * l) {
    int retval = -1;
    node_t * next_node = NULL;
    next_node = l -> head -> next;
    retval = l -> head -> value;
    free(l -> head);
    l -> head = next_node;
    return retval;
};

elem list_remove_at_index(list_t * l, int index) {
    int retval = -1;
    node_t * next_node = NULL;
    node_t * current = l->head;
    int current_index = 0;
    while(current_index < index-1) {
        current_index++;
        current = current -> next;
    }
    retval = current -> next -> value;
    next_node = current -> next -> next;
    free(current -> next);
    current -> next = next_node;
    return retval;
};

bool list_is_in(list_t * l, elem value) {
    node_t * current = l -> head;
    while(current -> next != NULL) {
        if(current -> value == value) 
        return true;
        current = current -> next;
    }
    return false;
};

elem list_get_elem_at(list_t * l, int index) {
    node_t * current = l -> head;
    int current_index = 0;
    while(current != NULL) {
        if(current_index == index) return current -> value;
        current = current -> next;
        current_index++;
    }
    return -1;
};

int list_get_index_of(list_t * l, elem value) {
    node_t * current = l -> head;
    int current_index = 0;
    while(current != NULL) {
        if(current->value == value) return current_index;
        current = current -> next;
        current_index++;
    }
    return -1;
};

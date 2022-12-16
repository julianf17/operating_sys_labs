#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "list.h"
#include "util.h"
//<Author> Julian Forbes @julianf17
// collaborated with Zoe Carter and Kyndall Jones
void TOUPPER(char * arr){
  
    for(int i=0;i<strlen(arr);i++){
        arr[i] = toupper(arr[i]);
    }
}

void get_input(char *args[], int input[][2], int *n, int *size, int *policy) 
{
  	FILE *input_file = fopen(args[1], "r");
	  if (!input_file) {
		    fprintf(stderr, "Error: Invalid filepath\n");
		    fflush(stdout);
		    exit(0);
	  }

    parse_file(input_file, input, n, size);
  
    fclose(input_file);
  
    TOUPPER(args[2]);
  
    if((strcmp(args[2],"-F") == 0) || (strcmp(args[2],"-FIFO") == 0))
        *policy = 1;
    else if((strcmp(args[2],"-B") == 0) || (strcmp(args[2],"-BESTFIT") == 0))
        *policy = 2;
    else if((strcmp(args[2],"-W") == 0) || (strcmp(args[2],"-WORSTFIT") == 0))
        *policy = 3;
    else {
       printf("usage: ./mmu <input file> -{F | B | W }  \n(F=FIFO | B=BESTFIT | W-WORSTFIT)\n");
       exit(1);
    }
        
}

void allocate_memory(list_t * freelist, list_t * alloclist, int pid, int blocksize, int policy) {
    node_t* curr = freelist->head;
	node_t* prev = curr;
	int blk_size;
	
	while (curr) {
		blk_size = curr->blk->end - curr->blk->start;
		if (blk_size >= blocksize) {
			if (prev == curr) {
				if (!curr->next) {
					freelist->head = NULL;
				} else {
					freelist->head = curr->next;
				}
			} else {
				prev->next = curr->next;
			}
			break;
		}
		prev = curr;
		curr = curr->next;
	}
	if (!curr) {
		printf("Error: Memory Allocation %d blocks\n", blocksize);
		return;
	}
	curr->next = NULL;
	curr->blk->pid = pid;
	int org_end = curr->blk->end;
	curr->blk->end = curr->blk->start + blocksize - 1;
	list_add_ascending_by_address(alloclist, curr->blk);
	block_t* frgmnt = (block_t *) malloc(sizeof(block_t));
	frgmnt->pid = 0;
	frgmnt->start = curr->blk->end + 1;
	frgmnt->end = org_end;
	
	if (policy == 1) {
		list_add_to_back(freelist, frgmnt);
	} else if (policy == 2) {
		list_add_ascending_by_blocksize(freelist, frgmnt);
	} else if (policy == 3) {
		list_add_descending_by_blocksize(freelist, frgmnt);
	}
}


void deallocate_memory(list_t * alloclist, list_t * freelist, int pid, int policy) { 
    node_t* curr = alloclist->head;
	node_t* prev = curr;
	
	while (curr) {
		if (curr->blk->pid == pid) {
			if (prev == curr) {
				if (!curr->next) {
					alloclist->head = NULL;
				} else {
					alloclist->head = curr->next;
				}
			} else {
				prev->next = curr->next;
			}
			break;
		}
		prev = curr;
		curr = curr->next;
	}
	if (!curr) {
		printf("Error: Can't locate Memory Used by PID: %d\n", pid);
		return;
	}
	
	curr->blk->pid = 0;
	curr->next = NULL;
	if (policy == 1) {
		list_add_to_back(freelist, curr->blk);
	} else if (policy == 2) {
		list_add_ascending_by_blocksize(freelist, curr->blk);
	} else if (policy == 3) {
		list_add_descending_by_blocksize(freelist, curr->blk);
	}
}
    


list_t* coalese_memory(list_t * list){
  list_t *temp_list = list_alloc();
  block_t *blk;
  
  while((blk = list_remove_from_front(list)) != NULL) {  // sort the list in ascending order by address
        list_add_ascending_by_address(temp_list, blk);
  }
  
  // try to combine physically adjacent blocks
  
  list_coalese_nodes(temp_list);
        
  return temp_list;
}

void print_list(list_t * list, char * message){
    node_t *current = list->head;
    block_t *blk;
    int i = 0;
  
    printf("%s:\n", message);
  
    while(current != NULL){
        blk = current->blk;
        printf("Block %d:\t START: %d\t END: %d", i, blk->start, blk->end);
      
        if(blk->pid != 0)
            printf("\t PID: %d\n", blk->pid);
        else  
            printf("\n");
      
        current = current->next;
        i += 1;
    }
}

/* DO NOT MODIFY */
int main(int argc, char *argv[]) 
{
   int PARTITION_SIZE, inputdata[200][2], N = 0, Memory_Mgt_Policy;
  
   list_t *FREE_LIST = list_alloc();   // list that holds all free blocks (PID is always zero)
   list_t *ALLOC_LIST = list_alloc();  // list that holds all allocated blocks
   int i;
  
   if(argc != 3) {
       printf("usage: ./mmu <input file> -{F | B | W }  \n(F=FIFO | B=BESTFIT | W-WORSTFIT)\n");
       exit(1);
   }
  
   get_input(argv, inputdata, &N, &PARTITION_SIZE, &Memory_Mgt_Policy);
  
   // Allocated the initial partition of size PARTITION_SIZE
   
   block_t * partition = malloc(sizeof(block_t));   
   partition->start = 0;
   partition->end = PARTITION_SIZE + partition->start - 1;
                                   
   list_add_to_front(FREE_LIST, partition);          
                                   
   for(i = 0; i < N; i++) 
   {
       printf("************************\n");
       if(inputdata[i][0] != -99999 && inputdata[i][0] > 0) {
             printf("ALLOCATE: %d FROM PID: %d\n", inputdata[i][1], inputdata[i][0]);
             allocate_memory(FREE_LIST, ALLOC_LIST, inputdata[i][0], inputdata[i][1], Memory_Mgt_Policy);
       }
       else if (inputdata[i][0] != -99999 && inputdata[i][0] < 0) {
             printf("DEALLOCATE MEM: PID %d\n", abs(inputdata[i][0]));
             deallocate_memory(ALLOC_LIST, FREE_LIST, abs(inputdata[i][0]), Memory_Mgt_Policy);
       }
       else {
             printf("COALESCE/COMPACT\n");
             FREE_LIST = coalese_memory(FREE_LIST);
       }   
     
       printf("************************\n");
       print_list(FREE_LIST, "Free Memory");
       print_list(ALLOC_LIST,"\nAllocated Memory");
       printf("\n\n");
   }
  
   list_free(FREE_LIST);
   list_free(ALLOC_LIST);
  
   return 0;
}

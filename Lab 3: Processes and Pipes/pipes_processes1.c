// C program to demonstrate use of fork() and pipe() 
#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 
// <Author> Julian Forbes @julianf17
// collaborated with Kyndall Jones and Zoe Carter  
int main()  // First pipe sends input string from parent; Second pipe sends concatenated string from child 
{ 
    int fd1[2];  // stores 2 ends of 1st pipe 
    int fd2[2];  // stores 2 ends of 2nd pipe 
  
    char fixed_str[] = "howard.edu"; 
    char input_str[100]; 
    pid_t p; 
  
    if (pipe(fd1) == -1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
    if (pipe(fd2) == -1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
  
    printf("Enter a string to concatenate: ");
    scanf("%s", input_str); 
    p = fork(); 
  
    if (p < 0) 
    { 
        fprintf(stderr, "fork Failed" ); 
        return 1; 
    } 
  
    else if (p > 0) 
    { 
        char concat_str[100];
        close(fd1[0]);  // Closes reading end of pipe 1
   
        write(fd1[1], input_str, strlen(input_str)+1);
        close(fd1[1]); // Closes writing end of pipe 1
        close(fd2[1]); // Closes writing end of pipe 2

        wait(NULL);

        read(fd2[0], concat_str, 100);
        printf("Concatenated string %s\n", concat_str);
        close(fd2[0]); // Closes reading end of pipe 2
    } 
  
  
    else
    { 
        close(fd1[1]);  // Closes writing end of pipe 1
      
        char concat_str[100];
        read(fd1[0], concat_str, 100);
 
        int k = strlen(concat_str); 
        int i; 
        for (i = 0; i < strlen(fixed_str); i++) 
            concat_str[k++] = fixed_str[i]; 

        concat_str[k] = '\0';    
  
        printf("Concatenated string %s\n", concat_str);
        
        close(fd1[0]); // Close both ends 
        close(fd2[0]);

        printf("Enter a string to concatenate:");
        scanf("%s", input_str);

        k = strlen(input_str);
        for (i=0; i<strlen(concat_str); i++) 
            input_str[k++] = concat_str[i]; 

        input_str[k] = '\0';  

        write(fd2[1], input_str, strlen(input_str)+1);
        close(fd2[1]); 
        exit(0); 
    } 
    
} 

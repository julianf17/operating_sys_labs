#include <stdio.h>
#include <stdlib.h>
// <Author> Julian Forbes @julianf17
// collaborated with Kyndall Jones and Zoe Carter
/* IMPLEMENT ME: Declare your functions here */
int add (int a, int b);
int subtract (int a, int b);
int multiply (int a, int b);
int divide (int a, int b);

int main (void)
{
	/* IMPLEMENT ME: Insert your algorithm here */
	int inp, x;
	int (*my_calculator[])(int, int) = {add, subtract, multiply, divide};

	printf("Operand ‘a’ : 6 | Operand ‘b’ : 3\n");
	printf("Specify operation to perform (0 : add | 1 : subtract | 2 : Multiply | 3 : divide | 4 : exit):\n");

	scanf("%d", &inp);
	x = (*my_calculator[inp])(6, 3);
	printf("x = %d\n", x);
}

/* IMPLEMENT ME: Define your functions here */
int add (int a, int b) {printf("Adding 'a' and 'b'\n"); return a + b;}
int subtract (int a, int b) {printf("Subtracting 'b' from 'a'\n"); return a - b;}
int multiply (int a, int b) {printf("Multiplying 'a' and 'b'\n"); return a * b;}
int divide (int a, int b) {printf("Dividing 'a' by 'b'\n"); return a / b;}

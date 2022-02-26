#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
char* ARGV0 = "assignment2";
#define NUMBER_OF_LINES	12
typedef struct Customer Customer;
struct Customer
{
	int timeOfEntry;
	int line;
	char name[10];
	int numberOfItems;
};
typedef struct Node Node;
struct Node
{
	Node* next;
	Customer* customer;
};
typedef struct Queue Queue;
struct Queue
{
	Node* head; // points to the first entry of the queue
	Node* tail; // points to the last entry of the queue
};

Queue* Init(Queue* q)
{
	if (q == NULL)
	{
		return NULL;
	}
	q->head = NULL;
	q->tail = NULL;
}

int Empty(Queue* q)
{
	if(q == NULL || q->head == NULL)
	{
		return 1;
	}
	return 0;
}

void Enqueue(Queue* q, Customer* c)
{
	Node* n = (Node*)malloc(sizeof(Node));
	n->next = NULL;
	n->customer = c;
	if (!Empty(q))
	{
		q->tail->next = n;
		q->tail = n;
		return;
	}
	q->head = q->tail = n;
}

void Dequeue(Queue* q)
{
	if (Empty(q))
	{
		return;
	}
	Node* n = q->head;
	if (q->head == q->tail)
	{
		q->head = q->tail = NULL;
	}
	else
	{
		q->head = q->head->next;
	}
	free(n);
}

Customer* Peak(Queue* q)
{
	if (q->head != NULL)
	{
		return q->head->customer;
	}
}

void Visit(Queue* q, void(*visitor)(Customer* c))
{
	if (q == NULL|| visitor == NULL)
	{
		return;
	}
	for (Node* n = q->head; n != NULL; n = n->next)
	{
		visitor(n->customer);
	}
}

void printCustomer(Customer* c)
{
	if (c == NULL)
	{
		return;
	}
	printf("%4d %2d %-9s %3d\n", c->timeOfEntry, c->line, c->name, c->numberOfItems);
}

void printQueue(Queue* q)
{
	Visit(q, printCustomer);
}

void executetest(FILE* input, int t)
{
	Queue * lines[NUMBER_OF_LINES];
	Queue *customers;
	int clock = 0; // represents how long it takes for things to happen
	int cashierIsBusy = 0; //keeps track of if the cashier is busy or not, how much longer will he be busy.
	int numberOFCustomers; // the number of customers
	if (fscanf(input,"%d", &numberOFCustomers) != 1)
	{
		fprintf(stderr, "%s: unable to find number of customers.\n", ARGV0);
		exit(1);
	}
	for (int l = 0; l < NUMBER_OF_LINES; ++l)
	{
		lines[l] = Init((Queue*)malloc(sizeof(Queue)));
	}
	customers = Init((Queue*)malloc(sizeof(Queue)));
	for (int c = 0; c < numberOFCustomers; ++c)
	{
		int timeOfEntry;
		int line;
		char name[10];
		int numberOfItems;
		if (fscanf(input,"%d %d %9s %d", &timeOfEntry, &line, name, &numberOfItems) != 4)
		{
			fprintf(stderr, "%s: unable to read record for customer %d in test %d.\n", ARGV0, c, t);
			exit(1);
		}
		Customer* c = (Customer*)malloc(sizeof(Customer));
		c->timeOfEntry = timeOfEntry;
		c->line = line;
		strncpy(c->name, name, sizeof(c->name));
		c->numberOfItems = numberOfItems;
	//	Enqueue(lines[line - 1], c);
		Enqueue(customers, c);
	}
	//Visit(customers, printCustomer);
	while (1)
	{
		int moreToDo = 0;
		for (int l = 0; l < NUMBER_OF_LINES; ++l)
		{
			if (!Empty(lines[l]))
			{
				moreToDo = 1;
				break;
			}
		}
		moreToDo |= !Empty(customers); // nothing more to do
		if (!moreToDo)
		{
			break; // your done :)
		}
		// checking to see if there is an open customer
		if (!Empty(customers))
		{
			Customer* c = Peak(customers);
			if (c->timeOfEntry <= clock)
			{
				Enqueue(lines[c->line - 1], c);
				Dequeue(customers); // moves the customer off of the im busy shopping into the line.
			//	printf("%s enters line %d at %d\n", c->name, c->line, clock);
			}
		}
		// is the cashier busy? if not he will go look for a customer || we are done
		if (cashierIsBusy <= 0)
		{
			//cashier not busy
			int promisingLine = 0;
			int minimumNumberOfItems =	INT_MAX;
			for (int l = 0; l < NUMBER_OF_LINES; ++l)
			{
				if (!Empty(lines[l]))
				{
					Customer* c = Peak(lines[l]);
					if (c->numberOfItems < minimumNumberOfItems)
					{
						promisingLine = l;
						minimumNumberOfItems = c->numberOfItems;
					}
				}
			}
			// if there are customers in a line
			if (minimumNumberOfItems < INT_MAX)
			{
				Customer* c = Peak(lines[promisingLine]);
				Dequeue(lines[promisingLine]);
				cashierIsBusy = 30 + 5 * c->numberOfItems;
				printf("%s from line %d checks out at time %d.\n", c->name, promisingLine + 1, clock + cashierIsBusy);
			}
		}
		cashierIsBusy--;
		clock++;
	}
}

int main(int argc, char *const argv[])
{
	ARGV0 = argv[0];
	FILE* input = fopen("assignment2input.txt", "r");
	if (input == NULL)
	{
		fprintf(stderr, "%s: unable to open assignment2input.txt.\n", ARGV0);
		perror(argv[0]);
		exit(1);
	}
	int numberOfTests;
	if (fscanf(input, "%d", &numberOfTests) != 1)
	{
		fprintf(stderr, "%s: unable to find number of tests.\n", ARGV0);
		exit(1);
	}
	for (int t = 0; t < numberOfTests; ++t)
	{
		executetest(input, t);
	}
	return 0;
}
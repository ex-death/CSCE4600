#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct Node{
	int data;
	struct Node* next;
};

// shared variables
struct Node* freeList = NULL;
struct Node* list1 = NULL;
struct Node* list2 = NULL;

pthread_cond_t openFree;        // shared buffer full
pthread_cond_t usedFree;        // shared buffer empty
pthread_cond_t filled1;        // shared buffer empty
pthread_cond_t filled2;        // shared buffer empty

pthread_mutex_t mutex1;        // pthread mutex
pthread_mutex_t mutex2;        // pthread mutex
pthread_mutex_t mutex3;        // pthread mutex

//unsigned int prod_index = 0;     // producer index into shared buffer
//unsigned int cons_index = 0;     // consumer index into shard buffer

// function prototypes
void * thread1(void *arg);
void * thread2(void *arg);
void * thread3(void *arg);
void Link(struct Node* prev_node, int new_data);


int main() 
{ 
    pthread_t t1_tid, t2_tid, t3_tid; 

    // initialize pthread and conditional variables
    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);
    pthread_mutex_init(&mutex3, NULL);
    pthread_cond_init(&openFree, NULL); // full shared buffer cond variable initialization
    pthread_cond_init(&usedFree, NULL); // empty shared buffer cond variable initialization
    pthread_cond_init(&filled1, NULL); // full shared buffer cond variable initialization
    pthread_cond_init(&filled2, NULL); // empty shared buffer cond variable initialization
    
    // start threads
    pthread_create(&t1_tid, NULL, thread1, NULL);
    pthread_create(&t2_tid, NULL, thread2, NULL);
    pthread_create(&t3_tid, NULL, thread3, NULL);
    
    // wait for threads to finish
    pthread_join(t1_tid, NULL);
    pthread_join(t2_tid, NULL);
    pthread_join(t3_tid, NULL);
            
    // clean up
    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    pthread_mutex_destroy(&mutex3);
    pthread_cond_destroy(&openFree);
    pthread_cond_destroy(&usedFree);
    pthread_cond_destroy(&filled1);
    pthread_cond_destroy(&filled2);
    
    return 0;
}



void * thread1(void *arg)
{
	int *b;
    while (1)
    {
        pthread_cond_wait(&openFree, &mutex1);
				pthread_cond_wait(&usedFree, &mutex1);

        pthread_mutex_lock(&mutex1);

        // b= unlink freelist

        pthread_mutex_unlock(&mutex1);
        
        pthread_cond_signal(&openFree);
				// produce info in b        

        pthread_mutex_lock(&mutex2); 
        
				//Link(list1, b);
				
				pthread_mutex_unlock(&mutex2); 
				
				pthread_cond_signal(&filled1);
				
    }

    return NULL;
}


void * thread2(void *arg)
{
	int *x;
	int *y;
     while (1)
    {
				pthread_cond_wait(&filled1, &mutex2);
				pthread_mutex_lock(&mutex2);
				// x = unlink list 1
				pthread_mutex_unlock(&mutex2);
				pthread_mutex_lock(&mutex1);
				// y = unlink freelist
				pthread_mutex_unlock(&mutex1);
				
				pthread_cond_signal(&openFree);
				// x to produce in y
				pthread_mutex_lock(&mutex1);
				//Link(&freeList, y);
				pthread_mutex_unlock(&mutex1);

				pthread_cond_signal(&usedFree);
				pthread_mutex_lock(&mutex3);
				//Link(list2, y);
				pthread_mutex_unlock(&mutex3);

				pthread_cond_signal(&filled2);

    }

    return NULL;
}

void * thread3 (void *arg)
{
	int *c;

	while(1)
	{
		pthread_cond_wait(&filled2, &mutex3);
		pthread_mutex_lock(&mutex3);
		//c = unlink list2
		pthread_mutex_unlock(&mutex3);
		
		//consume info c
		pthread_mutex_lock(&mutex1);
		//Link(freeList, c);
		pthread_mutex_unlock(&mutex1);

		pthread_cond_signal(&usedFree);
	}

}



/* Given a node prev_node, insert a new node after the given
prev_node */
void Link(struct Node* prev_node, int new_data)
{
    /*1. check if the given prev_node is NULL */
    if (prev_node == NULL) {
        printf("the given previous node cannot be NULL");
        return;
    }
 
    /* 2. allocate new node */
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
 
    /* 3. put in the data */
    new_node->data = new_data;
 
    /* 4. Make next of new node as next of prev_node */
    new_node->next = prev_node->next;
 
    /* 5. move the next of prev_node as new_node */
    prev_node->next = new_node;
}
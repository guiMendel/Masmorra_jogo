#ifndef LINKED_LIST_C
#define LINKED_LIST_C
// C program for generic linked list 
#include<stdio.h> 
#include<stdlib.h> 
#include<string.h> 
  
/* A linked list node */
typedef struct node_type { 
    // Any data type can be stored in this node 
    void *data; 
    
    // ponteiro para o prox elemento da lista
    struct node_type *next; 
} node_t; 
  
// inicia uma lista
node_t **list_new() {
    node_t **list = (node_t**) malloc(sizeof(node_t*));
    *list = NULL;
    return list;
}
  
/* Function to add a node at the beginning of Linked List. 
   This function expects a pointer to the data to be added 
   and size of the data type */
void push(node_t** head, void *new_data, size_t data_size) { 
    // Allocate memory for node 
    node_t* new_node = (node_t*)malloc(sizeof(node_t)); 
  
    new_node->data = malloc(data_size); 
    // Copy contents of new_data to newly allocated memory. 
    memcpy(new_node->data, new_data, data_size);

    new_node->next = (*head);
    *head = new_node;

    // Change head pointer as new node is added at the beginning 
}
// empurra um inteiro para o comeco da lista
void pushi(node_t** head, int data) { 
    // Allocate memory for node 
    node_t* new_node = (node_t*)malloc(sizeof(node_t)); 
  
    new_node->data = malloc(sizeof(int)); 
    // Copy contents of new_data to newly allocated memory. 
    memcpy(new_node->data, &data, sizeof(int));

    new_node->next = (*head);
    *head = new_node;

    // Change head pointer as new node is added at the beginning 
} 
  
/* Function to print nodes in a given linked list. fpitr is used 
   to access the function to be used for printing current node data. 
   Note that different data types need different specifier in printf() */
void list_each(node_t **head, void (*fptr)(void *)) { 
    node_t *node = *head;
    while (node != NULL) 
    { 
        (*fptr)(node->data); 
        node = node->next; 
    } 
}

// retorna um ponteiro com o n-esimo elemento da lista. Se ele exceder o tamanho da lista, retorna NULL
void *list_get(node_t **head, int n) {
    if(*head == NULL) return NULL;
    if(n == 0) return (*head)->data;
    if((*head)->next == NULL) return NULL;
    if(n > 0) return list_get(&(*head)->next, n-1);
    return NULL;
}
// assume que o n-esimo elemento da lista e um ponteiro para inteiro e retorna este inteiro. Caso n exceda o tamanho da lista, retorna -1
int list_geti(node_t **head, int n) {
    if(*head == NULL) return -1;
    if(n == 0) return *(int*)(*head)->data;
    if((*head)->next == NULL) return -1;
    if(n > 0) return *(int*)list_get(&(*head)->next, n-1);
    return -1;
}

// apaga a lista e libera todos seus recursos
void free_list(node_t **head) {
    node_t *node = *head;
    while (node != NULL) 
    { 
        node_t *aux = node->next;
        free(node->data);
        free(node);
        node = aux;
    } 
    free(head);
}

// retorna o numero de elementos armazenados na lista
int list_size(node_t **head) {
    int size = 0;

    for(node_t *node = *head; node != NULL; node = node->next)
        size++;
        
    return size;
}

#endif
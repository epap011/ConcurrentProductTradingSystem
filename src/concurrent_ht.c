#include "concurrent_ht.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

int hash_function(int key, int table_size, int collision_number);
int hash_1(int key, int table_size);
int hash_2(int key, int table_size);

struct HTNode** createHT(int size) {
    struct HTNode **hash_table = (struct HTNode**)malloc(sizeof(struct HTNode*) * size);
    assert(hash_table != NULL);

    for(int i = 0; i < size; i++) {
        hash_table[i] = (struct HTNode*)malloc(sizeof(struct HTNode)); /*Dummy ht nodes*/
        hash_table[i]->productID = -1;
        if(pthread_mutex_init(&hash_table[i]->lock, NULL) != 0) {                                    
            perror("Error: pthread_mutex_init failed!");                                                       
            exit(1);                                                                    
        }
    }

    return hash_table;
}

struct HTNode* createHTNode(int productID) {
    struct HTNode* ht_node = (struct HTNode*)malloc(sizeof(struct HTNode));
    assert(ht_node != NULL);
    ht_node->productID = productID;
    
    return ht_node;
}

int HTInsert(struct HTNode** hash_table, int hash_table_size, struct HTNode* ht_node) {
    int prev = -1, curr, counter, result;
    int key = ht_node->productID;

    curr = hash_function(key, hash_table_size, 0);    
    if(pthread_mutex_lock(&hash_table[curr]->lock) != 0) {
        perror("Error: pthread_mutex_lock failed!");
        exit(1);
    }

    counter = 1;
    while(hash_table[curr]->productID != -1) {
        prev = curr;
        curr = hash_function(key, hash_table_size, counter++);
        if(pthread_mutex_lock(&hash_table[curr]->lock) != 0) {
            perror("Error: pthread_mutex_lock failed!");
            exit(1);
        }

        if(pthread_mutex_unlock(&hash_table[prev]->lock) != 0) {
            perror("Error: pthread_mutex_lock failed!");
            exit(1);
        }
        printf("key to insert: %d | hash_table[%d]: %d\n", key, curr, hash_table[curr]->productID);
    }
    
    hash_table[curr]->productID = key;
    result = 1;

    if(pthread_mutex_unlock(&hash_table[curr]->lock) != 0) {
        perror("Error: pthread_mutex_lock failed!");
        exit(1);
    }

    if(prev != -1 && pthread_mutex_unlock(&hash_table[prev]->lock) != 0) {
        perror("Error: pthread_mutex_lock failed!");
        exit(1);
    }

    return result;
}

int hash_function(int key, int table_size, int collision_number) {
    return (hash_1(key, table_size)+collision_number+hash_2(key, table_size))%table_size;
}

int hash_1(int key, int table_size) {
    return key%table_size;
}

int hash_2(int key, int table_size) {
    return 23 - (key%23);
}

int HTSize(struct HTNode** hash_table) {
    return 0;
}

int HTProductIDSum(struct HTNode** hash_table) {
    return 0;
}

void printHT(struct HTNode** hash_table, int size) {
    for(int i = 0; i < size; i++) {
        printf("%d[%d]\n", i, hash_table[i]->productID);
    }
}
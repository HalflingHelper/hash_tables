#pragma once

#define HT_INITIAL_BASE_SIZE 53

typedef struct {
    char* key;
    char* value;
} ht_item;

typedef struct {
    int base_size;
    int size;        //How much space there is
    int count;       //How much space is being used
    ht_item** items; //An array of pointers to each item
} ht_hash_table;


ht_hash_table* ht_new();
ht_hash_table* ht_new_sized(int);
void ht_del_hash_table(ht_hash_table *ht);
void ht_insert(ht_hash_table* ht, const char* key, const char* value);
char* ht_search(ht_hash_table* ht, const char* key);
void ht_delete(ht_hash_table* ht, const char* key);

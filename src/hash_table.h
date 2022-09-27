#pragma once

typedef struct {
    char* key;
    char* value;
} ht_item;

typedef struct {
    int size;        //How much space there is
    int count;       //How much space is being used
    ht_item** items; //An array of pointers to each item
} ht_hash_table;


#include <stdlib.h>
#include <string.h>

#include "hash_table.h"

//Creates a new ht_item, with key k and value v
//This is marked as static, because it will only be called internally by the hash table
static ht_item* ht_item_new(const char* k, const char* v) {
    ht_item* i = malloc(sizeof(ht_item));
    i->key = strdup(k);
    i->value = strdup(v);
    return i;
}

//Create a new hash table
//Size is fixed at 53 for now
ht_hash_table* ht_new() {
    ht_hash_table* ht = malloc(sizeof(ht_hash_table));

    ht->size = 53;
    ht->count = 0;
    //Initialize every item as being NULL
    ht->items = calloc((size_t)ht->size, sizeof(ht_item*));
    return ht;
}

//Deletion functions as to not cause memory leaks :)
static void ht_del_item(ht_item* i) {
    free(i->key);
    free(i->value);
    free(i);
}

void ht_del_hash_table(ht_hash_table* ht) {
    for (int i = 0; i < ht->size; i++) {
        ht_item* item = ht->items[i];
        if (item != NULL) {
            ht_del_item(item);
        }
    }
    free(ht->items);
    free(ht);
}

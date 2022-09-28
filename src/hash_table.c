#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "hash_table.h"
#include "prime.h"

#define PRIME_1 163
#define PRIME_2 131

static ht_item HT_DELETED_ITEM = {NULL, NULL};

// Creates a new ht_item, with key k and value v
// This is marked as static, because it will only be called internally by the hash table
static ht_item *ht_item_new(const char *k, const char *v)
{
    ht_item *i = malloc(sizeof(ht_item));
    i->key = strdup(k);
    i->value = strdup(v);
    return i;
}

ht_hash_table* ht_new_sized(const int base_size) {
    ht_hash_table* ht = malloc(sizeof(ht_hash_table));
    ht->base_size = base_size;

    ht->size = next_prime(ht->base_size);

    ht->count = 0;
    ht->items = calloc((size_t)ht->size, sizeof(ht_item*));
    return ht;
}

ht_hash_table *ht_new()
{
    return ht_new_sized(HT_INITIAL_BASE_SIZE);
}

static void ht_resize(ht_hash_table* ht, const int base_size) {
    if (base_size < HT_INITIAL_BASE_SIZE) {
        return;
    }
    ht_hash_table* new_ht = ht_new_sized(base_size);
    for (int i = 0; i < ht->size; i++) {
        ht_item* item = ht->items[i];
        if (item != NULL && item != &HT_DELETED_ITEM) {
            ht_insert(new_ht, item->key, item->value);
        }
    }

    ht->base_size = new_ht->base_size;
    ht->count = new_ht->count;

    // To delete new_ht, we give it ht's size and items 
    const int tmp_size = ht->size;
    ht->size = new_ht->size;
    new_ht->size = tmp_size;

    ht_item** tmp_items = ht->items;
    ht->items = new_ht->items;
    new_ht->items = tmp_items;

    ht_del_hash_table(new_ht);
}

static void ht_resize_up(ht_hash_table* ht) {
    const int new_size = ht->base_size * 2;
    ht_resize(ht, new_size);
}

static void ht_resize_down(ht_hash_table* ht) {
    const int new_size = ht->base_size / 2;
    ht_resize(ht, new_size);
}

// Deletion functions as to not cause memory leaks :)
static void ht_del_item(ht_item *i)
{
    free(i->key);
    free(i->value);
    free(i);
}

void ht_del_hash_table(ht_hash_table *ht)
{
    for (int i = 0; i < ht->size; i++)
    {
        ht_item *item = ht->items[i];
        if (item != NULL)
        {
            ht_del_item(item);
        }
    }
    free(ht->items);
    free(ht);
}

// The actual hash function!
// Takes string s as input and returns a value between 0 and n, the desired array length
// a is a prime number larger than the alphabet size (128 for ASCII)
static int ht_hash(const char *s, const int a, const int n)
{
    long hash = 0;
    const int len_s = strlen(s);
    for (int i = 0; i < len_s; i++)
    {
        hash += (long)pow(a, len_s - i + 1) * s[i];
        hash = hash % n;
    }
    return (int)hash;
}

static int ht_get_hash(const char *s, const int num_buckets, const int attempt)
{
    const int hash_a = ht_hash(s, PRIME_1, num_buckets);
    const int hash_b = ht_hash(s, PRIME_2, num_buckets);
    return (hash_a + (attempt * (hash_b + 1))) % num_buckets;
}

void ht_insert(ht_hash_table* ht, const char *key, const char *value)
{
    const int load = ht->count * 100 / ht->size;
    if (load > 70) {
        ht_resize_up(ht);
    }

    ht_item *item = ht_item_new(key, value);
    int index = ht_get_hash(item->key, ht->size, 0);
    ht_item *current_item = ht->items[index];
    int i = 1;
    while (current_item != NULL && current_item != &HT_DELETED_ITEM)
    {
        if (strcmp(current_item->key, key) == 0) {
            ht_del_item(current_item);
            ht->items[index] = item;
            return;
        }
        index = ht_get_hash(item->key, ht->size, i);
        current_item = ht->items[index];
        i++;
    }
    ht->items[index] = item;
    ht->count++;
}

char *ht_search(ht_hash_table *ht, const char *key)
{
    int index = ht_get_hash(key, ht->size, 0);
    ht_item *current_item = ht->items[index];
    int i = 1;
    while (current_item != NULL)
    {
        if (current_item != &HT_DELETED_ITEM)
        {
            if (strcmp(current_item->key, key) == 0)
            {
                return current_item->value;
            }
        }
        index = ht_get_hash(key, ht->size, i);
        current_item = ht->items[index];
        i++;
    }
    return NULL;
}

// We can't actually delete an item, because it might break a collision chain
// Instead we replace it with a pointer to a global item representing something deleted
void ht_delete(ht_hash_table *ht, const char *key)
{
    const int load = ht->count * 100 / ht->size;
    if (load < 10) {
        ht_resize_down(ht);
    }

    int index = ht_get_hash(key, ht->size, 0);
    ht_item *current_item = ht->items[index];
    int i = 1;
    while (current_item != NULL)
    {
        if (current_item != &HT_DELETED_ITEM)
        {
            if (strcmp(current_item->key, key) == 0)
            {
                ht_del_item(current_item);
                ht->items[index] = &HT_DELETED_ITEM;
            }
        }
        index = ht_get_hash(key, ht->size, i);
        current_item = ht->items[index];
        i++;
    }
    ht->count--;
}

void ht_print_hash_table(ht_hash_table* ht) {
    for (int i = 0; i < ht->size; i++) {
        ht_item* item = ht->items[i];
        if (item != NULL && item != &HT_DELETED_ITEM) {
            printf("%-15s%s\n", item->key, item->value);
        }
    }
}

#include <stdlib.h>
#include <string.h>

#include "hash_table.h"

#define PRIME_1 163
#define PRIME_2 131

// Creates a new ht_item, with key k and value v
// This is marked as static, because it will only be called internally by the hash table
static ht_item *ht_item_new(const char *k, const char *v)
{
    ht_item *i = malloc(sizeof(ht_item));
    i->key = strdup(k);
    i->value = strdup(v);
    return i;
}

// Create a new hash table
// Size is fixed at 53 for now
ht_hash_table *ht_new()
{
    ht_hash_table *ht = malloc(sizeof(ht_hash_table));

    ht->size = 53;
    ht->count = 0;
    // Initialize every item as being NULL
    ht->items = calloc((size_t)ht->size, sizeof(ht_item *));
    return ht;
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
    }
    hash = hash % n;
    return (int)hash;
}

static int ht_get_hash(const char *s, const int num_buckets, const int attempt)
{
    const int hash_a = ht_hash(s, PRIME_1, num_buckets);
    const int hash_b = ht_hash(s, PRIME_2, num_buckets);
    return (hash_a + (attempt * (hash_b + 1))) % num_buckets;
}

void ht_insert(ht_hash_table *ht, const char *key, const char *value)
{
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

static ht_item HT_DELETED_ITEM = {NULL, NULL};
// We can't actually delete an item, because it might break a collision chain
// Instead we replace it with a pointer to a global item representing something deleted
void ht_delete(ht_hash_table *ht, const char *key)
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

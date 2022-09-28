#include <stdio.h>

#include "hash_table.h"


int main() {
    ht_hash_table* my_ht = ht_new();

    ht_insert(my_ht, "first", "1");
    ht_insert(my_ht, "second", "2");
    ht_insert(my_ht, "third", "3");
    ht_insert(my_ht, "fourth", "4");


    char* s = ht_search(my_ht, "second");

    

    ht_print_hash_table(my_ht);

    return 0;

}
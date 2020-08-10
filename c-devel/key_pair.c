#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast_node.h"


void append_key_object_pair(
    struct key_object_pair **dest,
    size_t *size,
    char *key,
    struct mpl_object *value
) {
    *size++;
    *dest = realloc(*dest, *size * sizeof(struct key_object_pair));
    struct key_object_pair *this_pair = *dest + (*size - 1);
    this_pair->key = key;
    this_pair->value = value;
}


void append_key_program_block_pair(
    struct key_program_block_pair **dest,
    size_t *size,
    char *key,
    struct mpl_program_block *value
) {
    *size++;
    *dest = realloc(*dest, *size * sizeof(struct key_program_block_pair));
    struct key_program_block_pair *this_pair = *dest + (*size - 1);
    this_pair->key = key;
    this_pair->value = value;
}

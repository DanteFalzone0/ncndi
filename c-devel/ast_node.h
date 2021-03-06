#include "config.h"
#include <stdlib.h>
#include "token.h"

#ifndef AST_NODE_H
#define AST_NODE_H

/* The types supported by the MPL language. */
enum mpl_type {
    FLOAT,
    INT,
    STRING,
    VOID
};

enum ast_node_type {
    MPL_PROGRAM_BLOCK,
    MPL_OBJECT,
    MPL_VARIABLE,
    MPL_FUNCTION,
    BINARY_OP
};

/* These forward declarations prevent the compiler from screaming
   that it doesn't yet know what they are when we compile the
   eval() methods for various objects. */
struct ast_node;
struct key_object_pair;
struct key_program_block_pair;

/* This method returns NULL or a pointer to a heap-allocated
   struct mpl_object. The caller is responsible for freeing
   this. */
#define EVAL_METHOD struct mpl_object *(*eval)(\
    struct ast_node *,\
    size_t *,\
    struct key_object_pair *,\
    size_t *,\
    struct key_program_block_pair *\
);

#define DESTROY_CHILDREN_METHOD void (*destroy_children)(struct ast_node *);

/*
    A program is represented with a struct mpl_program_block containing a
    pointer to an array of children. The children are all heap-allocated,
    so when it's cleanup time, we call the destroy_children() method on
    it before freeing it. The cool thing is that every struct which
    "inherits" from ast_node (meaning it has an eval method and
    destroy_children method at the same memory offsets) has its own
    destroy_children method, which gets called on its own children before
    it too gets freed. In the same way the program evaluates itself by
    recursively calling its children's implementations of eval(), it
    deallocates itself by recursively calling its children's
    implementations of destroy_children().

    TL;DR: Every object that has a destroy_children() method is responsible
    for freeing all heap-allocated memory that it has pointers to, and all
    pointers in an object with a destroy_children() method are expected to
    point to heap-allocated memory.
*/

// This struct should never be actually instantiated.
struct ast_node {
    enum ast_node_type node_type;
    EVAL_METHOD;
    DESTROY_CHILDREN_METHOD;
};

struct mpl_object;
struct mpl_program_block;

/*
    The pointers in a key_object_pair and a key_program_block_pair are non-owning.
    The caller is responsible for freeing them.
*/
struct key_object_pair {
    char *key;
    struct mpl_object *value;
};

struct key_program_block_pair {
    char *key;
    struct mpl_program_block *value;
};

/* The pointers in the next two functions' arguments are non-owning too. */

void append_key_object_pair(
    struct key_object_pair **dest,
    size_t *size,
    char *key,
    struct mpl_object *value
);

void append_key_program_block_pair(
    struct key_program_block_pair **dest,
    size_t *size,
    char *key,
    struct mpl_program_block *value
);

/*
    The structs below all own the memory to which their members point and are thus responsible
    for freeing them with their destroy_children() methods.
*/

struct mpl_program_block {
    enum ast_node_type node_type;
    EVAL_METHOD;
    DESTROY_CHILDREN_METHOD;
    size_t child_count;
    struct ast_node **children;
};

void construct_mpl_program_block(struct mpl_program_block *dest);

void mpl_program_block_append_child(
    struct mpl_program_block *dest,
    struct ast_node **child
);

struct mpl_object {
    enum ast_node_type node_type;
    EVAL_METHOD;
    DESTROY_CHILDREN_METHOD;
    enum mpl_type type;
    union {
        double float_value;
        long long int_value;
        char *string_value;
    } value;
};

void construct_mpl_object(
    struct mpl_object *dest,
    enum mpl_type type,
    const char *value /* String representation, regardless of type. */
);

struct mpl_variable {
    enum ast_node_type node_type;
    EVAL_METHOD;
    DESTROY_CHILDREN_METHOD;
    char *identifier;
};


void construct_mpl_variable(
    struct mpl_variable *dest,
    const char *identifier
);

struct mpl_function {
    enum ast_node_type node_type;
    EVAL_METHOD;
    DESTROY_CHILDREN_METHOD;
    char *identifier;
    enum mpl_type return_type;
    struct mpl_program_block *body;
};

void construct_mpl_function(
    struct mpl_function *dest,
    const char *identifier,
    enum mpl_type return_type,
    struct mpl_program_block **body
);

struct binary_op {
    enum ast_node_type node_type;
    EVAL_METHOD;
    DESTROY_CHILDREN_METHOD;
    struct ast_node *left;
    struct token *op;
    struct ast_node *right;
};

void construct_binary_op(
    struct binary_op *dest,
    struct ast_node **left,
    struct token *op,
    struct ast_node **right
);

#endif // AST_NODE_H

/*
    Compile with:
    $ gcc -c lexer.c
    $ gcc -c token.c
    $ gcc text.c lexer.o token.o

    TODO write a makefile
*/

#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "token.h"
#include "lexer.h"
#include "ast_node.h"


int main(int argc, char *argv[]) {
    char *program_code;
    if (argc >= 2) {
        program_code = argv[1];
    } else {
        program_code = "funct foo() {\n  int x = 60 % 3;\n  print \"Hello!\\n\";\n}\n\n# here's a comment\nstart {\n  foo();\n} end\n";
    }

    struct lexer lex = {
        .text = program_code,
        .text_size = strlen(program_code),
        .pos = 0,
        .current_char = program_code[0]
    };

    printf("-=-=-=-=-=-=-=-=-=-=-=-=-\n");
    printf("Program to tokenize:\n%s\n", lex.text);

    /* We slurp up tokens from the code and
       spit them out as soon as we've got them. */
    struct token *tok = NULL;
    printf("-=-=-=-=-=-=-=-=-=-=-=-=-\n");
    printf("Got these tokens:\n");
    do {
        free_token(tok);
        tok = get_next_token(&lex);
        if (tok == NULL) {
            printf("<NULL token returned>\n");
            goto fail;
        } else {
            if (tok->type == STRING_L) {
                printf("\"%s\"\n", tok->value);
            } else {
                printf("%s\n", tok->value);
            }
        }
    } while (tok->type != END_OF_FILE && tok != NULL);
    printf("-=-=-=-=-=-=-=-=-=-=-=-=-\n");
    free_token(tok);

    return 0;

fail:
    printf("-=-=-=-=-=-=-=-=-=-=-=-=-\n");
    fprintf(stderr, "An unexpected NULL pointer was returned from get_next_token().\n");
    free_token(tok);
    return 1;

}

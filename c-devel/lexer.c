#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "token.h"
#include "lexer.h"

enum token_type reserved_word_type(const char *reserved_word) {
    if (!strcmp("start", reserved_word)) {
        return START;
    } else if (!strcmp("end", reserved_word)) {
        return END;
    } else if (!strcmp("print", reserved_word)) {
        return PRINT;
    } else if (!strcmp("int", reserved_word)) {
        return TYPE_IDENTIFIER;
    } else if (!strcmp("float", reserved_word)) {
        return TYPE_IDENTIFIER;
    } else if (!strcmp("string", reserved_word)) {
        return TYPE_IDENTIFIER;
    } else if (!strcmp("funct", reserved_word)) {
        return FUNCTION_DECL;
    } else if (!strcmp("return", reserved_word)) {
        return RETURN;
    } else if (!strcmp("if", reserved_word)) {
        return IF;
    } else if (!strcmp("else", reserved_word)) {
        return ELSE;
    } else if (!strcmp("while", reserved_word)) {
        return WHILE;
    } else if (!strcmp("for", reserved_word)) {
        return FOR;
    }
    return IDENTIFIER;
}

int is_reserved_word(const char *string) {
    size_t i;
    for (i = 0; i < reserved_word_count; ++i) {
        if (!strcmp(string, reserved_words[i])) {
            return 1;
        }
    }
    return 0;
}

void lexer_error(const char *error_message) {
    fprintf(
        stderr,
        "Error while lexing: %s\n",
        error_message
    );
}

void advance(struct lexer *lex) {
    lex->pos++;
    if (lex->pos >= lex->text_size) {
        lex->current_char = '\0';
    } else {
        lex->current_char = lex->text[lex->pos];
    }
}

void skip_whitespace(struct lexer *lex) {
    while (lex->current_char && isspace(lex->current_char)) {
        advance(lex);
    }
}

void skip_comment(struct lexer *lex) {
    if (lex->current_char == '#') {
        while (lex->current_char != '\r' && lex->current_char != '\n') {
            advance(lex);
        }
    }

    while (lex->current_char == '\r' || lex->current_char == '\n') {
        advance(lex);
    }
}

struct token *parse_number(struct lexer *lex) {
    // TODO implement
}

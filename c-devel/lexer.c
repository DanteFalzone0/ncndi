#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "token.h"
#include "lexer.h"


const char *reserved_words[] = {
    "start",
    "end",
    "print",
    "int",
    "float",
    "string",
    "funct",
    "return",
    "if",
    "else",
    "while",
    "for"
};

size_t reserved_word_count = 12;


enum token_type identifier_type(const char *identifier) {
    MPL_DEBUG(fprintf(stderr, "DEBUG: Calling indentifier_type() on \"%s\".\n", identifier));

    enum token_type i;
    for (i = 0; i < reserved_word_count; ++i) {
        if (!strcmp(identifier, reserved_words[i])) {
            MPL_DEBUG(fprintf(stderr,
                "DEBUG:\t\tIdentifier type was MPL reserved word \"%s\".\n", reserved_words[i]));

            return i;
        }
    }

    MPL_DEBUG(fprintf(stderr, "DEBUG:\t\tIdentifier was not an MPL reserved word.\n"));
    return IDENTIFIER;

}


static void advance(struct lexer *lex) {
    lex->pos++;
    if (lex->pos >= lex->text_size) {
        lex->current_char = '\0';
    } else {
        lex->current_char = lex->text[lex->pos];
    }
}


static void skip_whitespace(struct lexer *lex) {
    while (lex->current_char && isspace(lex->current_char)) {
        advance(lex);
    }
}


static void skip_comment(struct lexer *lex) {
    if (lex->current_char == '#') {
        while (lex->current_char != '\r' && lex->current_char != '\n') {
            advance(lex);
        }
    }

    while (lex->current_char == '\r' || lex->current_char == '\n') {
        advance(lex);
    }
}


static struct token *parse_number(struct lexer *lex) {
    size_t char_count = 0;
    int is_float = 0;

    // Find out how many chars we need to store the number.
    size_t i = lex->pos;
    while (lex->text[i] == '.'
           || isdigit(lex->text[i])
    ) {
        char_count++; i++;
        // Are we parsing a float?
        if (lex->text[i] == '.') is_float = 1;
    }

    // Give ourselves a tiny bit of extra room, just in case.
    char_count++;

    char *result_number = calloc(char_count, sizeof(char));

    // Copy the number into the allocated buffer. Wish I knew a DRYer way...
    i = 0;
    while (
        lex->current_char == '.'
        || isdigit(lex->current_char)
    ) {
        result_number[i] = lex->current_char;
        ++i;
        advance(lex);
    }

    struct token *result = malloc(sizeof(struct token));
    result->type = is_float? FLOAT_L : INT_L;
    result->value = result_number;
    return result;

}


static struct token *parse_identifier(struct lexer *lex) {
    // Start by getting the first char, and making sure it's valid.
    char first_char;
    if (
        lex->current_char == '_'
        || isalpha(lex->current_char)
    ) {
        first_char = lex->current_char;
        advance(lex);
    } else {
        fprintf(
            stderr,
            "Error while lexing: identifiers can't start with %c\n",
            lex->current_char
        );
        return NULL;
    }

    // Next we count how much more space we're going to need.
    size_t char_count = 1;
    size_t i = lex->pos;
    while (
        lex->text[i] == '_'
        || isalpha(lex->text[i])
        || isdigit(lex->text[i])
    ) {
        ++i; ++char_count;
    }

    // Get the memory we need plus a little extra
    char *result_identifier = calloc(char_count + 1, sizeof(char));

    // Slurp up that juicy identifier
    result_identifier[0] = first_char;
    for (i = 1; i < char_count; ++i) {
        result_identifier[i] = lex->current_char;
        advance(lex);
    }

    struct token *result = malloc(sizeof(struct token));
    result->type = identifier_type(result_identifier);
    result->value = result_identifier;
    return result;

}


/* This condition looks butt-ugly, so I made it
   into a macro. Sue me. */
#define INSIDE_STRING_LITERAL(lex, i)         \
    lex->text[i] != '"'                       \
    || (lex->text[i] == '"'                   \
        && lex->text[i - 1] == '\\'           \
        && lex->text[i - 2] != '\\')


static struct token *parse_string_literal(struct lexer *lex) {
    if (lex->current_char == '"') {
        advance(lex);
    } else {
        fprintf(stderr, "Error while lexing: String literals must begin with '\"'\n");
        return NULL;
    }

    // Find out how much space we need to allocate
    size_t char_count = 0, i = lex->pos;
    while (INSIDE_STRING_LITERAL(lex, i)) { ++char_count; ++i; }

    char *parsed_chars = calloc(char_count + 1, sizeof(char));
    for (i = 0; i < char_count; ++i) {
        parsed_chars[i] = lex->current_char;
        advance(lex);
    }
    advance(lex); // skip the closing quotation mark when done looping

    char *result_string = calloc(char_count + 1, sizeof(char));

    // Copy chars and handle escape sequences
    size_t j = 0;
    for (i = j; i < char_count; ++i, ++j) {
        if (parsed_chars[i] == '\\') {
            switch (parsed_chars[++i]) {
                case 'n':
                    result_string[j] = '\n'; break;
                case 'r':
                    result_string[j] = '\r'; break;
                case 't':
                    result_string[j] = '\t'; break;
                case '\\':
                    result_string[j] = '\\'; break;
                case '"':
                    result_string[j] = '"'; break;
            }
        } else {
            result_string[j] = parsed_chars[i];
        }
    }

    free(parsed_chars);
    struct token *result = malloc(sizeof(struct token));
    result->type = STRING_L; result->value = result_string;
    return result;

}


struct token *get_next_token(struct lexer *lex) {
    MPL_DEBUG(fprintf(stderr, "DEBUG: Calling get_next_token() on lexer object @ %p.\n", (void *)lex));
    while (lex->current_char != '\0') {
        /* This code isn't great. It's very repetetive.
           I'll probably refactor most of it into a switch
           statement  at  some  point,  and  find a way to
           make  it  so it doesn't have the same few lines
           copied almost verbatim all over the place. */
        if (lex->current_char == '_'
            || isalpha(lex->current_char)
        ) {
            return parse_identifier(lex);
        }

        if (isspace(lex->current_char)) {
            skip_whitespace(lex);
            return get_next_token(lex);
        }

        if (isdigit(lex->current_char)) {
            return parse_number(lex);
        }

        if (lex->current_char == ',') {
            advance(lex);
            struct token *result = malloc(sizeof(struct token));
            construct_token(result, COMMA, ",");
            return result;
        }

        if (lex->current_char == '"') {
            return parse_string_literal(lex);
        }

        if (lex->current_char == '#') {
            skip_comment(lex);
            return get_next_token(lex);
        }

        if (lex->current_char == '+') {
            advance(lex);
            struct token *result = malloc(sizeof(struct token));
            if (lex->current_char == '=') {
                advance(lex);
                construct_token(result, PLUS_ASSIGN, "+=");
                return result;
            }
            construct_token(result, PLUS, "+");
            return result;
        }

        if (lex->current_char == '-') {
            advance(lex);
            struct token *result = malloc(sizeof(struct token));
            if (lex->current_char == '=') {
                advance(lex);
                construct_token(result, SUBTRACT_ASSIGN, "-=");
                return result;
            }
            construct_token(result, SUBTRACT, "-");
            return result;
        }

        if (lex->current_char == '*') {
            advance(lex);
            struct token *result = malloc(sizeof(struct token));
            if (lex->current_char == '=') {
                advance(lex);
                construct_token(result, MULT_ASSIGN, "*=");
                return result;
            }
            construct_token(result, MULT, "*");
            return result;
        }

        if (lex->current_char == '/') {
            advance(lex);
            struct token *result = malloc(sizeof(struct token));
            if (lex->current_char == '=') {
                advance(lex);
                construct_token(result, DIVIDE_ASSIGN, "/=");
                return result;
            }
            construct_token(result, DIVIDE, "/");
            return result;
        }

        if (lex->current_char == '(') {
            advance(lex);
            struct token *result = malloc(sizeof(struct token));
            construct_token(result, OPEN_PAREN, "(");
            return result;
        }

        if (lex->current_char == ')') {
            advance(lex);
            struct token *result = malloc(sizeof(struct token));
            construct_token(result, CLOSE_PAREN, ")");
            return result;
        }

        if (lex->current_char == '{') {
            advance(lex);
            struct token *result = malloc(sizeof(struct token));
            construct_token(result, OPEN_BRACE, "{");
            return result;
        }

        if (lex->current_char == '}') {
            advance(lex);
            struct token *result = malloc(sizeof(struct token));
            construct_token(result, CLOSE_BRACE, "}");
            return result;
        }

        if (lex->current_char == ';') {
            advance(lex);
            struct token *result = malloc(sizeof(struct token));
            construct_token(result, SEMICOLON, ";");
            return result;
        }

        if (lex->current_char == '=') {
            advance(lex);
            struct token *result = malloc(sizeof(struct token));
            if (lex->current_char == '=') {
                advance(lex);
                construct_token(result, EQUALS, "==");
                return result;
            }
            construct_token(result, ASSIGN, "=");
            return result;
        }

        if (lex->current_char == '>') {
            advance(lex);
            struct token *result = malloc(sizeof(struct token));
            if (lex->current_char == '=') {
                advance(lex);
                construct_token(result, GREATER_EQ, ">=");
                return result;
            }
            construct_token(result, GREATER_THAN, ">");
            return result;
        }

        if (lex->current_char == '<') {
            advance(lex);
            struct token *result = malloc(sizeof(struct token));
            if (lex->current_char == '>') {
                advance(lex);
                construct_token(result, XOR, "<>");
                return result;
            } else if (lex->current_char == '=') {
                advance(lex);
                construct_token(result, LESS_EQ, "<=");
            }
            construct_token(result, LESS_THAN, "<");
            return result;
        }

        if (lex->current_char == '&') {
            advance(lex);
            if (lex->current_char == '&') {
                advance(lex);
                struct token *result = malloc(sizeof(struct token));
                construct_token(result, AND, "&&");
                return result;
            }
            return NULL;
        }

        if (lex->current_char == '|') {
            advance(lex);
            if (lex->current_char == '|') {
                advance(lex);
                struct token *result = malloc(sizeof(struct token));
                construct_token(result, OR, "||");
                return result;
            }
            return NULL;
        }

        if (lex->current_char == '!') {
            advance(lex);
            struct token *result = malloc(sizeof(struct token));
            if (lex->current_char == '=') {
                advance(lex);
                construct_token(result, NOT_EQ, "!=");
                return result;
            }
            construct_token(result, NOT, "!");
            return result;
        }

        if (lex->current_char == '%') {
            advance(lex);
            struct token *result = malloc(sizeof(struct token));
            construct_token(result, MODULUS, "%");
            return result;
        }

        fprintf(
            stderr,
            "Error while lexing: Syntax error at unexpected character %c\n",
            lex->current_char
        );
        return NULL;

    }

    struct token *result = malloc(sizeof(struct token));
    construct_token(result, END_OF_FILE, "<end of file>");
    return result;

}

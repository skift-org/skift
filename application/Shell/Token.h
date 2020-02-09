#pragma once

#define TOKEN_LITERAL_MAX_LENGHT 256

typedef enum
{
    TOKEN_ARGUMENT,
    TOKEN_WHITESPACE,
    TOKEN_VALUE,
    TOKEN_STRING_LITERAL,
    TOKEN_NUMBER_LITERAL,
    TOKEN_EOF,
} TokenType;

typedef struct
{
    TokenType type;
    char literal[TOKEN_LITERAL_MAX_LENGHT];

    int ln, col;
} Token;

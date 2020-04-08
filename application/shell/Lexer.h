#pragma once

#include "shell/Source.h"
#include "shell/Token.h"

typedef struct
{
    Source *source;
} Lexer;

Lexer *lexer_create(Source *source);

void lexer_destroy(Lexer *lexer);

Token lexer_next_token(Lexer *lexer);
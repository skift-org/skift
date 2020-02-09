#include "Shell/Lexer.h"

Lexer *lexer_create(Source *source)
{
    Lexer *lexer = __create(Lexer);

    lexer->source = source;

    return lexer;
}

void lexer_destroy(Lexer *lexer)
{
    source_destroy(lexer->source);

    free(lexer);
}

Token lexer_next_token(Lexer *lexer)
{
    __unused(lexer);

    return (Token){};
}

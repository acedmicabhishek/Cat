#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {
    // Keywords
    FN, RETURN, IF, ELSE, WHILE,
    INT_TYPE, FLOAT_TYPE, STRING_TYPE, BOOL_TYPE,
    PRINT, SCAN, MEOW, MAIN,

    // Literals
    IDENTIFIER, INT_LITERAL, FLOAT_LITERAL, STRING_LITERAL, BOOL_LITERAL,

    // Operators
    ASSIGN, PLUS, GT, LESS, ARROW, COLON,
    EQUAL_EQUAL, BANG_EQUAL, LESS_EQUAL, GREATER_EQUAL,
    AMPERSAND_AMPERSAND, PIPE_PIPE, BANG,

    // Punctuation
    LPAREN, RPAREN, LBRACE, RBRACE, SEMICOLON, COMMA,

    // Other
    COMMENT,
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

#endif
#ifndef LEXER_H
#define LEXER_H

#include "token.h"
#include <string>
#include <vector>

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    void skipWhitespace();
    Token nextToken();
    Token identifier();
    Token number();
    Token stringLiteral();
    char advance();
    bool isAtEnd();
    bool match(char expected);
    char peek();
    char peekNext();
    bool isDigit(char c);
    bool isAlpha(char c);
    bool isAlphaNumeric(char c);

    std::string source;
    int start = 0;
    int current = 0;
    int line = 1;
    int column = 1;
};

#endif
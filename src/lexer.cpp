#include "lexer.h"
#include <unordered_map>

static const std::unordered_map<std::string, TokenType> keywords = {
    {"fn", TokenType::FN},
    {"return", TokenType::RETURN},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"while", TokenType::WHILE},
    {"int", TokenType::INT_TYPE},
    {"float", TokenType::FLOAT_TYPE},
    {"string", TokenType::STRING_TYPE},
    {"bool", TokenType::BOOL_TYPE},
    {"true", TokenType::BOOL_LITERAL},
    {"false", TokenType::BOOL_LITERAL},
    {"print", TokenType::PRINT},
    {"scan", TokenType::SCAN},
    {"meow", TokenType::MEOW},
};

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        skipWhitespace();
        if (isAtEnd()) break;
        tokens.push_back(nextToken());
    }
    tokens.push_back({TokenType::END_OF_FILE, "", line, column});
    return tokens;
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                line++;
                column = 1;
                advance();
                break;
            case '/':
                if (peekNext() == '/') {
                    // A comment goes until the end of the line.
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

Token Lexer::nextToken() {
    start = current;
    char c = advance();

    switch (c) {
        case '(': return {TokenType::LPAREN, "(", line, column};
        case ')': return {TokenType::RPAREN, ")", line, column};
        case '{': return {TokenType::LBRACE, "{", line, column};
        case '}': return {TokenType::RBRACE, "}", line, column};
        case ';': return {TokenType::SEMICOLON, ";", line, column};
        case ',': return {TokenType::COMMA, ",", line, column};
        case '+': return {TokenType::PLUS, "+", line, column};
        case ':': return {TokenType::COLON, ":", line, column};
        case '=':
            return match('=') ? Token{TokenType::EQUAL_EQUAL, "==", line, column} : Token{TokenType::ASSIGN, "=", line, column};
        case '!':
            return match('=') ? Token{TokenType::BANG_EQUAL, "!=", line, column} : Token{TokenType::BANG, "!", line, column};
        case '<':
            return match('=') ? Token{TokenType::LESS_EQUAL, "<=", line, column} : Token{TokenType::LESS, "<", line, column};
        case '>':
            return match('=') ? Token{TokenType::GREATER_EQUAL, ">=", line, column} : Token{TokenType::GT, ">", line, column};
        case '&':
            if (match('&')) {
                return {TokenType::AMPERSAND_AMPERSAND, "&&", line, column};
            }
            break;
        case '|':
            if (match('|')) {
                return {TokenType::PIPE_PIPE, "||", line, column};
            }
            break;
        case '-':
            if (match('>')) {
                return {TokenType::ARROW, "->", line, column};
            }
            break;
        case '"': return stringLiteral();
        default:
            if (isAlpha(c)) {
                return identifier();
            }
            if (isDigit(c)) {
                return number();
            }
    }

    return {TokenType::UNKNOWN, std::string(1, c), line, column};
}

Token Lexer::identifier() {
    while (isAlphaNumeric(peek())) {
        advance();
    }
    std::string text = source.substr(start, current - start);
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        return {it->second, text, line, column};
    }
    return {TokenType::IDENTIFIER, text, line, column};
}

Token Lexer::number() {
    bool isFloat = false;
    while (isDigit(peek())) {
        advance();
    }
    if (peek() == '.' && isDigit(peekNext())) {
        isFloat = true;
        advance(); // Consume the '.'
        while (isDigit(peek())) {
            advance();
        }
    }
    std::string text = source.substr(start, current - start);
    return {isFloat ? TokenType::FLOAT_LITERAL : TokenType::INT_LITERAL, text, line, column};
}

Token Lexer::stringLiteral() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            column = 1;
        }
        advance();
    }

    if (isAtEnd()) {
        // Unterminated string
        return {TokenType::UNKNOWN, "Unterminated string.", line, column};
    }

    advance(); // The closing "
    std::string value = source.substr(start + 1, current - start - 2);
    return {TokenType::STRING_LITERAL, value, line, column};
}

char Lexer::advance() {
    current++;
    column++;
    return source[current - 1];
}

bool Lexer::isAtEnd() {
    return current >= source.length();
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    current++;
    column++;
    return true;
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool Lexer::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool Lexer::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Lexer::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}
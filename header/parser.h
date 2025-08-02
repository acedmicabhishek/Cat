#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"
#include <vector>
#include <memory>

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<ModuleAST> parse();

private:
    std::unique_ptr<Stmt> parseStatement();
    std::unique_ptr<Expr> parseExpression();
    std::unique_ptr<Expr> parsePrimary();
    std::unique_ptr<Expr> parseUnary();
    std::unique_ptr<Expr> parseBinOpRHS(int exprPrec, std::unique_ptr<Expr> LHS);
    std::unique_ptr<PrototypeAST> parsePrototype();
    std::unique_ptr<FunctionAST> parseDefinition();
    std::unique_ptr<Expr> parseIdentifierExpr();
    std::unique_ptr<Expr> parseNumberExpr();
    std::unique_ptr<Expr> parseStringExpr();
    std::unique_ptr<Expr> parseBoolExpr();
    std::unique_ptr<Expr> parseParenExpr();
    std::unique_ptr<Stmt> parseReturnStmt();
    std::unique_ptr<Stmt> parsePrintStmt();
    std::unique_ptr<Stmt> parseScanStmt();
    std::unique_ptr<Stmt> parseVarDeclStmt();
    std::unique_ptr<Stmt> parseIfStmt();
    std::unique_ptr<Stmt> parseWhileStmt();
    std::unique_ptr<BlockStmt> parseBlock();

    Token& currentToken();
    void advance();
    bool check(TokenType type);
    bool match(TokenType type);
    bool isType();
    int getTokPrecedence();

    std::vector<Token> tokens;
    size_t current = 0;
};

#endif
#include "parser.h"
#include <map>

static std::map<TokenType, int> BinopPrecedence;

static void initializePrecedence() {
    BinopPrecedence[TokenType::LESS] = 10;
    BinopPrecedence[TokenType::EQUAL_EQUAL] = 10;
    BinopPrecedence[TokenType::BANG_EQUAL] = 10;
    BinopPrecedence[TokenType::LESS_EQUAL] = 10;
    BinopPrecedence[TokenType::GREATER_EQUAL] = 10;
    BinopPrecedence[TokenType::PLUS] = 20;
    BinopPrecedence[TokenType::AMPERSAND_AMPERSAND] = 5;
    BinopPrecedence[TokenType::PIPE_PIPE] = 5;
}

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {
    initializePrecedence();
}

std::unique_ptr<ModuleAST> Parser::parse() {
    auto module = std::make_unique<ModuleAST>();
    while (current < tokens.size() && tokens[current].type != TokenType::END_OF_FILE) {
        if (auto f = parseDefinition()) {
            module->Functions.push_back(std::move(f));
        } else {
            // Skip to the next token to avoid infinite loops on errors
            advance();
        }
    }
    return module;
}

Token& Parser::currentToken() {
    return tokens[current];
}

void Parser::advance() {
    if (current < tokens.size() - 1) {
        current++;
    }
}

bool Parser::check(TokenType type) {
    if (current >= tokens.size()) return false;
    return currentToken().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::isType() {
    return check(TokenType::INT_TYPE) || check(TokenType::FLOAT_TYPE) || check(TokenType::STRING_TYPE) || check(TokenType::BOOL_TYPE);
}

int Parser::getTokPrecedence() {
    if (BinopPrecedence.count(currentToken().type)) {
        return BinopPrecedence[currentToken().type];
    }
    return -1;
}

std::unique_ptr<Expr> Parser::parseIdentifierExpr() {
    std::string idName = currentToken().value;
    advance(); // consume identifier.

    if (!match(TokenType::LPAREN)) // Simple variable ref.
        return std::make_unique<VariableExpr>(idName);

    // Call.
    std::vector<std::unique_ptr<Expr>> args;
    if (!check(TokenType::RPAREN)) {
        do {
            if (auto arg = parseExpression())
                args.push_back(std::move(arg));
            else
                return nullptr;
        } while (match(TokenType::COMMA));
    }

    if (!match(TokenType::RPAREN))
        return nullptr; // Expected ')'

    return std::make_unique<CallExpr>(idName, std::move(args));
}

std::unique_ptr<Expr> Parser::parseNumberExpr() {
    auto result = std::make_unique<NumberExpr>(currentToken().value, currentToken().type);
    advance(); // consume the number
    return std::move(result);
}

std::unique_ptr<Expr> Parser::parseStringExpr() {
    auto result = std::make_unique<StringExpr>(currentToken().value);
    advance(); // consume the string
    return std::move(result);
}

std::unique_ptr<Expr> Parser::parseParenExpr() {
    advance(); // eat '('.
    auto V = parseExpression();
    if (!V)
        return nullptr;

    if (!match(TokenType::RPAREN))
        return nullptr; // expected ')'
    return V;
}

std::unique_ptr<Expr> Parser::parseBoolExpr() {
    bool value = currentToken().value == "true";
    advance(); // consume the boolean
    return std::make_unique<BoolExpr>(value);
}

std::unique_ptr<Expr> Parser::parsePrimary() {
    if (check(TokenType::IDENTIFIER)) return parseIdentifierExpr();
    if (check(TokenType::INT_LITERAL) || check(TokenType::FLOAT_LITERAL)) return parseNumberExpr();
    if (check(TokenType::STRING_LITERAL)) return parseStringExpr();
    if (check(TokenType::BOOL_LITERAL)) return parseBoolExpr();
    if (check(TokenType::LPAREN)) return parseParenExpr();
    return nullptr;
}

std::unique_ptr<Expr> Parser::parseUnary() {
    if (!check(TokenType::BANG)) {
        return parsePrimary();
    }

    std::string op = currentToken().value;
    advance(); // eat operator

    auto operand = parseUnary();
    if (!operand) return nullptr;

    return std::make_unique<UnaryExpr>(op, std::move(operand));
}

std::unique_ptr<Expr> Parser::parseBinOpRHS(int exprPrec, std::unique_ptr<Expr> LHS) {
    while (true) {
        int tokPrec = getTokPrecedence();

        if (tokPrec < exprPrec)
            return LHS;

        std::string binOp = currentToken().value;
        advance(); // eat binop

        auto RHS = parseUnary();
        if (!RHS)
            return nullptr;

        int nextPrec = getTokPrecedence();
        if (tokPrec < nextPrec) {
            RHS = parseBinOpRHS(tokPrec + 1, std::move(RHS));
            if (!RHS)
                return nullptr;
        }

        LHS = std::make_unique<BinaryExpr>(binOp, std::move(LHS), std::move(RHS));
    }
}

std::unique_ptr<Expr> Parser::parseExpression() {
    auto LHS = parseUnary();
    if (!LHS)
        return nullptr;

    return parseBinOpRHS(0, std::move(LHS));
}

std::unique_ptr<Stmt> Parser::parseReturnStmt() {
    advance(); // consume 'return'
    auto value = parseExpression();
    if (!match(TokenType::SEMICOLON)) return nullptr;
    return std::make_unique<ReturnStmt>(std::move(value));
}

std::unique_ptr<Stmt> Parser::parsePrintStmt() {
    advance(); // consume 'print'
    if (!match(TokenType::LPAREN)) return nullptr;

    auto formatExpr = parseStringExpr();
    if (!formatExpr) return nullptr;

    std::vector<std::unique_ptr<Expr>> args;
    while (match(TokenType::COMMA)) {
        auto arg = parseExpression();
        if (!arg) return nullptr;
        args.push_back(std::move(arg));
    }

    if (!match(TokenType::RPAREN)) return nullptr;
    if (!match(TokenType::SEMICOLON)) return nullptr;

    return std::make_unique<PrintStmt>(std::unique_ptr<StringExpr>(static_cast<StringExpr*>(formatExpr.release())), std::move(args));
}

std::unique_ptr<Stmt> Parser::parseScanStmt() {
    advance(); // consume 'scan'
    if (!match(TokenType::LPAREN)) return nullptr;
    if (!check(TokenType::IDENTIFIER)) return nullptr;
    auto var = std::make_unique<VariableExpr>(currentToken().value);
    advance();
    if (!match(TokenType::RPAREN)) return nullptr;
    if (!match(TokenType::SEMICOLON)) return nullptr;
    return std::make_unique<ScanStmt>(std::move(var));
}

std::unique_ptr<Stmt> Parser::parseVarDeclStmt() {
    std::string type = currentToken().value;
    advance(); // consume type

    if (!check(TokenType::IDENTIFIER)) return nullptr;
    std::string name = currentToken().value;
    advance();

    std::unique_ptr<Expr> init = nullptr;
    if (match(TokenType::ASSIGN)) {
        init = parseExpression();
    }

    if (!match(TokenType::SEMICOLON)) return nullptr;
    return std::make_unique<VarDeclStmt>(type, name, std::move(init));
}

std::unique_ptr<Stmt> Parser::parseIfStmt() {
    advance(); // consume 'if'
    if (!match(TokenType::LPAREN)) return nullptr;
    auto condition = parseExpression();
    if (!match(TokenType::RPAREN)) return nullptr;
    auto thenBranch = parseBlock();
    std::unique_ptr<BlockStmt> elseBranch = nullptr;
    if (match(TokenType::ELSE)) {
        elseBranch = parseBlock();
    }
    return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<Stmt> Parser::parseWhileStmt() {
    advance(); // consume 'while'
    if (!match(TokenType::LPAREN)) return nullptr;
    auto condition = parseExpression();
    if (!match(TokenType::RPAREN)) return nullptr;
    auto body = parseBlock();
    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<Stmt> Parser::parseStatement() {
    if (check(TokenType::RETURN)) return parseReturnStmt();
    if (check(TokenType::PRINT)) return parsePrintStmt();
    if (check(TokenType::SCAN)) return parseScanStmt();
    if (isType()) return parseVarDeclStmt();
    if (check(TokenType::IF)) return parseIfStmt();
    if (check(TokenType::WHILE)) return parseWhileStmt();
    return nullptr;
}

std::unique_ptr<BlockStmt> Parser::parseBlock() {
    if (!match(TokenType::LBRACE)) return nullptr;

    auto block = std::make_unique<BlockStmt>();
    while (!check(TokenType::RBRACE) && current < tokens.size()) {
        if(auto stmt = parseStatement()) {
            block->Statements.push_back(std::move(stmt));
        } else {
            // Error, skip the token.
            advance();
        }
    }

    if (!match(TokenType::RBRACE)) return nullptr;
    return block;
}

std::unique_ptr<PrototypeAST> Parser::parsePrototype() {
    if (!match(TokenType::FN)) return nullptr;
    if (!check(TokenType::IDENTIFIER)) return nullptr;
    std::string fnName = currentToken().value;
    advance();

    if (!match(TokenType::LPAREN)) return nullptr;
    std::vector<std::pair<std::string, std::string>> argNames;
    if (!check(TokenType::RPAREN)) {
        do {
            if (!isType()) return nullptr;
            std::string argType = currentToken().value;
            advance();
            if (!check(TokenType::IDENTIFIER)) return nullptr;
            std::string argName = currentToken().value;
            advance();
            argNames.push_back({argType, argName});
        } while (match(TokenType::COMMA));
    }
    if (!match(TokenType::RPAREN)) return nullptr;

    std::string returnType = "void"; // Default return type
    if (match(TokenType::COLON) || match(TokenType::ARROW)) {
        if (!isType()) return nullptr;
        returnType = currentToken().value;
        advance();
    }

    return std::make_unique<PrototypeAST>(fnName, std::move(argNames), returnType);
}

std::unique_ptr<FunctionAST> Parser::parseDefinition() {
    auto proto = parsePrototype();
    if (!proto) return nullptr;

    auto body = parseBlock();
    if (!body) return nullptr;

    return std::make_unique<FunctionAST>(std::move(proto), std::move(body));
}
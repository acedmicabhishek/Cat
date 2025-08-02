#ifndef AST_H
#define AST_H

#include "token.h"
#include <memory>
#include <string>
#include <vector>
#include <map>

// Base class for all expression nodes
struct Expr {
    virtual ~Expr() = default;
};

// Base class for all statement nodes
struct Stmt {
    virtual ~Stmt() = default;
};

// Expression for a number literal
struct NumberExpr : Expr {
    std::string Value;
    TokenType Type;
    NumberExpr(const std::string& value, TokenType type) : Value(value), Type(type) {}
};

// Expression for a string literal
struct StringExpr : Expr {
    std::string Value;
    StringExpr(const std::string& value) : Value(value) {}
};

// Expression for a boolean literal
struct BoolExpr : Expr {
    bool Value;
    BoolExpr(bool value) : Value(value) {}
};

// Expression for a variable
struct VariableExpr : Expr {
    std::string Name;
    VariableExpr(const std::string& name) : Name(name) {}
};

// Expression for a binary operation
struct BinaryExpr : Expr {
    std::string Op;
    std::unique_ptr<Expr> LHS, RHS;
    BinaryExpr(const std::string& op, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs)
        : Op(op), LHS(std::move(lhs)), RHS(std::move(rhs)) {}
};

// Expression for a unary operation
struct UnaryExpr : Expr {
    std::string Op;
    std::unique_ptr<Expr> RHS;
    UnaryExpr(const std::string& op, std::unique_ptr<Expr> rhs)
        : Op(op), RHS(std::move(rhs)) {}
};

// Expression for a function call
struct CallExpr : Expr {
    std::string Callee;
    std::vector<std::unique_ptr<Expr>> Args;
    CallExpr(const std::string& callee, std::vector<std::unique_ptr<Expr>> args)
        : Callee(callee), Args(std::move(args)) {}
};

// Statement for a variable declaration
struct VarDeclStmt : Stmt {
    std::string VarType;
    std::string VarName;
    std::unique_ptr<Expr> Init;
    VarDeclStmt(const std::string& type, const std::string& name, std::unique_ptr<Expr> init)
        : VarType(type), VarName(name), Init(std::move(init)) {}
};

// Statement for a return
struct ReturnStmt : Stmt {
    std::unique_ptr<Expr> Value;
    ReturnStmt(std::unique_ptr<Expr> value) : Value(std::move(value)) {}
};

// Statement for a print call
struct PrintStmt : Stmt {
    std::unique_ptr<StringExpr> Format;
    std::vector<std::unique_ptr<Expr>> Args;
    PrintStmt(std::unique_ptr<StringExpr> format, std::vector<std::unique_ptr<Expr>> args)
        : Format(std::move(format)), Args(std::move(args)) {}
};

// Statement for a scan call
struct ScanStmt : Stmt {
    std::unique_ptr<VariableExpr> Var;
    ScanStmt(std::unique_ptr<VariableExpr> var) : Var(std::move(var)) {}
};

// Forward declaration
struct BlockStmt;

// Statement for an if-else
struct IfStmt : Stmt {
    std::unique_ptr<Expr> Condition;
    std::unique_ptr<BlockStmt> ThenBranch;
    std::unique_ptr<BlockStmt> ElseBranch; // Can be nullptr
    IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<BlockStmt> thenBranch, std::unique_ptr<BlockStmt> elseBranch);
};

// Statement for a while loop
struct WhileStmt : Stmt {
    std::unique_ptr<Expr> Condition;
    std::unique_ptr<BlockStmt> Body;
    WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<BlockStmt> body);
};

// Statement for a block of statements
struct BlockStmt : Stmt {
    std::vector<std::unique_ptr<Stmt>> Statements;
};

// Statement for a function prototype (declaration)
struct PrototypeAST {
    std::string Name;
    std::vector<std::pair<std::string, std::string>> Args; // (type, name)
    std::string ReturnType;
    PrototypeAST(const std::string& name, std::vector<std::pair<std::string, std::string>> args, const std::string& returnType)
        : Name(name), Args(std::move(args)), ReturnType(returnType) {}
};

// Statement for a function definition
struct FunctionAST {
    std::unique_ptr<PrototypeAST> Proto;
    std::unique_ptr<BlockStmt> Body;
    FunctionAST(std::unique_ptr<PrototypeAST> proto, std::unique_ptr<BlockStmt> body);
};

// Top-level module/translation unit
struct ModuleAST {
    std::vector<std::unique_ptr<FunctionAST>> Functions;
};

#endif
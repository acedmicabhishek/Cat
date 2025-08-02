#include "ast.h"

IfStmt::IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<BlockStmt> thenBranch, std::unique_ptr<BlockStmt> elseBranch)
    : Condition(std::move(condition)), ThenBranch(std::move(thenBranch)), ElseBranch(std::move(elseBranch)) {}

WhileStmt::WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<BlockStmt> body)
    : Condition(std::move(condition)), Body(std::move(body)) {}

FunctionAST::FunctionAST(std::unique_ptr<PrototypeAST> proto, std::unique_ptr<BlockStmt> body)
    : Proto(std::move(proto)), Body(std::move(body)) {}
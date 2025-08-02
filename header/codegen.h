#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <map>
#include <memory>

class CodeGen {
public:
    CodeGen();
    void generate(ModuleAST& ast);
    void dump();
    bool writeToFile(const std::string& filename);

private:
    llvm::Value* logErrorV(const char* str);
    llvm::Function* getFunction(std::string name);
    llvm::Type* getType(const std::string& typeName);

    // Expression visitors
    llvm::Value* visit(Expr& ast);
    llvm::Value* visit(NumberExpr& ast);
    llvm::Value* visit(StringExpr& ast);
    llvm::Value* visit(BoolExpr& ast);
    llvm::Value* visit(VariableExpr& ast);
    llvm::Value* visit(BinaryExpr& ast);
    llvm::Value* visit(UnaryExpr& ast);
    llvm::Value* visit(CallExpr& ast);

    // Statement visitors
    void visit(Stmt& ast);
    void visit(BlockStmt& ast);
    void visit(ReturnStmt& ast);
    void visit(PrintStmt& ast);
    void visit(ScanStmt& ast);
    void visit(VarDeclStmt& ast);
    void visit(IfStmt& ast);
    void visit(WhileStmt& ast);

    // Top-level visitors
    llvm::Function* visit(PrototypeAST& ast);
    llvm::Function* visit(FunctionAST& ast);
    void visit(ModuleAST& ast);

    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::Module> module;
    std::map<std::string, llvm::AllocaInst*> namedValues;
};

#endif
#include "codegen.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"

CodeGen::CodeGen() {
    context = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>("CatLang", *context);
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
}

void CodeGen::generate(ModuleAST& ast) {
    visit(ast);
}

void CodeGen::dump() {
    module->print(llvm::outs(), nullptr);
}

bool CodeGen::writeToFile(const std::string& filename) {
    std::error_code EC;
    llvm::raw_fd_ostream dest(filename, EC, llvm::sys::fs::OF_None);

    if (EC) {
        llvm::errs() << "Could not open file: " << EC.message();
        return false;
    }

    module->print(dest, nullptr);
    return true;
}

llvm::Value* CodeGen::logErrorV(const char* str) {
    fprintf(stderr, "Error: %s\n", str);
    return nullptr;
}

llvm::Type* CodeGen::getType(const std::string& typeName) {
    if (typeName == "int") return builder->getInt32Ty();
    if (typeName == "float") return builder->getFloatTy();
    if (typeName == "bool") return builder->getInt1Ty();
    return builder->getVoidTy();
}

llvm::Function* CodeGen::getFunction(std::string name) {
    if (auto* F = module->getFunction(name)) {
        return F;
    }
    if (name == "printf") {
        llvm::Type* charPtrType = llvm::PointerType::get(builder->getInt8Ty(), 0);
        llvm::FunctionType* ft = llvm::FunctionType::get(builder->getInt32Ty(), charPtrType, true);
        return llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "printf", module.get());
    }
    if (name == "scanf") {
        llvm::Type* charPtrType = llvm::PointerType::get(builder->getInt8Ty(), 0);
        llvm::FunctionType* ft = llvm::FunctionType::get(builder->getInt32Ty(), charPtrType, true);
        return llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "scanf", module.get());
    }
    return nullptr;
}

llvm::Value* CodeGen::visit(Expr& ast) {
    if (auto* e = dynamic_cast<NumberExpr*>(&ast)) return visit(*e);
    if (auto* e = dynamic_cast<StringExpr*>(&ast)) return visit(*e);
    if (auto* e = dynamic_cast<BoolExpr*>(&ast)) return visit(*e);
    if (auto* e = dynamic_cast<VariableExpr*>(&ast)) return visit(*e);
    if (auto* e = dynamic_cast<BinaryExpr*>(&ast)) return visit(*e);
    if (auto* e = dynamic_cast<UnaryExpr*>(&ast)) return visit(*e);
    if (auto* e = dynamic_cast<CallExpr*>(&ast)) return visit(*e);
    return logErrorV("Unknown expression type");
}

llvm::Value* CodeGen::visit(NumberExpr& ast) {
    if (ast.Type == TokenType::INT_LITERAL) {
        return llvm::ConstantInt::get(*context, llvm::APInt(32, std::stoll(ast.Value), true));
    } else if (ast.Type == TokenType::FLOAT_LITERAL) {
        return llvm::ConstantFP::get(*context, llvm::APFloat(std::stod(ast.Value)));
    }
    return logErrorV("Unknown number type");
}

llvm::Value* CodeGen::visit(StringExpr& ast) {
    return builder->CreateGlobalString(ast.Value);
}

llvm::Value* CodeGen::visit(BoolExpr& ast) {
    if (ast.Value) {
        return llvm::ConstantInt::getTrue(*context);
    } else {
        return llvm::ConstantInt::getFalse(*context);
    }
}

llvm::Value* CodeGen::visit(VariableExpr& ast) {
    llvm::AllocaInst* a = namedValues[ast.Name];
    if (!a) {
        return logErrorV("Unknown variable name");
    }
    return builder->CreateLoad(a->getAllocatedType(), a, ast.Name.c_str());
}

llvm::Value* CodeGen::visit(BinaryExpr& ast) {
    llvm::Value* L = visit(*ast.LHS);
    llvm::Value* R = visit(*ast.RHS);
    if (!L || !R) {
        return nullptr;
    }

    if (L->getType()->isIntegerTy()) {
        if (ast.Op == "+") return builder->CreateAdd(L, R, "addtmp");
        if (ast.Op == "-") return builder->CreateSub(L, R, "subtmp");
        if (ast.Op == "*") return builder->CreateMul(L, R, "multmp");
        if (ast.Op == "<") return builder->CreateICmpULT(L, R, "cmptmp");
        if (ast.Op == "==") return builder->CreateICmpEQ(L, R, "cmptmp");
        if (ast.Op == "!=") return builder->CreateICmpNE(L, R, "cmptmp");
        if (ast.Op == "<=") return builder->CreateICmpULE(L, R, "cmptmp");
        if (ast.Op == ">=") return builder->CreateICmpUGE(L, R, "cmptmp");
        if (ast.Op == "&&") return builder->CreateAnd(L, R, "andtmp");
        if (ast.Op == "||") return builder->CreateOr(L, R, "ortmp");
    } else {
        if (ast.Op == "+") return builder->CreateFAdd(L, R, "addtmp");
        if (ast.Op == "-") return builder->CreateFSub(L, R, "subtmp");
        if (ast.Op == "*") return builder->CreateFMul(L, R, "multmp");
        if (ast.Op == "<") return builder->CreateFCmpULT(L, R, "cmptmp");
        if (ast.Op == "==") return builder->CreateFCmpUEQ(L, R, "cmptmp");
        if (ast.Op == "!=") return builder->CreateFCmpUNE(L, R, "cmptmp");
        if (ast.Op == "<=") return builder->CreateFCmpULE(L, R, "cmptmp");
        if (ast.Op == ">=") return builder->CreateFCmpUGE(L, R, "cmptmp");
    }
    return logErrorV("invalid binary operator");
}

llvm::Value* CodeGen::visit(UnaryExpr& ast) {
    llvm::Value* operand = visit(*ast.RHS);
    if (!operand) return nullptr;

    if (ast.Op == "!") {
        return builder->CreateNot(operand, "nottmp");
    }

    return logErrorV("invalid unary operator");
}

llvm::Value* CodeGen::visit(CallExpr& ast) {
    llvm::Function* calleeF = getFunction(ast.Callee);
    if (!calleeF) {
        return logErrorV("Unknown function referenced");
    }

    if (calleeF->isVarArg() == false && calleeF->arg_size() != ast.Args.size()) {
        return logErrorV("Incorrect # arguments passed");
    }

    std::vector<llvm::Value*> argsV;
    for (unsigned i = 0, e = ast.Args.size(); i != e; ++i) {
        argsV.push_back(visit(*ast.Args[i]));
        if (!argsV.back()) {
            return nullptr;
        }
    }

    return builder->CreateCall(calleeF, argsV, "calltmp");
}

void CodeGen::visit(Stmt& ast) {
    if (auto* s = dynamic_cast<BlockStmt*>(&ast)) return visit(*s);
    if (auto* s = dynamic_cast<ReturnStmt*>(&ast)) return visit(*s);
    if (auto* s = dynamic_cast<PrintStmt*>(&ast)) return visit(*s);
    if (auto* s = dynamic_cast<ScanStmt*>(&ast)) return visit(*s);
    if (auto* s = dynamic_cast<VarDeclStmt*>(&ast)) return visit(*s);
    if (auto* s = dynamic_cast<IfStmt*>(&ast)) return visit(*s);
    if (auto* s = dynamic_cast<WhileStmt*>(&ast)) return visit(*s);
}

void CodeGen::visit(BlockStmt& ast) {
    for (auto& stmt : ast.Statements) {
        visit(*stmt);
    }
}

void CodeGen::visit(ReturnStmt& ast) {
    builder->CreateRet(visit(*ast.Value));
}

void CodeGen::visit(PrintStmt& ast) {
    llvm::Function* printfFn = getFunction("printf");
    std::vector<llvm::Value*> args;
    args.push_back(builder->CreateGlobalString(ast.Format->Value));
    for (auto& arg : ast.Args) {
        args.push_back(visit(*arg));
    }
    builder->CreateCall(printfFn, args);
}

void CodeGen::visit(ScanStmt& ast) {
    llvm::AllocaInst* alloca = namedValues[ast.Var->Name];
    if (!alloca) {
        logErrorV("Unknown variable name in scan");
        return;
    }
    llvm::Function* scanfFn = getFunction("scanf");
    llvm::Value* formatStr = builder->CreateGlobalString("%d");
    builder->CreateCall(scanfFn, {formatStr, alloca});
}

void CodeGen::visit(VarDeclStmt& ast) {
    llvm::Function* theFunction = builder->GetInsertBlock()->getParent();
    llvm::IRBuilder<> TmpB(&theFunction->getEntryBlock(), theFunction->getEntryBlock().begin());
    llvm::AllocaInst* alloca = TmpB.CreateAlloca(getType(ast.VarType), 0, ast.VarName.c_str());

    if (ast.Init) {
        llvm::Value* initVal = visit(*ast.Init);
        builder->CreateStore(initVal, alloca);
    }

    namedValues[ast.VarName] = alloca;
}

void CodeGen::visit(IfStmt& ast) {
    llvm::Value* condV = visit(*ast.Condition);
    if (!condV) return;

    condV = builder->CreateICmpNE(condV, llvm::ConstantInt::get(builder->getInt1Ty(), 0, true), "ifcond");

    llvm::Function* theFunction = builder->GetInsertBlock()->getParent();
    llvm::BasicBlock* thenBB = llvm::BasicBlock::Create(*context, "then", theFunction);
    llvm::BasicBlock* elseBB = llvm::BasicBlock::Create(*context, "else", theFunction);
    llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(*context, "ifcont", theFunction);

    builder->CreateCondBr(condV, thenBB, elseBB);

    builder->SetInsertPoint(thenBB);
    visit(*ast.ThenBranch);
    builder->CreateBr(mergeBB);

    builder->SetInsertPoint(elseBB);
    if (ast.ElseBranch) {
        visit(*ast.ElseBranch);
    }
    builder->CreateBr(mergeBB);

    builder->SetInsertPoint(mergeBB);
}

void CodeGen::visit(WhileStmt& ast) {
    llvm::Function* theFunction = builder->GetInsertBlock()->getParent();
    llvm::BasicBlock* loopBB = llvm::BasicBlock::Create(*context, "loop", theFunction);
    llvm::BasicBlock* afterBB = llvm::BasicBlock::Create(*context, "afterloop");

    builder->CreateBr(loopBB);
    builder->SetInsertPoint(loopBB);

    llvm::Value* condV = visit(*ast.Condition);
    condV = builder->CreateICmpNE(condV, llvm::ConstantInt::get(builder->getInt1Ty(), 0, true), "loopcond");

    builder->CreateCondBr(condV, loopBB, afterBB);

    visit(*ast.Body);
    builder->CreateBr(loopBB);

    builder->SetInsertPoint(afterBB);
}

llvm::Function* CodeGen::visit(PrototypeAST& ast) {
    std::vector<llvm::Type*> argTypes;
    llvm::Type* returnType = getType(ast.ReturnType);

    if (ast.Name == "main") {
        // Force main to have the standard C signature
        argTypes.push_back(builder->getInt32Ty()); // argc
        argTypes.push_back(llvm::PointerType::get(builder->getInt8Ty()->getPointerTo(), 0)); // argv
    } else {
        for (const auto& arg : ast.Args) {
            argTypes.push_back(getType(arg.first));
        }
    }

    llvm::FunctionType* ft = llvm::FunctionType::get(returnType, argTypes, false);
    llvm::Function* f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, ast.Name, module.get());

    if (ast.Name == "main") {
        f->getArg(0)->setName("argc");
        f->getArg(1)->setName("argv");
    } else {
        unsigned idx = 0;
        for (auto& arg : f->args()) {
            arg.setName(ast.Args[idx++].second);
        }
    }

    return f;
}

llvm::Function* CodeGen::visit(FunctionAST& ast) {
    llvm::Function* theFunction = getFunction(ast.Proto->Name);
    if (!theFunction) {
        theFunction = visit(*ast.Proto);
    }

    if (!theFunction) {
        return nullptr;
    }

    llvm::BasicBlock* BB = llvm::BasicBlock::Create(*context, "entry", theFunction);
    builder->SetInsertPoint(BB);

    namedValues.clear();
    for (auto& arg : theFunction->args()) {
        llvm::AllocaInst* alloca = builder->CreateAlloca(arg.getType(), 0, arg.getName());
        builder->CreateStore(&arg, alloca);
        namedValues[std::string(arg.getName())] = alloca;
    }

    visit(*ast.Body);

    if (theFunction->getReturnType()->isVoidTy()) {
        builder->CreateRetVoid();
    }

    llvm::verifyFunction(*theFunction);
    return theFunction;
}

void CodeGen::visit(ModuleAST& ast) {
    // First pass: create function declarations.
    for (auto& func : ast.Functions) {
        visit(*func->Proto);
    }
    // Second pass: generate function bodies.
    for (auto& func : ast.Functions) {
        visit(*func);
    }
}

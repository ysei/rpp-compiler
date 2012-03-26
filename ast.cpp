#include <llvm/Module.h>
#include <llvm/LLVMContext.h>

#include "ast.h"

CodeGenContext::CodeGenContext()
{
    module = new llvm::Module("main", llvm::getGlobalContext());
}

llvm::Value *IdentifierNode::codeGen(CodeGenContext &context)
{
    return NULL;
}

llvm::Value *VariableDeclaration::codeGen(CodeGenContext &context)
{
    return NULL;
}

llvm::Value *MethodDeclaration::codeGen(CodeGenContext &context)
{
    return NULL;
}

llvm::Value *BlockStatement::codeGen(CodeGenContext &context)
{
    return NULL;
}


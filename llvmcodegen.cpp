#include <llvm/Module.h>
#include <llvm/Instructions.h>
#include <llvm/LLVMContext.h>
#include <llvm/PassManager.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "llvmcodegen.h"

using namespace llvm;

LLVMCodeGen::LLVMCodeGen()
{

}

LLVMCodeGen::~LLVMCodeGen()
{
}

void LLVMCodeGen::visit(Program *node)
{
}

void LLVMCodeGen::visit(ASTNode *node)
{
}

void LLVMCodeGen::visit(ExpressionNode *node)
{
}

void LLVMCodeGen::visit(StatementNode *node)
{
}

void LLVMCodeGen::visit(FloatNode *node)
{
}

void LLVMCodeGen::visit(IntegerNode *node)
{
}

void LLVMCodeGen::visit(IdentifierNode *node)
{
}

void LLVMCodeGen::visit(BinaryOpExpression *node)
{
}

void LLVMCodeGen::visit(MethodCallExpression *node)
{
}

void LLVMCodeGen::visit(BlockStatement *node)
{
}

void LLVMCodeGen::visit(VariableDeclaration *node)
{
}

void LLVMCodeGen::visit(MethodDeclaration *node)
{
}

void LLVMCodeGen::visit(ReturnStatement *node)
{
}

void LLVMCodeGen::visit(AssignmentExpression *node)
{
}

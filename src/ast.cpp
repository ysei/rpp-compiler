#include <llvm/Module.h>
#include <llvm/Instructions.h>
#include <llvm/LLVMContext.h>
#include <llvm/PassManager.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "ast.h"

using namespace llvm;
using namespace std;

static Type * getType(const string & typeString) {
    if(typeString == string("int")) {
        return Type::getInt32Ty(getGlobalContext());
    } else if(typeString == string("float")) {
        return Type::getFloatTy(getGlobalContext());
    } else if(typeString == string("void")) {
        return Type::getVoidTy(getGlobalContext());
    }

    cerr << "Requested unknown type: " << typeString << endl;

    return NULL;
}

static Type * getType(ExpressionNode::Type expressionType) {
    switch(expressionType) {
    case ExpressionNode::Int:
        return Type::getInt32Ty(getGlobalContext());
    case ExpressionNode::Float:
        return Type::getFloatTy(getGlobalContext());

    default:
        cerr << "getType - unknown type" << endl;
    }

    return Type::getVoidTy(getGlobalContext());
}

static ExpressionNode::Type getExpressionNodeType(llvm::Type * type)
{
    if(type->isIntegerTy()) {
        return ExpressionNode::Int;
    } else if(type->isFloatTy()) {
        return ExpressionNode::Float;
    }

    return ExpressionNode::Invalid;
}

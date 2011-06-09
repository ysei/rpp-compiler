#include <iostream>
#include <vector>
#include <llvm/ExecutionEngine/JIT.h>
#include "ast.hpp"
#include "rppLexer.h"

using namespace std;

void CodeGenContext::generateCode(NBlock& root)
{
    cout << "Generating code..." << endl;

    vector<const Type*> argTypes;
    FunctionType *ftype = FunctionType::get(Type::getInt32Ty(getGlobalContext()), argTypes, false);
    mainFunction = Function::Create(ftype, GlobalValue::InternalLinkage, "main", module);
    BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", mainFunction, 0);
    builder->SetInsertPoint(bblock);

    pushBlock(bblock);
    Value * returnedValue = root.codeGen(*this);
    builder->CreateRet(returnedValue);
    popBlock();

    // Print bytecode
    cout << "Code is generated." << endl;
    PassManager pm;
    pm.add(createPrintModulePass(&outs()));
    pm.run(*module);
}

typedef int (*mainFuncType)();
GenericValue CodeGenContext::runCode()
{
    cout << "Running code..." << endl;
    string error;
    ExecutionEngine *ee = EngineBuilder(module).setErrorStr(&error).setEngineKind(EngineKind::JIT).create();
    if(!error.empty()) {
        cout << "Can't run the code. Reason: " << error << endl;
    } else {
        void * p = ee->getPointerToFunction(mainFunction);
        mainFuncType func = (mainFuncType)(p);
        cout << "Code was run. Return value: " << func() << endl;
    }
    return GenericValue();
}

Value * NInteger::codeGen(CodeGenContext& context)
{
    cout << "Creating integer: " << value << endl;
    return ConstantInt::get(getGlobalContext(), APInt(32, value));
}

Value * NDouble::codeGen(CodeGenContext& context)
{
    cout << "Creating double: " << value << endl;
    return ConstantFP::get(getGlobalContext(), APFloat(value));
}

Value * NBinaryOperator::codeGen(CodeGenContext& context)
{
    cout << "Creating binary operation " << op << endl;

/*
    switch (op) {
        case TPLUS:
            return context.builder->CreateAdd(lhs.codeGen(context), rhs.codeGen(context));
            break;
        case TMINUS:
            return context.builder->CreateSub(lhs.codeGen(context), rhs.codeGen(context));
        case TMUL:
            return context.builder->CreateMul(lhs.codeGen(context), rhs.codeGen(context));
        case TDIV:
            return context.builder->CreateSDiv(lhs.codeGen(context), rhs.codeGen(context));
        default:
            assert(false);
    }
*/
    return NULL;
}

Value * NBlock::codeGen(CodeGenContext& context)
{
    StatementList::const_iterator it;
    Value *last = NULL;
    for (it = statements.begin(); it != statements.end(); it++) {
        cout << "Generating code for " << "some statement" << endl;
        last = (**it).codeGen(context);
    }
    cout << "Creating block" << endl;
    return last;
}

Value * NExpressionStatement::codeGen(CodeGenContext& context)
{
    cout << "Generating code for expression" << endl;
    return expression.codeGen(context);
}

Value * NIfStatement::codeGen(CodeGenContext& context)
{
    cout << "Generating code for if" << endl;
    Value * exprValue = expr.codeGen(context);
    if(!exprValue)
        return NULL;

    exprValue = context.builder->CreateICmpEQ(ConstantInt::get(getGlobalContext(), APInt(32, 0)), exprValue, "ifcond");

    Function * function = context.builder->GetInsertBlock()->getParent();
    assert(function);
    BasicBlock * thenBlock = BasicBlock::Create(getGlobalContext(), "then", function);
    BasicBlock * elseBlock = BasicBlock::Create(getGlobalContext(), "else", function);
    BasicBlock * mergeBlock = BasicBlock::Create(getGlobalContext(), "ifcond", function);

    context.builder->CreateCondBr(exprValue, thenBlock, elseBlock);
    context.builder->SetInsertPoint(thenBlock);

    Value * thenValue = thenStmt.codeGen(context);
    if(!thenValue)
        return NULL;

    context.builder->CreateBr(mergeBlock);
    thenBlock = context.builder->GetInsertBlock();

    // else
    function->getBasicBlockList().push_back(elseBlock);
    context.builder->SetInsertPoint(elseBlock);
    Value * elseValue = elseStmt.codeGen(context);
    if(!elseValue)
        return NULL;
    context.builder->CreateBr(mergeBlock);
    elseBlock = context.builder->GetInsertBlock();

    function->getBasicBlockList().push_back(mergeBlock);
    context.builder->SetInsertPoint(mergeBlock);

    PHINode * phiNode = context.builder->CreatePHI(Type::getInt32Ty(getGlobalContext()), "iftmp");
    phiNode->addIncoming(thenValue, thenBlock);
    phiNode->addIncoming(elseValue, elseBlock);
    return phiNode;
}

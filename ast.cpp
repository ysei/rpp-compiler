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

CodeGenContext::CodeGenContext()
{
    m_module = new llvm::Module("main", llvm::getGlobalContext());
}

Module * CodeGenContext::module()
{
    return m_module;
}

void CodeGenContext::printAssembly()
{
    PassManager pm;
#ifdef OPTIMIZE_GENERATED_CODE
    pm.add(createCFGSimplificationPass());
    pm.add(createInstructionCombiningPass());
    pm.add(createPromoteMemoryToRegisterPass());
#endif
    pm.add(createPrintModulePass(&outs()));
    pm.run(*m_module);
}

llvm::Value *IdentifierNode::codeGen(CodeGenContext &context)
{
    cout << "Generating code for IdentifierNode" << endl;
    Value * val = context.locals()[name()];
    if(val) {
        cout << "  Using local variable: " << name() << endl;
        setType(getExpressionNodeType(val->getType())); // TODO this should happen before code generation
        return val;
    }

    val = context.arguments()[name()];
    if(val) {
        cout << "  Using argument: " << name() << endl;
        setType(getExpressionNodeType(val->getType())); // TODO this should happen before code generation
        return val;
    }

    cout << "  Can't find identifier: " << name();
    return NULL;
}

llvm::Value *VariableDeclaration::codeGen(CodeGenContext &context)
{
    std::cout << "Generating code for VariableDeclaration" << std::endl;
    return NULL;
}

llvm::Value *MethodDeclaration::codeGen(CodeGenContext &context)
{
    cout << "Generating code for MethodDeclaration" << std::endl;
    cout << "  Name: " << m_name->name() << endl;
    cout << "  Return: " << m_returnType->name() << endl;

    Type * retType = getType(m_returnType->name());

    vector<Type *> argTypes;
    cout << "  Params:" << endl;
    for(vector<VariableDeclaration *>::const_iterator iter = m_arguments.begin(); iter != m_arguments.end(); iter++) {
        IdentifierNode * varType = (*iter)->varType();
        argTypes.push_back(getType(varType->name().c_str()));
        cout << "    Adding " << varType->name() << endl;
    }

    FunctionType * functionType = FunctionType::get(retType, makeArrayRef(argTypes), false);
    Function * function = Function::Create(functionType, GlobalValue::InternalLinkage, m_name->name(), context.module());

    BasicBlock * basicBlock = BasicBlock::Create(getGlobalContext(), "entry", function, 0);

    context.pushBlock(basicBlock);

    int argIndex = 0;
    for(Function::arg_iterator args = function->arg_begin(); args != function->arg_end(); args++, argIndex++) {
        Value * arg = args;
        arg->setName(m_arguments[argIndex]->varName()->name());

        context.arguments()[arg->getName()] = arg;
    }


    if(m_block) {
        m_block->codeGen(context);
    }

    context.popBlock();

    cout << "Code generation is done" << endl << endl;
    return function;
}

llvm::Value *BlockStatement::codeGen(CodeGenContext &context)
{
    std::cout << "Generating code for BlockStatement" << std::endl;
    for(vector<ASTNode *>::const_iterator iter = statements.begin(); iter != statements.end(); iter++) {
        (*iter)->codeGen(context);
    }

    return NULL;
}

llvm::Value *ReturnStatement::codeGen(CodeGenContext &context)
{
}

#include <llvm/Module.h>
#include <llvm/LLVMContext.h>
#include <llvm/PassManager.h>
#include <llvm/Assembly/PrintModulePass.h>
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
    pm.add(createPrintModulePass(&outs()));
    pm.run(*m_module);
}

llvm::Value *IdentifierNode::codeGen(CodeGenContext &context)
{
    std::cout << "Generating code for IdentifierNode" << std::endl;
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
    cout << "  Name: " << name->idName() << endl;
    cout << "  Return: " << returnType->idName() << endl;

    Type * retType = getType(returnType->idName());

    vector<Type *> argTypes;
    cout << "  Params:" << endl;
    for(vector<VariableDeclaration *>::const_iterator iter = arguments.begin(); iter != arguments.end(); iter++) {
         IdentifierNode * varType = (*iter)->varType();
         argTypes.push_back(getType(varType->idName().c_str()));
         cout << "    Adding " << varType->idName() << endl;
    }

    FunctionType * functionType = FunctionType::get(retType, makeArrayRef(argTypes), false);
    Function * function = Function::Create(functionType, GlobalValue::InternalLinkage, name->idName(), context.module());
    BasicBlock * basicBlock = BasicBlock::Create(getGlobalContext(), "entry", function, 0);

    context.pushBlock(basicBlock);

    return function;
}

llvm::Value *BlockStatement::codeGen(CodeGenContext &context)
{
    std::cout << "Generating code for BlockStatement" << std::endl;
    return NULL;
}

llvm::Value *ReturnStatement::codeGen(CodeGenContext &context)
{
    std::cout << "Generating code for ReturnStatement" << std::endl;
    return NULL;
}

llvm::Value *AssignmentExpression::codeGen(CodeGenContext &context)
{
    std::cout << "Generating code for AssignmentExpression" << std::endl;
    return NULL;
}

llvm::Value *FloatNode::codeGen(CodeGenContext &context)
{
    std::cout << "Generating code for FloatNode" << std::endl;
    return NULL;
}

llvm::Value *IntegerNode::codeGen(CodeGenContext &context)
{
    std::cout << "Generating code for IntegerNode" << std::endl;
    return NULL;
}

llvm::Value *BinaryOpExpression::codeGen(CodeGenContext &context)
{
    std::cout << "Generating code for BinaryOpExpression" << std::endl;
    return NULL;
}

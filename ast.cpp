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
    Value * val = context.locals()[idName()];
    if(val) {
        cout << "  Using local variable: " << idName() << endl;
        setType(getExpressionNodeType(val->getType())); // TODO this should happen before code generation
        return val;
    }

    val = context.arguments()[idName()];
    if(val) {
        cout << "  Using argument: " << idName() << endl;
        setType(getExpressionNodeType(val->getType())); // TODO this should happen before code generation
        return val;
    }

    cout << "  Can't find identifier: " << idName();
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

    int argIndex = 0;
    for(Function::arg_iterator args = function->arg_begin(); args != function->arg_end(); args++, argIndex++) {
        Value * arg = args;
        arg->setName(arguments[argIndex]->varName()->idName());

        context.arguments()[arg->getName()] = arg;
    }


    if(block) {
        block->codeGen(context);
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
    cout << "Generating code for ReturnStatement" << endl;

    Value * expr = NULL;

    if(expression) {
        cout << "  Return expression" << endl;
        expr = expression->codeGen(context);
    }

    return ReturnInst::Create(getGlobalContext(), expr, context.currentBlock());
}

llvm::Value *AssignmentExpression::codeGen(CodeGenContext &context)
{
    std::cout << "Generating code for AssignmentExpression" << std::endl;
    Value * existingVar = context.locals()[id->idName()];
    Value * expr = rightExpression->codeGen(context);
    if(!existingVar) {
        cout << "  Creating variable " << id->idName() << endl;
        cout << "    Detected type: " << rightExpression->typeString() << endl;

        llvm::Type * varType;
        if(rightExpression->type() != ExpressionNode::Invalid) {
            varType = getType(rightExpression->type());
        } else {
            cerr << "Can't figure out type of expression!!" << endl;
            varType = getType("int");
        }

        existingVar = new AllocaInst(varType, id->idName(), context.currentBlock());
        context.locals()[id->idName()] = existingVar;
    }

    return new StoreInst(expr, existingVar, false, context.currentBlock());
}

llvm::Value *FloatNode::codeGen(CodeGenContext &context)
{
    cout << "Generating code for FloatNode" << value << endl;

    return ConstantFP::get(llvm::Type::getFloatTy(getGlobalContext()), value);
}

llvm::Value *IntegerNode::codeGen(CodeGenContext &context)
{
    cout << "Generating code for IntegerNode " << value << endl;

    return ConstantInt::get(llvm::Type::getInt32Ty(getGlobalContext()), value, true);
}

llvm::Value *BinaryOpExpression::codeGen(CodeGenContext &context)
{
    std::cout << "Generating code for BinaryOpExpression" << std::endl;
    Value * left = leftExpression->codeGen(context);
    Value * right = rightExpression->codeGen(context);

    // TODO This should be done before code generation
    setType(combineTypes(leftExpression->type(), rightExpression->type()));

    Instruction::BinaryOps instr;

    if(op == "+") {
        instr = Instruction::Add;
    } else if(op == "-") {
        instr = Instruction::Sub;
    } else if(op == "*") {
        instr = Instruction::Mul;
    } else if(op == "/") {
        instr = Instruction::SDiv;
    }

    return BinaryOperator::Create(instr, left, right, "tmp", context.currentBlock());
}

Value *Program::codeGen(CodeGenContext &context)
{
    for(std::vector<MethodDeclaration *>::const_iterator iter = m_methods.begin(); iter != m_methods.end(); iter++) {
        (*iter)->codeGen(context);
    }

    return NULL;
}

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
    cout << "Generating code for ReturnStatement" << endl;

    Value * expr = NULL;

    if(m_expression) {
        cout << "  Return expression" << endl;
        expr = m_expression->codeGen(context);
    }

    return ReturnInst::Create(getGlobalContext(), expr, context.currentBlock());
}

llvm::Value *AssignmentExpression::codeGen(CodeGenContext &context)
{
    std::cout << "Generating code for AssignmentExpression" << std::endl;
    Value * existingVar = context.locals()[m_id->name()];
    Value * expr = m_rightExpression->codeGen(context);
    if(!existingVar) {
        cout << "  Creating variable " << m_id->name() << endl;
        cout << "    Detected type: " << m_rightExpression->typeString() << endl;

        llvm::Type * varType;
        if(m_rightExpression->type() != ExpressionNode::Invalid) {
            varType = getType(m_rightExpression->type());
        } else {
            cerr << "Can't figure out type of expression!!" << endl;
            varType = getType("int");
        }

        existingVar = new AllocaInst(varType, m_id->name(), context.currentBlock());
        context.locals()[m_id->name()] = existingVar;
    }

    return new StoreInst(expr, existingVar, false, context.currentBlock());
}

llvm::Value *FloatNode::codeGen(CodeGenContext &context)
{
    cout << "Generating code for FloatNode" << m_value << endl;

    return ConstantFP::get(llvm::Type::getFloatTy(getGlobalContext()), m_value);
}

llvm::Value *IntegerNode::codeGen(CodeGenContext &context)
{
    cout << "Generating code for IntegerNode " << m_value << endl;

    return ConstantInt::get(llvm::Type::getInt32Ty(getGlobalContext()), m_value, true);
}

llvm::Value *BinaryOpExpression::codeGen(CodeGenContext &context)
{
    std::cout << "Generating code for BinaryOpExpression" << std::endl;
    Value * left = m_leftExpression->codeGen(context);
    Value * right = m_rightExpression->codeGen(context);

    // TODO This should be done before code generation
    setType(combineTypes(m_leftExpression->type(), m_rightExpression->type()));

    Instruction::BinaryOps instr;

    if(m_op == "+") {
        instr = Instruction::Add;
    } else if(m_op == "-") {
        instr = Instruction::Sub;
    } else if(m_op == "*") {
        instr = Instruction::Mul;
    } else if(m_op == "/") {
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

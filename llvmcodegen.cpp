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

#include "llvmcodegen.h"

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

LLVMCodeGen::LLVMCodeGen()
{
    m_module = new llvm::Module("main", llvm::getGlobalContext());
}

LLVMCodeGen::~LLVMCodeGen()
{
}

void LLVMCodeGen::visitEnter(Program *node)
{

}

void LLVMCodeGen::visitExit(Program *node)
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
    cout << "Generating code for FloatNode " << node->value() << endl;
    Value * value = ConstantFP::get(llvm::Type::getFloatTy(getGlobalContext()), node->value());
    m_valuesStack.push(value);
}

void LLVMCodeGen::visit(IntegerNode *node)
{
    cout << "Generating code for IntegerNode " << node->value() << endl;
    Value * value = ConstantInt::get(llvm::Type::getInt32Ty(getGlobalContext()), node->value(), true);
    m_valuesStack.push(value);
}

void LLVMCodeGen::visit(IdentifierNode *node)
{
    cout << "Generating code for IdentifierNode" << endl;
    Value * value = locals()[node->name()];
    if(value) {
        cout << "  Using local variable: " << node->name() << endl;
    } else {
        value = arguments()[node->name()];
        if(value) {
            cout << "  Using argument: " << node->name() << endl;
        }
    }

    if(value) {
        push(value);
    } else {
        cerr << "Can't find identifier: " << node->name() << endl;
    }
}

void LLVMCodeGen::visit(BinaryOpExpression *node)
{
    std::cout << "Generating code for BinaryOpExpression" << std::endl;

    Value * right = castIfNeeded(pop(), node->right()->type(), node->type());
    Value * left = castIfNeeded(pop(), node->left()->type(), node->type());

    Instruction::BinaryOps instr = getOperator(node->op(), node->type());

    cout << "BinaryOpExpression type: " << node->typeString() << endl;

    Value * result = BinaryOperator::Create(instr, left, right, "", currentBlock());
    m_valuesStack.push(result);
}

void LLVMCodeGen::visit(MethodCallExpression *node)
{

}

void LLVMCodeGen::visit(BlockStatement *node)
{
    // TODO locals management should happen here
}

void LLVMCodeGen::visit(VariableDeclaration *node)
{
}

void LLVMCodeGen::visitEnter(MethodDeclaration *node)
{
    cout << "Generating code for MethodDeclaration" << std::endl;
    cout << "  Name: " << node->name() << endl;
    cout << "  Return: " << ExpressionNode::toString(node->returnType()) << endl;

    Type * retType = getType(node->returnType());

    vector<Type *> argTypes = generateArgTypes(node->arguments().begin(), node->arguments().end());

    FunctionType * functionType = FunctionType::get(retType, makeArrayRef(argTypes), false);
    Function * function = Function::Create(functionType, GlobalValue::InternalLinkage, node->name(), module());

    BasicBlock * basicBlock = BasicBlock::Create(getGlobalContext(), "entry", function, 0);

    pushBlock(basicBlock);

    int argIndex = 0;
    for(Function::arg_iterator args = function->arg_begin(); args != function->arg_end(); args++, argIndex++) {
        Value * arg = args;
        arg->setName(node->arguments()[argIndex]->varName()->name());

        arguments()[arg->getName()] = arg;
    }

    cout << "Code generation is done" << endl << endl;
}

void LLVMCodeGen::visitExit(MethodDeclaration *node)
{
    popBlock();
}

void LLVMCodeGen::visit(ReturnStatement *node)
{
    cout << "Generating code for ReturnStatement" << endl;
    Value * retValue = castIfNeeded(pop(), node->expression()->type(), node->returnType());

    ReturnInst::Create(getGlobalContext(), retValue, currentBlock());
}

void LLVMCodeGen::visit(AssignmentExpression *node)
{
    std::cout << "Generating code for AssignmentExpression" << std::endl;
    Value * existingVar = locals()[node->id()->name()];
    Value * expr = pop();

    if(!existingVar) {
        cout << "  Generating variable " << node->id()->name() << endl;
        cout << "    Type: " << node->id()->typeString() << endl;

        existingVar = new AllocaInst(getType(node->id()->type()), node->id()->name(), currentBlock());
        locals()[node->id()->name()] = existingVar;
    }

    new StoreInst(expr, existingVar, false, currentBlock());
}

void LLVMCodeGen::printAssembly()
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

std::map<std::string, llvm::Value *> &LLVMCodeGen::locals()
{
    return m_blocks.top()->locals;
}

std::map<std::string, llvm::Value *> &LLVMCodeGen::arguments()
{
    return m_blocks.top()->arguments;
}

llvm::BasicBlock *LLVMCodeGen::currentBlock()
{
    return m_blocks.top()->block;
}

void LLVMCodeGen::pushBlock(llvm::BasicBlock *block)
{
    m_blocks.push(new CodeGenBlock());
    m_blocks.top()->block = block;
}

void LLVMCodeGen::popBlock()
{
    CodeGenBlock * top = m_blocks.top();
    m_blocks.pop();
    delete top;
}

vector<Type *> LLVMCodeGen::generateArgTypes(vector<VariableDeclaration *>::const_iterator start, vector<VariableDeclaration *>::const_iterator end)
{
    vector<Type *> argTypes;

    for(vector<VariableDeclaration *>::const_iterator& iter = start; iter != end; iter++) {
        IdentifierNode * varType = (*iter)->varType();
        argTypes.push_back(getType(varType->name().c_str()));
        cout << "    Adding " << varType->name() << endl;
    }

    return argTypes;
}

Instruction::BinaryOps LLVMCodeGen::getOperator(const string &opString, ExpressionNode::Type type)
{
    Instruction::BinaryOps instr;

    if(type == ExpressionNode::Float) {
        instr = BinaryOpFactory::getFloatOperator(opString);
    } else if(type == ExpressionNode::Int) {
        instr = BinaryOpFactory::getIntOperator(opString);
    }

    return instr;
}

Value *LLVMCodeGen::castIfNeeded(Value *value, ExpressionNode::Type sourceTypeOfValue, ExpressionNode::Type targetTypeOfValue)
{
    Value * targetValue = value;

    if(sourceTypeOfValue == targetTypeOfValue)
        return targetValue;

    if(sourceTypeOfValue == ExpressionNode::Int) {
        if(targetTypeOfValue == ExpressionNode::Float) {
            targetValue = new SIToFPInst(value, Type::getFloatTy(getGlobalContext()), "", currentBlock());
        } else {
            cerr << "LLVMCodeGen::castIfNeeded Can't cast one type to another" << endl;
        }
    } else if(sourceTypeOfValue == ExpressionNode::Float) {
        if(targetTypeOfValue == ExpressionNode::Int) {
            targetValue = new FPToSIInst(value, getType(targetTypeOfValue), "casted", currentBlock());
        } else {
            cerr << "LLVMCodeGen::castIfNeeded Can't cast one type to another" << endl;
        }
    } else {
        cerr << "LLVMCodeGen::castIfNeeded Can't cast one type to another" << endl;
    }

    return targetValue;
}

void LLVMCodeGen::push(Value *value)
{
    m_valuesStack.push(value);
}

Value *LLVMCodeGen::pop()
{
    Value * res = m_valuesStack.top();
    m_valuesStack.pop();

    return res;
}

llvm::Module *LLVMCodeGen::module()
{
    return m_module;
}

Instruction::BinaryOps BinaryOpFactory::getIntOperator(const string &opString)
{
    Instruction::BinaryOps instr;

    if(opString == "+") {
        instr = Instruction::Add;
    } else if(opString == "-") {
        instr = Instruction::Sub;
    } else if(opString == "*") {
        instr = Instruction::Mul;
    } else if(opString == "/") {
        instr = Instruction::SDiv;
    }

    return instr;
}

Instruction::BinaryOps BinaryOpFactory::getFloatOperator(const string &opString)
{
    Instruction::BinaryOps instr;

    if(opString == "+") {
        instr = Instruction::FAdd;
    } else if(opString == "-") {
        instr = Instruction::FSub;
    } else if(opString == "*") {
        instr = Instruction::FMul;
    } else if(opString == "/") {
        instr = Instruction::FDiv;
    }

    return instr;
}

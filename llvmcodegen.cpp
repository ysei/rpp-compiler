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
    Value * value = ConstantInt::get(llvm::Type::getFloatTy(getGlobalContext()), node->value(), true);
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

    // TODO We should cast here if types do not match
    Value * right = pop();
    Value * left = pop();

    Instruction::BinaryOps instr;
    string op = node->op();
    if(op == "+") {
        instr = Instruction::Add;
    } else if(op == "-") {
        instr = Instruction::Sub;
    } else if(op == "*") {
        instr = Instruction::Mul;
    } else if(op == "/") {
        instr = Instruction::SDiv;
    }

    Value * result = BinaryOperator::Create(instr, left, right, "tmp", currentBlock());
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

    vector<Type *> argTypes;
    cout << "  Params:" << endl;

    for(vector<VariableDeclaration *>::const_iterator iter = node->arguments().begin(); iter != node->arguments().end(); iter++) {
        IdentifierNode * varType = (*iter)->varType();
        argTypes.push_back(getType(varType->name().c_str()));
        cout << "    Adding " << varType->name() << endl;
    }

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

    Value * expr = pop();
    ReturnInst::Create(getGlobalContext(), expr, currentBlock());
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

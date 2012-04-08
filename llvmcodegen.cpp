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
}

void LLVMCodeGen::visit(BinaryOpExpression *node)
{
    std::cout << "Generating code for BinaryOpExpression" << std::endl;

    // TODO We should cast here if types do not match
    Value * right = pop();
    Value * left = pop();

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

    Value * result = BinaryOperator::Create(instr, left, right, "tmp", context.currentBlock());
    m_valuesStack.push(result);
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
    Function * function = Function::Create(functionType, GlobalValue::InternalLinkage, m_name->name(), module());

    BasicBlock * basicBlock = BasicBlock::Create(getGlobalContext(), "entry", function, 0);

    pushBlock(basicBlock);

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

void LLVMCodeGen::visit(ReturnStatement *node)
{
    cout << "Generating code for ReturnStatement" << endl;

    Value * expr = NULL;

    if(m_expression) {
        cout << "  Return expression" << endl;
        expr = m_expression->codeGen(context);
    }

    ReturnInst::Create(getGlobalContext(), expr, currentBlock());
}

void LLVMCodeGen::visit(AssignmentExpression *node)
{
    std::cout << "Generating code for AssignmentExpression" << std::endl;
    Value * existingVar = locals()[m_id->name()];
    Value * expr = pop();

    if(!existingVar) {
        cout << "  Generating variable " << node->id()->name() << endl;
        cout << "    Type: " << node->id()->typeString() << endl;

        llvm::Type * varType;
        if(m_rightExpression->type() != ExpressionNode::Invalid) {
            varType = getType(node->id()->type());
        } else {
            cerr << "Can't figure out type of expression!!" << endl;
            varType = getType("int");
        }

        existingVar = new AllocaInst(varType, m_id->name(), currentBlock());
        locals()[m_id->name()] = existingVar;
    }

    new StoreInst(expr, existingVar, false, currentBlock());
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

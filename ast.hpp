#ifndef AST_HPP__
#define AST_HPP__

#include <map>
#include <stack>
#include <vector>
#include <llvm/Value.h>
#include <llvm/Module.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/LLVMContext.h>
#include <llvm/Constants.h>
#include <llvm/Type.h>
#include <llvm/Function.h>
#include <llvm/DerivedTypes.h>
#include <llvm/support/IRBuilder.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/PassManager.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>

using namespace llvm;
using namespace std;

class NBlock;
class NStatement;
class NExpression;

typedef vector<NStatement *> StatementList;
typedef vector<NExpression *> ExpressionList;

class CodeGenBlock {
public:
    BasicBlock *block;
    map<string, Value*> locals;
};

class CodeGenContext {
    stack<CodeGenBlock *> blocks;
    Function *mainFunction;

public:
    Module *module;
    CodeGenContext() {
        module = new Module("main", getGlobalContext());
    }
    
    void generateCode(NBlock& root);
    GenericValue runCode();
    
    map<string, Value*>& locals()
    {
        return blocks.top()->locals;
    }
    
    BasicBlock * currentBlock()
    {
        return blocks.top()->block;
    }
    
    void pushBlock(BasicBlock * block)
    {
        blocks.push(new CodeGenBlock());
        blocks.top()->block = block;
    }
    
    void popBlock() {
        CodeGenBlock *top = blocks.top();
        blocks.top();
        delete top;
    }
};

class Node {
public:
    virtual ~Node() { }
    virtual Value* codeGen(CodeGenContext& context) { }
};

class NExpression : public Node {
};

class NStatement : public Node {
};

class NInteger : public NExpression {
public:
    long long value;
    NInteger(long long value) : value(value) { }
    virtual Value* codeGen(CodeGenContext& context);
};

class NDouble : public NExpression {
public:
    double value;
    NDouble(double value) : value(value) { }
    virtual Value* codeGen(CodeGenContext& context);
};

class NBlock : public NExpression {
public:
    StatementList statements;
    NBlock() { }
    virtual Value * codeGen(CodeGenContext& context);
};

class NExpressionStatement : public NStatement {
public:
    NExpression& expression;
    NExpressionStatement(NExpression& expression)
        : expression(expression) { }
    virtual Value * codeGen(CodeGenContext& context);
};

#endif // AST_H__
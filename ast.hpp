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
class NExpressionStatement;

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
    IRBuilder<> *builder;

    CodeGenContext() {
        module = new Module("main", getGlobalContext());
        builder = new IRBuilder<>(getGlobalContext());
    }

    virtual ~CodeGenContext() {
        delete builder;
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

class NIfStatement : public NStatement {
public:
    NStatement& expr;
    NStatement& thenStmt;
    NStatement& elseStmt;

    NIfStatement(NStatement& expr1, NStatement& thenStmt1, NStatement& elseStmt1) :
        expr(expr1), thenStmt(thenStmt1), elseStmt(elseStmt1) {}
    virtual Value* codeGen(CodeGenContext& context);
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

class NBinaryOperator : public NExpression {
public:
    int op;
    NExpression& lhs;
    NExpression& rhs;
    NBinaryOperator(NExpression& lhs, int op, NExpression& rhs) :
        lhs(lhs), rhs(rhs), op(op) { }
    virtual Value * codeGen(CodeGenContext& context);
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

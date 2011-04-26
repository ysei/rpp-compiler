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

using namespace llvm;
using namespace std;

class NBlock;

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
};

class Node {
public:
    virtual ~Node() { }
    virtual Value* codeGen(CodeGenContext& context) { }
};

class NExpression : public Node {
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

#endif // AST_H__
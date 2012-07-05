#ifndef _LLVMCODEGEN_H__
#define _LLVMCODEGEN_H__

#include <map>
#include <stack>
#include <vector>
#include <llvm/Instruction.h>
#include <llvm/InstrTypes.h>
#include "astnodevisitor.h"

class llvm::Value;
class llvm::Module;
class llvm::BasicBlock;

class BinaryOpFactory {
public:
    static llvm::Instruction::BinaryOps getIntOperator(const std::string& opString);
    static llvm::Instruction::BinaryOps getFloatOperator(const std::string& opString);

    static llvm::CmpInst::Predicate getIntPredicate(const std::string& opString);
    static llvm::CmpInst::Predicate getFloatPredicate(const std::string& opString);
};

class LLVMCodeGen : public ASTNodeVisitor
{
public:
    LLVMCodeGen();
    virtual ~LLVMCodeGen();

    virtual void visitEnter(Program * node);
    virtual void visitExit(Program * node);
    virtual void visit(ASTNode * node);
    virtual void visit(ExpressionNode * node);
    virtual void visit(StatementNode * node);
    virtual void visit(FloatNode * node);
    virtual void visit(IntegerNode * node);
    virtual void visit(IdentifierNode * node);
    virtual void visit(BinaryOpExpression * node);
    virtual void visit(MethodCallExpression * node);
    virtual void visit(BlockStatement * node);
    virtual void visit(VariableDeclaration * node);
    virtual void visitEnter(MethodDeclaration * node);
    virtual void visitExit(MethodDeclaration * node);
    virtual void visit(ReturnStatement * node);
    virtual void visit(AssignmentExpression * node);
    virtual void visitEnter(IfStatement * node);
    virtual void visitExit(IfStatement * node);

    void printAssembly();

protected:
    void push(llvm::Value * value);
    llvm::Value * pop();

    struct CodeGenBlock
    {
        llvm::BasicBlock * block;
        std::map<std::string, llvm::Value *> locals;
        std::map<std::string, llvm::Value *> arguments;
    };

    llvm::Module * module();

    std::map<std::string, llvm::Value *>& locals();
    std::map<std::string, llvm::Value *>& arguments();
    llvm::BasicBlock * currentBlock();

    void pushBlock(llvm::BasicBlock * block);
    void popBlock();

private:
    llvm::Function * getOrCreateFunction(const MethodDeclaration *node);
    std::vector<llvm::Type *> generateArgTypes(std::vector<VariableDeclaration *>::const_iterator start,
                                               std::vector<VariableDeclaration *>::const_iterator end);
    llvm::Instruction::BinaryOps getOperator(const std::string& opString, ExpressionNode::Type type);
    llvm::CmpInst::Predicate getLogicalOperator(const std::string& opString, ExpressionNode::Type type);
    llvm::Value * castIfNeeded(llvm::Value * value, ExpressionNode::Type sourceTypeOfValue, ExpressionNode::Type targetTypeOfValue);
    bool isLogicalOp(const std::string& opString);

private:
    std::stack<llvm::Value *> m_valuesStack;
    std::stack<CodeGenBlock *> m_blocks;
    llvm::Module * m_module;
};

#endif // _LLVMCODEGEN_H__

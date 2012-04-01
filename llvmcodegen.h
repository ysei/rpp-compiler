#ifndef _LLVMCODEGEN_H__
#define _LLVMCODEGEN_H__

#include "astnodevisitor.h"

class LLVMCodeGen : public ASTNodeVisitor
{
public:
    LLVMCodeGen();
    virtual ~LLVMCodeGen();

    virtual void visit(Program * node);
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
    virtual void visit(MethodDeclaration * node);
    virtual void visit(ReturnStatement * node);
    virtual void visit(AssignmentExpression * node);

private:
};

#endif // _LLVMCODEGEN_H__

#ifndef _ASTNODEVISITOR_H__
#define _ASTNODEVISITOR_H__

class ASTNode;
class ExpressionNode;
class StatementNode;
class FloatNode;
class IntegerNode;
class IdentifierNode;
class BinaryOpExpression;
class MethodCallExpression;
class BlockStatement;
class VariableDeclaration;
class MethodDeclaration;
class ReturnStatement;
class AssignmentExpression;
class Program;

class ASTNodeVisitor
{
public:
    virtual void visit(ASTNode * node) {}
    virtual void visit(ExpressionNode * node) {}
    virtual void visit(StatementNode * node) {}
    virtual void visit(FloatNode * node) {}
    virtual void visit(IntegerNode * node) {}
    virtual void visit(IdentifierNode * node) {}
    virtual void visit(BinaryOpExpression * node) {}
    virtual void visit(MethodCallExpression * node) {}
    virtual void visit(BlockStatement * node) {}
    virtual void visit(VariableDeclaration * node) {}
    virtual void visit(MethodDeclaration * node) {}
    virtual void visit(ReturnStatement * node) {}
    virtual void visit(AssignmentExpression * node) {}
    virtual void visit(Program * node) {}
};

#endif

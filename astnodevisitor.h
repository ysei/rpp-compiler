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
class IfStatement;
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
    virtual void visitEnter(MethodDeclaration * node) {}
    virtual void visitExit(MethodDeclaration * node) {}
    virtual void visit(ReturnStatement * node) {}
    virtual void visit(AssignmentExpression * node) {}
    virtual void visitEnter(IfStatement * node) {}
    virtual void visitExit(IfStatement * node) {}
    virtual void visitEnter(Program * node) {}
    virtual void visitExit(Program * node) {}
};

#endif

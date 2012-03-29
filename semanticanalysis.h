#ifndef _SEMANTICANALYSIS_H__
#define _SEMANTICANALYSIS_H__

#include "ast.h"
#include "astnodevisitor.h"

class SemanticAnalysis : public ASTNodeVisitor
{
public:
    virtual void visit(ASTNode * node) {

    }

    virtual void visit(ExpressionNode * node) {

    }

    virtual void visit(StatementNode * node) {

    }

    virtual void visit(FloatNode * node) {

    }

    virtual void visit(IdentifierNode * node) {

    }

    virtual void visit(BinaryOpExpression * node) {

    }

    virtual void visit(MethodCallExpression * node) {

    }

    virtual void visit(BlockStatement * node) {

    }

    virtual void visit(VariableDeclaration * node) {

    }

    virtual void visit(MethodDeclaration * node) {

    }

    virtual void visit(ReturnStatement * node) {

    }

    virtual void visit(AssignmentExpression * node) {

    }

    virtual void visit(Program * node) {

    }
};

#endif // _SEMANTICANALYSIS_H__

#ifndef _SEMANTICANALYSIS_H__
#define _SEMANTICANALYSIS_H__

#include <stack>

#include "ast.h"
#include "astnodevisitor.h"

struct MethodContext {
    std::string methodName;
    std::map<const std::string, ExpressionNode::Type> locals;
    std::map<const std::string, ExpressionNode::Type> params;
    ExpressionNode::Type returnType;

    void clear() {
        methodName = "";
        locals.clear();
        params.clear();
    }
};

class SemanticAnalysis : public ASTNodeVisitor
{
public:
    SemanticAnalysis();
    virtual ~SemanticAnalysis();

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
    virtual void visit(Program * node);

protected:
    void push(ExpressionNode::Type type);
    ExpressionNode::Type pop();

private:
    std::stack<ExpressionNode::Type> m_types;
    MethodContext m_methodContext;
};

#endif // _SEMANTICANALYSIS_H__

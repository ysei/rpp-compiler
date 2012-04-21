#include <string>

#include "semanticanalysis.h"

using namespace std;

SemanticAnalysis::SemanticAnalysis()
{
}

SemanticAnalysis::~SemanticAnalysis()
{
}

void SemanticAnalysis::visit(ASTNode *node)
{
}

void SemanticAnalysis::visit(ExpressionNode *node)
{
}

void SemanticAnalysis::visit(StatementNode * node)
{
}

void SemanticAnalysis::visit(FloatNode * node)
{
    push(ExpressionNode::Float, node->value());
}

void SemanticAnalysis::visit(IntegerNode * node)
{
    push(ExpressionNode::Int, node->value());
}

void SemanticAnalysis::visit(IdentifierNode *node)
{
    const std::string id = node->name();
    if(m_methodContext.locals.find(id) != m_methodContext.locals.end()) {
        node->setType(m_methodContext.locals[id]);
        push(node->type(), node->name());
    } else if(m_methodContext.params.find(id) != m_methodContext.params.end()) {
        node->setType(m_methodContext.params[id]);
        push(node->type(), node->name());
    }
}

void SemanticAnalysis::visit(BinaryOpExpression *node)
{
    ExpressionNode::Type left = pop();
    ExpressionNode::Type right = pop();

    ExpressionNode::Type combined = ExpressionNode::combineTypes(left, right);

    push(combined, ExpressionNode::toString(combined));

    if(combined == ExpressionNode::Invalid) {
        std::cerr << "Can't convert one type to another" << std::endl;
    }

    node->setType(combined);
}

void SemanticAnalysis::visit(MethodCallExpression *node)
{
    node->setType(node->methodDeclaration()->returnType());
}

void SemanticAnalysis::visit(BlockStatement *node)
{
}

void SemanticAnalysis::visit(VariableDeclaration *node)
{
}

void SemanticAnalysis::visitEnter(MethodDeclaration *node)
{
    m_methodContext.clear();

    m_methodContext.methodName = node->name();
    for(std::vector<VariableDeclaration *>::const_iterator iter =
        node->arguments().begin(); iter != node->arguments().end(); iter++) {
        VariableDeclaration * decl = *iter;
        m_methodContext.params[decl->varName()->name()] = decl->varType()->asType();
    }

    m_methodContext.returnType = node->returnType();
}

void SemanticAnalysis::visitExit(MethodDeclaration *node)
{
}

void SemanticAnalysis::visit(ReturnStatement *node)
{
    // TODO need to check that expression can be castable to func return type
    node->setReturnType(m_methodContext.returnType);
}

void SemanticAnalysis::visit(AssignmentExpression *node)
{
    ExpressionNode::Type type = pop();
    node->id()->setType(type);
    m_methodContext.locals[node->id()->name()] = node->id()->type();
}

void SemanticAnalysis::visitEnter(Program *node)
{
}

void SemanticAnalysis::visitExit(Program *node)
{
}

template<class T>
void SemanticAnalysis::push(ExpressionNode::Type type, T msg)
{
    m_types.push(type);
    std::cout << "-- Pushing [" << ExpressionNode::toString(type) << ", " << msg << "] - size: " << m_types.size() << std::endl;
}

ExpressionNode::Type SemanticAnalysis::pop()
{
    ExpressionNode::Type type = m_types.top();
    m_types.pop();
    std::cout << "-- Popping: " << ExpressionNode::toString(type) << " - size: " << m_types.size() << std::endl;
    return type;
}

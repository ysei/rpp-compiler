#include "semanticanalysis.h"

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
    std::cout << "Visiting ExpressionNode" << std::endl;
}

void SemanticAnalysis::visit(StatementNode * node)
{
}

void SemanticAnalysis::visit(FloatNode * node)
{
    push(ExpressionNode::Float, node->value());
    std::cout << "Pushing Float: " << node->value() << std::endl;
}

void SemanticAnalysis::visit(IntegerNode * node)
{
    push(ExpressionNode::Int, node->value());
    std::cout << "Pushing Int: " << node->value() << std::endl;
}

void SemanticAnalysis::visit(IdentifierNode *node)
{
    const std::string id = node->name();
    if(m_methodContext.locals.find(id) != m_methodContext.locals.end()) {
        node->setType(m_methodContext.locals[id]);
        std::cout << "Pushing local: " + node->typeString() + ", " + node->name() << ", len: " << m_types.size() << std::endl;
        push(node->type(), node->name());
    } else if(m_methodContext.params.find(id) != m_methodContext.params.end()) {
        node->setType(m_methodContext.params[id]);
        std::cout << "Pushing param: " + node->typeString() + ", " + node->name() << ", len: " << m_types.size() << std::endl;
        push(node->type(), node->name());
    }
}

void SemanticAnalysis::visit(BinaryOpExpression *node)
{
    ExpressionNode::Type left = pop();
    ExpressionNode::Type right = pop();

    ExpressionNode::Type combined = ExpressionNode::combineTypes(left, right);
    std::cout << "BinOp: " << node->op() << ", " << combined << ", len: " << m_types.size() << std::endl;
    push(combined, ExpressionNode::toString(combined));
    if(combined == ExpressionNode::Invalid) {
        std::cerr << "Can't convert one type to another" << std::endl;
    }
}

void SemanticAnalysis::visit(MethodCallExpression *node)
{
}

void SemanticAnalysis::visit(BlockStatement *node)
{
}

void SemanticAnalysis::visit(VariableDeclaration *node)
{
}

void SemanticAnalysis::visit(MethodDeclaration *node)
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

void SemanticAnalysis::visit(ReturnStatement *node)
{
}

void SemanticAnalysis::visit(AssignmentExpression *node)
{
    ExpressionNode::Type type = pop();
    node->id()->setType(type);
    m_methodContext.locals[node->id()->name()] = node->id()->type();
}

void SemanticAnalysis::visit(Program *node)
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

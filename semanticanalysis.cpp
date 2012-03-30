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
    m_types.push(ExpressionNode::Float);
    std::cout << "Pushing Float: " << node->value() << std::endl;
}

void SemanticAnalysis::visit(IntegerNode * node)
{
    m_types.push(ExpressionNode::Int);
    std::cout << "Pushing Int: " << node->value() << std::endl;
}

void SemanticAnalysis::visit(IdentifierNode *node)
{
    const std::string id = node->name();
    if(m_methodContext.locals.find(id) != m_methodContext.locals.end()) {
        node->setType(m_methodContext.locals[id]);
        m_types.push(node->type());
        std::cout << "Pushing local: " + node->typeString() + ", " + node->name() << ", len: " << m_types.size() << std::endl;
    } else if(m_methodContext.params.find(id) != m_methodContext.params.end()) {
        node->setType(m_methodContext.params[id]);
        m_types.push(node->type());
        std::cout << "Pushing param: " + node->typeString() + ", " + node->name() << ", len: " << m_types.size() << std::endl;
    }
}

void SemanticAnalysis::visit(BinaryOpExpression *node)
{
    ExpressionNode::Type left = m_types.top();
    m_types.pop();
    ExpressionNode::Type right = m_types.top();
    m_types.pop();

    ExpressionNode::Type combined = ExpressionNode::combineTypes(left, right);
    std::cout << "BinOp: " << node->op() << ", " << combined << ", len: " << m_types.size() << std::endl;
    if(combined != ExpressionNode::Invalid) {
        m_types.push(combined);
    } else {
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
    m_types.pop();
}

void SemanticAnalysis::visit(AssignmentExpression *node)
{
    ExpressionNode::Type type = m_types.top();
    std::cout << "Popping: " << type << ",len: " << m_types.size() << std::endl;
    m_types.pop();
    node->id()->setType(type);
}

void SemanticAnalysis::visit(Program *node)
{
}

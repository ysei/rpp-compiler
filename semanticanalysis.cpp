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
    m_calls.push_back(node);
}

void SemanticAnalysis::visit(BlockStatement *node)
{
}

void SemanticAnalysis::visit(VariableDeclaration *node)
{
}

void SemanticAnalysis::visitEnter(MethodDeclaration *node)
{
    m_methods.insert(pair<string, MethodDeclaration *>(node->name(), node));
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
    bindMethodCalls();
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

void SemanticAnalysis::bindMethodCalls()
{
    for(vector<MethodCallExpression *>::const_iterator iter = m_calls.begin(); iter != m_calls.end(); iter++) {
        MethodCallExpression * call = *iter;

        MethodDeclaration * declaration = findMethodDeclaration(call);

        if(declaration) {
            call->bindTo(declaration);
        } else {
            cerr << "Function definition was not found" << endl;
        }
    }
}

MethodDeclaration *SemanticAnalysis::findMethodDeclaration(const MethodCallExpression *call)
{
    pair<MethodsIterator, MethodsIterator> methodRange = m_methods.equal_range(call->methodName()->name());
    if(methodRange.first != methodRange.second) {
        vector<pair<int, MethodDeclaration *> > candidates; // Score, method declaration pointer
        for(MethodsIterator iter = methodRange.first; iter != methodRange.second; iter++) {
            int score = calculateScore(call, (*iter).second);
            if(score > 0) {
                candidates.push_back(pair<int, MethodDeclaration *>(score, (*iter).second));
            }
        }

        return SemanticAnalysis::findBestMatch(candidates);
    } else {
        return NULL;
    }
}

int SemanticAnalysis::calculateScore(const MethodCallExpression *call, const MethodDeclaration *declaration) const
{
    const vector<ExpressionNode *> & callArguments(call->arguments());
    const vector<VariableDeclaration *> &declArguments(declaration->arguments());

    if(callArguments.size() != declArguments.size()) {
        return -1;
    }

    int score = 1;
    for(int i = 0; i < callArguments.size(); i++) {
        ExpressionNode::Type callArgType = callArguments[i]->type();
        ExpressionNode::Type declArgType = declArguments[i]->varType()->asType();

        if(callArgType == declArgType) {
            score++;
        } else if(!ExpressionNode::canImplicitCast(callArgType, declArgType)) {
            return -1;
        }
    }

    return score;
}

MethodDeclaration *SemanticAnalysis::findBestMatch(const vector<pair<int, MethodDeclaration *> > &candidates)
{
    MethodDeclaration * bestMatch = NULL;
    int bestScore = -1000;
    for(int i = 0; i < candidates.size(); i++) {
        if(candidates[i].first > bestScore) {
            bestScore = candidates[i].first;
            bestMatch = candidates[i].second;
        }
    }

    return bestMatch;
}

#include "bindmethods.h"

using namespace std;

BindMethods::BindMethods()
{
}

BindMethods::~BindMethods()
{
}

void BindMethods::visitEnter(Program *node)
{
}

void BindMethods::visitExit(Program *node)
{
    bindMethodCalls();
}

void BindMethods::visit(MethodCallExpression *node)
{
    m_calls.push_back(node);
}

void BindMethods::visitEnter(MethodDeclaration *node)
{
    m_methods.insert(pair<string, MethodDeclaration *>(node->name(), node));
}

void BindMethods::visitExit(MethodDeclaration *node)
{
}

void BindMethods::bindMethodCalls()
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

MethodDeclaration *BindMethods::findMethodDeclaration(const MethodCallExpression *call)
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

        return BindMethods::findBestMatch(candidates);
    } else {
        return NULL;
    }
}

int BindMethods::calculateScore(const MethodCallExpression *call, const MethodDeclaration *declaration) const
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

MethodDeclaration *BindMethods::findBestMatch(const vector<pair<int, MethodDeclaration *> > &candidates)
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


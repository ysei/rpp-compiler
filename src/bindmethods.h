#ifndef BINDMETHODS_H
#define BINDMETHODS_H

#include <map>
#include <vector>

#include "ast.h"
#include "astnodevisitor.h"

class BindMethods : public ASTNodeVisitor
{
public:
    BindMethods();
    virtual ~BindMethods();

    virtual void visit(MethodCallExpression * node);
    virtual void visitEnter(MethodDeclaration * node);
    virtual void visitExit(MethodDeclaration * node);
    virtual void visitEnter(Program * node);
    virtual void visitExit(Program * node);

private:
    void bindMethodCalls();
    MethodDeclaration *findMethodDeclaration(const MethodCallExpression * call);
    int calculateScore(const MethodCallExpression * call, const MethodDeclaration * declaration) const;

    static MethodDeclaration *findBestMatch(const std::vector<std::pair<int, MethodDeclaration *> >& candidates);

private:
    std::vector<MethodCallExpression *> m_calls;
    typedef std::multimap<std::string, MethodDeclaration *>::iterator MethodsIterator;
    std::multimap<std::string, MethodDeclaration *> m_methods;
};

#endif // BINDMETHODS_H

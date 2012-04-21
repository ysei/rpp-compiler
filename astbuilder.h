#ifndef _ASTCREATOR_H__
#define _ASTCREATOR_H__

#include <iostream>

#include <antlr3basetree.h>
#include <antlr3basetreeadaptor.h>
#include <antlr3commontreenodestream.h>
#include <antlr3commontoken.h>
#include <antlr3string.h>

#include "ast.h"
#include "antlrsupport.h"

#include "rppLexer.h"

class ASTBuilder
{
public:
    ASTBuilder();
    virtual ~ASTBuilder();

    ASTNode * createProgram(pANTLR3_BASE_TREE node);

    MethodDeclaration * createMethodDeclaration(pANTLR3_BASE_TREE node);
    BlockStatement * createBlock(pANTLR3_BASE_TREE node);
    ASTNode *createStatement(pANTLR3_BASE_TREE node);
    ReturnStatement * createReturnStatement(pANTLR3_BASE_TREE node);
    ExpressionNode * createExpression(pANTLR3_BASE_TREE node);
    AssignmentExpression * createAssignment(pANTLR3_BASE_TREE node);
    IdentifierNode * createReturnType(pANTLR3_BASE_TREE node);
    std::vector<VariableDeclaration *> createParams(pANTLR3_BASE_TREE node);
    VariableDeclaration * createParam(pANTLR3_BASE_TREE node);
    IdentifierNode * createIdentifier(pANTLR3_BASE_TREE node);
    MethodCallExpression * createMethodCall(pANTLR3_BASE_TREE node);
};

#endif

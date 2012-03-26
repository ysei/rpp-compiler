#include "astcreator.h"

AstCreator::AstCreator()
{
}

AstCreator::~AstCreator()
{
}

ASTNode * AstCreator::createAst(pANTLR3_BASE_TREE node)
{
    pANTLR3_COMMON_TOKEN token = node->getToken(node);
    ASTNode * n = NULL;

    if(token->type == FUNC_DEF) {
        n = createMethodDeclaration(node);
    }

    return n;
}

MethodDeclaration * AstCreator::createMethodDeclaration(pANTLR3_BASE_TREE node)
{
    IdentifierNode * idNode = createIdentifier(getNodeChild(node, 0));
    std::vector<VariableDeclaration *> arguments = createParams(getNodeChild(node, 1));
    IdentifierNode * returnType = createReturnType(getNodeChild(node, 2));
    BlockStatement * blockStatement = createBlock(getNodeChild(node, 3));

    return new MethodDeclaration(idNode, returnType, arguments, blockStatement);
}

BlockStatement * AstCreator::createBlock(pANTLR3_BASE_TREE node)
{
    assert(getTokenType(node) == BLOCK);

    std::vector<StatementNode *> statements;

    for(int i = 0; i < getNodeChildCount(node); i++) {
        pANTLR3_BASE_TREE stmtNode = getNodeChild(node, 0);
        StatementNode * stmt = createStatement(stmtNode);
        statements.push_back(stmt);
    }

    return new BlockStatement(statements);
}

StatementNode *AstCreator::createStatement(pANTLR3_BASE_TREE node)
{
    return NULL;
}

IdentifierNode * AstCreator::createReturnType(pANTLR3_BASE_TREE node)
{
    assert(getTokenType(node) == FUNC_RETURN_TYPE);

    if(getNodeChildCount(node) == 0) {
        return new IdentifierNode("void");
    }

    IdentifierNode * astNode = new IdentifierNode(getNodeString(getNodeChild(node, 0)));

    return astNode;
}

std::vector<VariableDeclaration *> AstCreator::createParams(pANTLR3_BASE_TREE node)
{
    assert(getTokenType(node) == FUNC_PARAMS);

    std::vector<VariableDeclaration *> arguments;
    for(int i = 0; i < getNodeChildCount(node); i++) {
        VariableDeclaration * paramNode = createParam(getNodeChild(node, i));
        arguments.push_back(paramNode);
    }

    return arguments;
}

VariableDeclaration * AstCreator::createParam(pANTLR3_BASE_TREE node)
{
    assert(getTokenType(node) == FUNC_PARAM);

    IdentifierNode * typeNode = createIdentifier(getNodeChild(node, 0));
    IdentifierNode * idNode = createIdentifier(getNodeChild(node, 1));
    VariableDeclaration * astNode = new VariableDeclaration(typeNode, idNode);

    return astNode;
}

IdentifierNode * AstCreator::createIdentifier(pANTLR3_BASE_TREE node)
{
    std::cout << "RPP_ID: " << getNodeString(node) << std::endl;
    IdentifierNode * astNode = new IdentifierNode(getNodeString(node));

    return astNode;
}


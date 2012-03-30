#include "astbuilder.h"

ASTBuilder::ASTBuilder()
{
}

ASTBuilder::~ASTBuilder()
{
}

ASTNode * ASTBuilder::createProgram(pANTLR3_BASE_TREE node)
{
    assert(node);

    pANTLR3_COMMON_TOKEN token = node->getToken(node);

    assert(token);
    assert(getTokenType(node) == PROGRAM);

    std::vector<MethodDeclaration *> methods;
    for(int i = 0; i < getNodeChildCount(node); i++) {
        MethodDeclaration * method = createMethodDeclaration(getNodeChild(node, i));
        methods.push_back(method);
    }

    return new Program(methods);
}

MethodDeclaration * ASTBuilder::createMethodDeclaration(pANTLR3_BASE_TREE node)
{
    IdentifierNode * idNode = createIdentifier(getNodeChild(node, 0));
    std::vector<VariableDeclaration *> arguments = createParams(getNodeChild(node, 1));
    IdentifierNode * returnType = createReturnType(getNodeChild(node, 2));
    BlockStatement * blockStatement = createBlock(getNodeChild(node, 3));

    return new MethodDeclaration(idNode, returnType, arguments, blockStatement);
}

BlockStatement * ASTBuilder::createBlock(pANTLR3_BASE_TREE node)
{
    assert(getTokenType(node) == BLOCK);

    std::vector<ASTNode *> statements;

    for(int i = 0; i < getNodeChildCount(node); i++) {
        pANTLR3_BASE_TREE stmtNode = getNodeChild(node, i);
        ASTNode * stmt = createStatement(stmtNode);
        statements.push_back(stmt);
    }

    return new BlockStatement(statements);
}

ASTNode * ASTBuilder::createStatement(pANTLR3_BASE_TREE node)
{
    int token = getTokenType(node);
    switch(token) {
    case RETURN:
        return createReturnStatement(node);

    case ASSIGNMENT:
        return createAssignment(node);
    }

    return NULL;
}

ReturnStatement * ASTBuilder::createReturnStatement(pANTLR3_BASE_TREE node)
{
    assert(getTokenType(node) == RETURN);

    ExpressionNode * exprNode = createExpression(getNodeChild(node, 0));
    return new ReturnStatement(exprNode);
}

ExpressionNode *ASTBuilder::createExpression(pANTLR3_BASE_TREE node)
{
    std::cout << "Visited createExpression" << std::endl;

    if(getTokenType(node) == RPP_ID) {
        return new IdentifierNode(getNodeString(node));
    } else if(getTokenType(node) == FLOAT) {
        return new FloatNode((float)atof(getNodeText(node)));
    } else if(getTokenType(node) == INT) {
        return new IntegerNode(atoi(getNodeText(node)));
    } else {
        std::string nodeString(getNodeString(node));

        if(nodeString == "+" || nodeString == "-" || nodeString == "*" || nodeString == "/") {
            if(getNodeChildCount(node) == 1) {
                std::cout << "Error: unary operations currently not supported" << std::endl;
                return NULL;
            }

            ExpressionNode * left = createExpression(getNodeChild(node, 0));
            ExpressionNode * right = createExpression(getNodeChild(node, 1));
            BinaryOpExpression * expr = new BinaryOpExpression(left, right, nodeString);
            return expr;
        }
    }

    std::cout << "Found unrecognized expression, node: [" << getNodeString(node) << "] type: [" << getTokenType(node) << "]" << std::endl;

    return NULL;
}

AssignmentExpression *ASTBuilder::createAssignment(pANTLR3_BASE_TREE node)
{
    assert(getTokenType(node) == ASSIGNMENT);
    assert(getNodeChildCount(node) == 2);

    IdentifierNode * idNode = createIdentifier(getNodeChild(node, 0));
    ExpressionNode * exprNode = createExpression(getNodeChild(node, 1));

    return new AssignmentExpression(idNode, exprNode);
}

IdentifierNode * ASTBuilder::createReturnType(pANTLR3_BASE_TREE node)
{
    assert(getTokenType(node) == FUNC_RETURN_TYPE);

    if(getNodeChildCount(node) == 0) {
        return new IdentifierNode("void");
    }

    IdentifierNode * astNode = new IdentifierNode(getNodeString(getNodeChild(node, 0)));

    return astNode;
}

std::vector<VariableDeclaration *> ASTBuilder::createParams(pANTLR3_BASE_TREE node)
{
    assert(getTokenType(node) == FUNC_PARAMS);

    std::vector<VariableDeclaration *> arguments;
    for(int i = 0; i < getNodeChildCount(node); i++) {
        VariableDeclaration * paramNode = createParam(getNodeChild(node, i));
        arguments.push_back(paramNode);
    }

    return arguments;
}

VariableDeclaration * ASTBuilder::createParam(pANTLR3_BASE_TREE node)
{
    assert(getTokenType(node) == FUNC_PARAM);

    IdentifierNode * typeNode = createIdentifier(getNodeChild(node, 0));
    IdentifierNode * idNode = createIdentifier(getNodeChild(node, 1));
    VariableDeclaration * astNode = new VariableDeclaration(typeNode, idNode);

    return astNode;
}

IdentifierNode * ASTBuilder::createIdentifier(pANTLR3_BASE_TREE node)
{
    std::cout << "RPP_ID: " << getNodeString(node) << std::endl;
    IdentifierNode * astNode = new IdentifierNode(getNodeString(node));

    return astNode;
}


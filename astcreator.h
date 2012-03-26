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

class AstCreator
{
public:
    AstCreator()
    {
    }

    virtual ~AstCreator()
    {
    }

    ASTNode * createAst(pANTLR3_BASE_TREE node)
    {
        pANTLR3_COMMON_TOKEN token = node->getToken(node);
        ASTNode * n = NULL;

        if(token->type == FUNC_DEF) {
            n = createMethodDeclaration(node);
        }

        return n;
    }

    MethodDeclaration * createMethodDeclaration(pANTLR3_BASE_TREE node)
    {
        IdentifierNode * idNode = createIdentifier(getNodeChild(node, 0));
        std::vector<VariableDeclaration *> arguments = createParams(getNodeChild(node, 1));
        IdentifierNode * returnType = createReturnType(getNodeChild(node, 2));
        BlockStatement * blockStatement = createBlock(getNodeChild(node, 3));

        return new MethodDeclaration(idNode, returnType, arguments, blockStatement);
    }

    BlockStatement * createBlock(pANTLR3_BASE_TREE node)
    {
        return NULL;
    }

    IdentifierNode * createReturnType(pANTLR3_BASE_TREE node)
    {
        assert(getTokenType(node) == FUNC_RETURN_TYPE);

        if(getNodeChildCount(node) == 0) {
            return new IdentifierNode("void");
        }

        IdentifierNode * astNode = new IdentifierNode(getNodeString(getNodeChild(node, 0)));

        return astNode;
    }

    std::vector<VariableDeclaration *> createParams(pANTLR3_BASE_TREE node)
    {
        assert(getTokenType(node) == FUNC_PARAMS);

        std::vector<VariableDeclaration *> arguments;
        for(int i = 0; i < getNodeChildCount(node); i++) {
            VariableDeclaration * paramNode = createParam(getNodeChild(node, i));
            arguments.push_back(paramNode);
        }

        return arguments;
    }

    VariableDeclaration * createParam(pANTLR3_BASE_TREE node)
    {
        assert(getTokenType(node) == FUNC_PARAM);

        IdentifierNode * typeNode = createIdentifier(getNodeChild(node, 0));
        IdentifierNode * idNode = createIdentifier(getNodeChild(node, 1));
        VariableDeclaration * astNode = new VariableDeclaration(typeNode, idNode);

        return astNode;
    }

    IdentifierNode * createIdentifier(pANTLR3_BASE_TREE node)
    {
        std::cout << "RPP_ID: " << getNodeString(node) << std::endl;
        IdentifierNode * astNode = new IdentifierNode(getNodeString(node));

        return astNode;
    }

protected:
    /*
    llvm::Type * getType(const unsigned char * type) {
        std::string typeString((const char *) type);
        if(typeString == std::string("int")) {
            return llvm::Type::getInt32Ty(llvm::getGlobalContext());
        } else if(typeString == std::string("float")) {
            return llvm::Type::getFloatTy(llvm::getGlobalContext());
        } else if(typeString == std::string("void")) {
            return llvm::Type::getVoidTy(llvm::getGlobalContext());
        }

        return NULL;
    }
    */

private:
};

#endif

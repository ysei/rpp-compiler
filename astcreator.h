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
    AstCreator();
    virtual ~AstCreator();

    ASTNode * createAst(pANTLR3_BASE_TREE node);

    MethodDeclaration * createMethodDeclaration(pANTLR3_BASE_TREE node);
    BlockStatement * createBlock(pANTLR3_BASE_TREE node);
    StatementNode * createStatement(pANTLR3_BASE_TREE node);
    IdentifierNode * createReturnType(pANTLR3_BASE_TREE node);
    std::vector<VariableDeclaration *> createParams(pANTLR3_BASE_TREE node);
    VariableDeclaration * createParam(pANTLR3_BASE_TREE node);
    IdentifierNode * createIdentifier(pANTLR3_BASE_TREE node);

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

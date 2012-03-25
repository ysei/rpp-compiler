#include <llvm/Support/TargetSelect.h>
#include <llvm/Value.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/LLVMContext.h>
#include <llvm/Constants.h>
#include <llvm/Module.h>
#include <llvm/Function.h>
#include <llvm/DerivedTypes.h>
#include <llvm/support/IRBuilder.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/PassManager.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "rppLexer.h"
#include "rppParser.h"
#include <antlr3basetree.h>
#include <antlr3basetreeadaptor.h>
#include <antlr3commontreenodestream.h>
#include <antlr3commontoken.h>
#include <antlr3string.h>
#include <vector>

using namespace std;

/*
def func(x, y)
    return x + y
end

def main
    k = func(10, 2)
    puts k

    p = 23 + 43 * (10 + k)
    puts k
end

*/

inline pANTLR3_COMMON_TOKEN getToken(pANTLR3_BASE_TREE node)
{
    return node->getToken(node);
}

inline unsigned char * getTokenText(pANTLR3_COMMON_TOKEN token)
{
    return token->getText(token)->chars;
}

inline unsigned char * getNodeText(pANTLR3_BASE_TREE node)
{
    return getTokenText(getToken(node));
}

inline unsigned int getTokenType(pANTLR3_COMMON_TOKEN token)
{
    return token->type;
}

inline unsigned int getTokenType(pANTLR3_BASE_TREE node)
{
    return getTokenType(getToken(node));
}

inline pANTLR3_BASE_TREE getNodeChild(pANTLR3_BASE_TREE node, unsigned int index)
{
    return (pANTLR3_BASE_TREE) node->getChild(node, index);
}

inline unsigned int getNodeChildCount(pANTLR3_BASE_TREE node)
{
    return node->getChildCount(node);
}

class RppCodeGen
{
public:
    RppCodeGen()
    {
        module = new llvm::Module("main", llvm::getGlobalContext());
        builder = new llvm::IRBuilder<>(llvm::getGlobalContext());
    }

    virtual ~RppCodeGen()
    {
        delete builder;
        delete module;
    }

    llvm::Value * codeGenForFunc(const string& funcName, const vector<llvm::Type *> arguments, llvm::Type * returnType, pANTLR3_BASE_TREE block)
    {
        llvm::FunctionType *ftype = llvm::FunctionType::get(returnType, makeArrayRef(arguments), false);
        llvm::Function * function = llvm::Function::Create(ftype, llvm::GlobalValue::InternalLinkage, funcName.c_str(), module);
        llvm::BasicBlock *bblock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", function, 0);
        llvm::IRBuilder<> *builder = new llvm::IRBuilder<>(llvm::getGlobalContext());
        builder->SetInsertPoint(bblock);


        printByteCode();
        return NULL;
    }

    void printByteCode()
    {
        cout << "Code is generated." << endl;
        llvm::PassManager pm;
        pm.add(llvm::createPrintModulePass(&llvm::outs()));
        pm.run(*module);
    }

    void generateFunc(pANTLR3_BASE_TREE node)
    {
        pANTLR3_BASE_TREE nameNode = getNodeChild(node, 0);
        unsigned char * funcName = getNodeText(nameNode);

        printf("Generating function: %s\n", funcName);

        vector<llvm::Type *> argTypes;

        populateFuncArgsList(node, argTypes);

        llvm::Type * returnType = getReturnType(node);

        codeGenForFunc(string((const char *)funcName), argTypes, returnType, getNodeChild(node, getNodeChildCount(node) - 1));
    }

    void generateCode(pANTLR3_BASE_TREE node)
    {
        pANTLR3_COMMON_TOKEN tok = node->getToken(node);
        switch(tok->type) {
        case FUNC_DEF:
            generateFunc(node);
            break;
        }
    }

protected:
    llvm::Type * getType(const unsigned char * type) {
        string typeString((const char *) type);
        if(typeString == string("int")) {
            return llvm::Type::getInt32Ty(llvm::getGlobalContext());
        } else if(typeString == string("float")) {
            return llvm::Type::getFloatTy(llvm::getGlobalContext());
        } else if(typeString == string("void")) {
            return llvm::Type::getVoidTy(llvm::getGlobalContext());
        }

        return NULL;
    }

    void populateFuncArgsList(pANTLR3_BASE_TREE node, vector<llvm::Type *>& argTypes)
    {
        if(getNodeChildCount(node) == 1)
            return;

        pANTLR3_BASE_TREE possibleFuncParams = getNodeChild(node, 1);

        if(getTokenType(possibleFuncParams) == FUNC_PARAMS) {
            for(int i = 0; i < getNodeChildCount(possibleFuncParams); i++) {
                pANTLR3_BASE_TREE param = getNodeChild(possibleFuncParams, i);

                pANTLR3_BASE_TREE p = getNodeChild(param, 0);
                unsigned char * text = getNodeText(p);

                argTypes.push_back(getType(text));
                cout << getType(text) << endl;
            }
        }
    }

    llvm::Type * getReturnType(pANTLR3_BASE_TREE node)
    {
        if(getNodeChildCount(node) == 1) {
            return llvm::Type::getVoidTy(llvm::getGlobalContext());
        }

        pANTLR3_BASE_TREE possibleReturn;
        if(getNodeChildCount(node) == 2) {
            possibleReturn = getNodeChild(node, 1);
        } else {
            possibleReturn = getNodeChild(node, 2);
        }

        if(getTokenType(possibleReturn) == FUNC_RETURN_TYPE) {
            pANTLR3_BASE_TREE typeNode = getNodeChild(possibleReturn, 0);

            return getType(getNodeText(typeNode));
        }

        return llvm::Type::getVoidTy(llvm::getGlobalContext());
    }


private:
    llvm::Module *module;
    llvm::IRBuilder<> *builder;
};

int main(int /* argc */, char **  /* argv[] */)
{
    llvm::InitializeNativeTarget();

    pANTLR3_UINT8 fileName = (pANTLR3_UINT8) "example.rpp";
    pANTLR3_INPUT_STREAM input = antlr3FileStreamNew(fileName, ANTLR3_ENC_8BIT);
    pRppLexer lexer = RppLexerNew(input);
    pANTLR3_COMMON_TOKEN_STREAM tokenStream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lexer));
    pRppParser parser = RppParserNew(tokenStream);

    RppParser_prog_return prog;
    prog = parser->prog(parser);

    if (parser->pParser->rec->state->errorCount > 0)
    {
        fprintf(stderr, "The parser returned %d errors, tree walking aborted.\n", parser->pParser->rec->state->errorCount);
    } else {
        RppCodeGen codeGen;
        codeGen.generateCode(prog.tree);
        // generateCode(prog.tree);
    }

    parser->free(parser);
    tokenStream->free(tokenStream);
    lexer->free(lexer);
    input->free(input);

    return 0;
}

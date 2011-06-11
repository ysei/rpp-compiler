#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <llvm/Target/TargetSelect.h>
#include "ast.hpp"
#include "rppLexer.h"
#include "rppParser.h"
#include <antlr3basetree.h>
#include <antlr3basetreeadaptor.h>
#include <antlr3commontreenodestream.h>
#include <antlr3commontoken.h>
#include <antlr3string.h>
#include <vector>

using namespace std;

int yylex();
extern int yyparse();
extern NBlock * programBlock;
void scan_buffer(const char * buf);

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

void handleFuncDef(pANTLR3_BASE_TREE funcNode)
{
    pANTLR3_BASE_TREE funcName = getNodeChild(funcNode, 0);
    printf("funcName: %d, %s\n", getTokenType(funcName), getNodeText(funcName));
    vector<string> argList;
    pANTLR3_BASE_TREE argsNode = getNodeChild(funcNode, 1);
    if(getTokenType(argsNode) == ARG_DEF) {
        for(int i = 0; i < getNodeChildCount(argsNode); i++){
            pANTLR3_BASE_TREE argNode = getNodeChild(argsNode, i);
            unsigned char * argText = getNodeText(argNode);
            argList.push_back(string((char *)argText, strlen((char *)argText)));
            printf("Arg: %s\n", argText);
        }
    }

    printf("argType: %d\n", getTokenType(argsNode));
    //if()
}

void handle(pANTLR3_BASE_TREE node)
{
    pANTLR3_COMMON_TOKEN token = node->getToken(node);
    printf("Token: %s\n", token->toString(token));
    switch(token->type) {
        case FUNC_DEF:
            printf("Got FUNC_DEF\n");
            handleFuncDef(node);
        break;
    case ARG_DEF:
        break;
    }
}

void traverse(pANTLR3_BASE_TREE node)
{
    printf("Token: %d\n", getTokenType(node));
    handle(node);
    for(int i = 0; i < getNodeChildCount(node); i++) {
        //traverse((pANTLR3_BASE_TREE)node->getChild(node, i));
    }
}

int main(int argc, char * argv[])
{
    argc = argc;
    argv = argv;

    InitializeNativeTarget();

    pANTLR3_UINT8 fileName = (pANTLR3_UINT8) "example.rpp";
    pANTLR3_INPUT_STREAM input = antlr3AsciiFileStreamNew(fileName);
    prppLexer lexer = rppLexerNew(input);
    pANTLR3_COMMON_TOKEN_STREAM tokenStream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lexer));
    prppParser parser = rppParserNew(tokenStream);

    rppParser_prog_return prog;
    prog = parser->prog(parser);

    if (parser->pParser->rec->state->errorCount > 0)
    {
        fprintf(stderr, "The parser returned %d errors, tree walking aborted.\n", parser->pParser->rec->state->errorCount);
    } else {
        pANTLR3_COMMON_TREE_NODE_STREAM nodes;
        nodes = antlr3CommonTreeNodeStreamNewTree(prog.tree, ANTLR3_SIZE_HINT);


        printf("Nodes: %s\n", prog.tree->toStringTree(prog.tree)->chars);

        //printf("Token: %d\n", prog.tree->getToken(prog.tree)->type);
        traverse(prog.tree);
        /*while(nodes->hasNext(nodes)) {
            pANTLR3_BASE_TREE node = nodes->next(nodes);
            printf("Node type: %d", node->getType(node));
        }*/
    }

    parser->free(parser);
    tokenStream->free(tokenStream);
    lexer->free(lexer);
    input->free(input);

    return 0;
}

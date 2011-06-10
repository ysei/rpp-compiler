#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <llvm/Target/TargetSelect.h>
#include "ast.hpp"
#include "rppLexer.h"
#include "rppParser.h"
#include <antlr3basetree.h>
#include <antlr3basetreeadaptor.h>

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

    /*
    pANTLR3_COMMON_TOKEN t;
    do
    {
        t = TOKENSOURCE(lexer)->nextToken(TOKENSOURCE(lexer));

        if (t != NULL)
        {
            printf(" %s", t->getText(t)->chars);
        }
    }
    while (t == NULL || t->getType(t) != ANTLR3_TOKEN_EOF);
*/
    rppParser_prog_return prog;
    prog = parser->prog(parser);

    if (parser->pParser->rec->state->errorCount > 0)
    {
        fprintf(stderr, "The parser returned %d errors, tree walking aborted.\n", parser->pParser->rec->state->errorCount);
    } else {
        pANTLR3_COMMON_TREE_NODE_STREAM nodes;
        nodes = antlr3CommonTreeNodeStreamNewTree(prog.tree, ANTLR3_SIZE_HINT);

        printf("Nodes: %s\n", prog.tree->toStringTree(prog.tree)->chars);
    }

    parser->free(parser);
    tokenStream->free(tokenStream);
    lexer->free(lexer);
    input->free(input);

    return 0;
}

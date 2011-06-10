#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <llvm/Target/TargetSelect.h>
#include "ast.hpp"
#include "rppLexer.h"
#include "rppParser.h"

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
    parser->prog(parser);

    parser->free(parser);
    tokenStream->free(tokenStream);
    lexer->free(lexer);
    input->free(input);


    //scan_buffer("if 20 + 30 begin 2 + 3 else 10 + 20 end");
    //scan_buffer("if 0 begin 30 else 40 end");
    /*
    scan_buffer("x = 10");
    yyparse();
    cout << programBlock << endl;
    CodeGenContext context;
    context.generateCode(*programBlock);
    context.runCode();
    */
    return 0;
}

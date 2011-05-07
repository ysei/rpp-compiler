#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <llvm/Target/TargetSelect.h>
#include "ast.hpp"
using namespace std;

int yylex();
extern int yyparse();
extern NBlock * programBlock;
void scan_buffer(const char * buf);

int main(int argc, char * argv[])
{
    argc = argc;
    argv = argv;
    InitializeNativeTarget();
    //scan_buffer("if 20 + 30 begin 2 + 3 else 10 + 20 end");
    scan_buffer("if 0 begin 30 else 40 end");
    yyparse();
    cout << programBlock << endl;
    CodeGenContext context;
    context.generateCode(*programBlock);
    context.runCode();

    return 0;
}

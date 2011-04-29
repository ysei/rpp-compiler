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
    InitializeNativeTarget();
    scan_buffer("2 + 3");
    yyparse();
    cout << programBlock << endl;
    CodeGenContext context;
    context.generateCode(*programBlock);
    context.runCode();
    
    return 0;
}
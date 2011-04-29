#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <llvm/Target/TargetSelect.h>
#include "ast.hpp"
using namespace std;

int yylex();
extern int yyparse();
extern NBlock * programBlock;

int main(int argc, char * argv[])
{
    yyparse();
    cout << programBlock << endl;
    InitializeNativeTarget();
    CodeGenContext context;
    context.generateCode(*programBlock);
    context.runCode();
    
    return 0;
}
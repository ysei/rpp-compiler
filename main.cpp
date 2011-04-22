#include <stdio.h>
#include <stdlib.h>

int yylex();
extern int yyparse();

int main(int argc, char * argv[])
{
    yyparse();
}
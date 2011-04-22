#include <stdio.h>
#include <stdlib.h>

extern "C" int yylex();

int main(int argc, char * argv[])
{
    yylex();
}
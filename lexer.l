%{
#include <string.h>
int yywrap() { }
%}

%%
[ \t\n]                 ;
[0-9]+                  printf("Parsed: %s\n", yytext); return 0;
%%
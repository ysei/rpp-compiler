%{
#include <string.h>
extern "C" int yywrap(void) { }
%}

%%
[ \t\n]                 ;
[0-9]+                  printf("Parsed: %s\n", yytext); return 0;
%%
%{
    #include <stdio.h>
    #include <stdlib.h>

    extern int yylex();

    void yyerror(const char *s)
    {
        printf("Error: %s\n", s);
        exit(1);
    }
%}

%start program

%%

program :
        ;
%%
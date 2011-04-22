%{
    #include <stdio.h>
    #include <string>

    extern int yylex();

    void yyerror(const char *s)
    {
        printf("Error: %s\n", s);
        exit(1);
    }
%}

%union {
    int token;
    std::string *string;
    void *expr;
}

%token <string> TIDENTIFIER TINTEGER TDOUBLE
%token <token> TCEQ TCNE TCLT TCLE TCGT TCGE TEQUAL
%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TCOMMA TDOT
%token <token> TPLUS TMINUS TMUL TDIV

%type <expr> numeric expr
%type <token> comparison

%left TPLUS TMINUS
%left TMUL TDIV

%start program

%%

program : expr
        ;

expr    : expr comparison expr { printf("Found operator\n"); }
        | numeric { printf("Found number\n"}
        ;

comparison : TCEQ | TCNE | TCLT | TCLE | TCGT | TCGE
           | TPLUS | TMINUS | TMUL | TDIV
           ;

numeric : TINTEGER
        | TDOUBLE
        ;
%%
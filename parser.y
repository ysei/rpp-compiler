%{
    #include <stdio.h>
    #include <string>
    #include "ast.hpp"
    #include "global.hpp"

    NBlock * programBlock;

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
    NBlock *block;
    NStatement *stmt;
    NExpression *expr;
}

%token <string> TIDENTIFIER TINTEGER TDOUBLE
%token <token> TCEQ TCNE TCLT TCLE TCGT TCGE TEQUAL
%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TCOMMA TDOT
%token <token> TPLUS TMINUS TMUL TDIV
%token <token> TIF TELSE TBEGIN TEND TNEWLINE

%type <expr> numeric expr
%type <token> comparison
%type <block> program stmts
%type <stmt> stmt ifstmt

%left TPLUS TMINUS
%left TMUL TDIV

%start program

%%

program : stmts { programBlock = $1; }
        ;

stmts   : stmt { $$ = new NBlock(); $$->statements.push_back($<stmt>1); }
        ;

ifstmt  : TIF stmt TBEGIN stmt TELSE stmt TEND { $$ = new NIfStatement(*$2, *$4, *$6); printf("Hello\n"); }
        ;

stmt    : expr { $$ = new NExpressionStatement(*$1); }
        | ifstmt
        ;

expr    : numeric { $$ = $1; }
        | expr comparison expr { $$ = new NBinaryOperator(*$1, $2, *$3); }
        | TLPAREN expr TRPAREN  { $$ = $2; }
        ;

comparison : TCEQ | TCNE | TCLT | TCLE | TCGT | TCGE
           | TPLUS | TMINUS | TMUL | TDIV
           ;

numeric : TINTEGER { $$ = new NInteger(atol($1->c_str())); delete $1; }
        | TDOUBLE { $$ = new NDouble(atof($1->c_str())); delete $1; }
        ;
%%

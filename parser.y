%{
    #include <stdio.h>
    #include <string>
    #include "ast.hpp"
    
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

%type <expr> numeric expr
%type <token> comparison
%type <block> program stmts
%type <stmt> stmt

%left TPLUS TMINUS
%left TMUL TDIV

%start program

%%

program : stmts { programBlock = $1; }
        ;
        
stmts   : stmt { $$ = new NBlock(); $$->statements.push_back($<stmt>1); }
        ;

stmt    : expr { $$ = new NExpressionStatement(*$1); }
        ;

expr    : expr comparison expr { printf("Found operator\n"); }
        | numeric { printf("Found number\n"); }
        ;

comparison : TCEQ | TCNE | TCLT | TCLE | TCGT | TCGE
           | TPLUS | TMINUS | TMUL | TDIV
           ;

numeric : TINTEGER
        | TDOUBLE
        ;
%%
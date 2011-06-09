grammar rpp;

options
{
    output  = AST;
    //language = Java;
    language = C;

    // If generating an AST (output=AST; option) or specifying a tree walker then
    // also add the following line
    //
    ASTLabelType=CommonTree;
}

tokens
{
	BLOCK;
	FUNC_DEF;
	FUNC_DECL;
	ARG_DEF;
	VAR_DEF;
	FOR_DEF;
	RANGE;
}

prog	:	(prog_entity)*
	;

prog_entity
	:	func_decl
	|	NEWLINE!
	;

func_decl
	:	DEF ID param_list? NEWLINE block END -> ^(FUNC_DEF ID ^(ARG_DEF param_list)? ^(BLOCK block?))
	;

param_list
	:	 '(' ID (',' ID)* ')' -> ID+
	;

block	:	(stat? NEWLINE)* -> stat?
	;

stat	:	expr NEWLINE -> expr
	|	assignment
	|	ifstmt
	|	return_stmt
	|	for_stmt
	;

for_stmt:	FOR ID IN '('? range_start '..' range_end ')'? block END -> ^(FOR ID ^(RANGE range_start range_end) ^(BLOCK block))
	|	FOR boolexpr NEWLINE block END -> ^(FOR boolexpr ^(BLOCK block))
	;

range_start
	:	(INT|ID)
	;

range_end:	(INT|ID)
	;
	
return_stmt
	:	RETURN expr -> ^(RETURN expr)
	;
	
ifstmt	:	IF boolx NEWLINE block (ELSE else_stmt=block)? END -> ^(IF boolx ^(BLOCK block) ^(BLOCK $else_stmt)?)
	;

assignment
	:	ID '='^ expr
	;

boolexpr
	:	expr (('=='^ | '||'^ | '&&'^ | '<'^ | '>'^ | '<=' | '>=') expr)?
	;
	
boolx	:	eqexpr (('&&'^ | '||'^) eqexpr)*
	;

eqexpr	:	boolatom (('=='^ | '>'^ | '<'^ | '<='^ | '>=') boolatom)*
	;

boolatom:	INT
	|	ID
	|	'('! boolx ')'!
	;

expr 	: 	multexpr (('+'^ | '-'^) multexpr)*
	;

multexpr:	atom (('*'^ | '/'^) atom)*
	;

atom	:	INT
	|	'('! expr ')'!
	|	ID ('(' expr_list? ')')?
	;
expr_list
	:	expr (',' expr)*
	;

BEGIN	:	'begin'
	;
	 
DEF 	:	'def'
	;
END	:	'end'
	;
RETURN	:	'return'
	;
IF	:	'if'
	;
ELSE	:	'else'
	;
FOR	:	'for'
	;
IN	:	'in'
	;

ID  :	('a'..'z'|'A'..'Z'|'_') ('a'..'z'|'A'..'Z'|'0'..'9'|'_')*
    ;

INT :	'0'..'9'+
    ;

NEWLINE : ( '\r\n' // DOS
	| '\r' // MAC
        | '\n' // Unix
        )
        ;

WS  :   ( ' '
        | '\t'
        ) {$channel=HIDDEN;}
    ;


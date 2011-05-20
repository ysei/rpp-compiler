grammar SimpleCalc;

options
{
    output  = AST;
    language = Java;
    //language = C;

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
	
}

prog	:	func_decl*
	;

func_decl
	:	DEF! ID^ param_list? stat* END!
	;

param_list
	:	 '('! ID! (','! ID!)* ')'!
	;

stat	:	expr ';'!
	|	assignment
	|	ifstmt
	|	return_stmt
	|	for_stmt
	;

for_stmt:	FOR! ID IN! '('? (INT|ID) '..' (INT|ID) ')'? stat* END
	;

return_stmt
	:	RETURN expr ';'
	;
	
ifstmt	:	IF^ expr ';' stat* (ELSE! stat*)? END!
	;

assignment
	:	ID '='! expr ';'
	;

expr 	: 	multexpr (('+'^ | '-'^) multexpr)*
	;

multexpr:	atom (('*'^ | '/'^) atom)*
	;

atom	:	INT
	|	'(' expr ')'
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
   
NEWLINE	: '\n'
	;

WS  :   ( ' '
        | '\t'
        | '\r'
        | '\n'
        ) {$channel=HIDDEN;}
    ;


grammar Rpp;

options
{
    output  = AST;
    //language = Java;
    language = C;

    // If generating an AST (output=AST; option) or specifying a tree walker then
    // also add the following line
    //
    ASTLabelType=pANTLR3_BASE_TREE;
}

prog 	: func_decl*
	;

func_decl 
	: DEF!  ID^ paramList? stat* END!
	;

paramList
	: '('! ID! (','! ID!)+ ')'!	
	;
	
stat	: 	expr
	|	assignment
	|	returnstmt
	|	ifstmt
	;
	
ifstmt	:	IF^ expr stmts (ELSE! stmts)? END!
	;
	
stmts	:	stat+ -> ^(BEGIN stat+)
	;
	
returnstmt  :	RETURN^ expr^
	;
	
assignment
	:	ID '='^ expr
	;
	
expr	:	multexpr (('+'^ | '-'^) multexpr)*
	;
	
multexpr:	atom (('*'^ | '/'^) atom)*
	;

atom	:	INT
	|	'('! expr ')'!
	|	ID
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

ID	:	('a'..'z'|'A'..'Z'|'_') (('a'..'z'|'A'..'Z'|'_'|'0'..'9'))*
	;

INT :	'0'..'9'+
    ;

FLOAT
    :   ('0'..'9')+ '.' ('0'..'9')* EXPONENT?
    |   '.' ('0'..'9')+ EXPONENT?
    |   ('0'..'9')+ EXPONENT
    ;

WS  :   ( ' '
        | '\t'
        | '\r'
        | '\n'
        ) {$channel=HIDDEN;}
    ;

fragment
EXPONENT : ('e'|'E') ('+'|'-')? ('0'..'9')+ ;


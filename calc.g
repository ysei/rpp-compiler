grammar calc;

options
{
	output = AST;
	language = Java;
}

prog: expression_list
	;

expression_list
	:	expression (NEWLINE expression)*
	;
	
expression
	:	additiveExpression
	;

additiveExpression
	:	multiplicativeExpression ( '+' multiplicativeExpression |Ê'-' multiplicativeExpression)*
	;

multiplicativeExpression
	:	unaryExpression ( '*' unaryExpression |Ê'/' unaryExpression)*
	;
	
unaryExpression
	:	ID '(' expression_list_by_comma?  ')'
	|	ID '[' additiveExpression ']'		
	|	primaryExpression
	|	'(' additiveExpression ')'
	;
	
expression_list_by_comma
	:	expression (',' expression)*
	;
	
primaryExpression
	:	ID
	|	INT
	|   FLOAT
	;
	
argument_expression_list
	: additiveExpression (',' additiveExpression)*
	;
	
NEWLINE
	:	'\r'
	|	'\n'
	|	'\r\n'
	;

ID  :	('a'..'z'|'A'..'Z'|'_') ('a'..'z'|'A'..'Z'|'0'..'9'|'_')*
    ;

INT :	'0'..'9'+
    ;

FLOAT
    :   ('0'..'9')+ '.' ('0'..'9')* EXPONENT?
    |   '.' ('0'..'9')+ EXPONENT?
    |   ('0'..'9')+ EXPONENT
    ;

fragment
EXPONENT : ('e'|'E') ('+'|'-')? ('0'..'9')+ ;


grammar Rpp;

options
{
	output = AST;
	language = Java;
}

@parser::header {
package org.dubikdev.toycompiler;
}

@lexer::header{
package org.dubikdev.toycompiler;
}

prog: expressions
	;

constant_expression
	:	conditional_expression
	;

expressions
	:	(NEWLINE!)* expression (NEWLINE! (expression)?)*
	;

expression
	:	assignment
	|	conditional_expression
	;

conditional_expression
	: logical_or_expression
	;

logical_or_expression
	: logical_and_expression ('||'^ logical_and_expression)*
	;

logical_and_expression
	: inclusive_or_expression ('&&'^ inclusive_or_expression)*
	;

inclusive_or_expression
	: exclusive_or_expression ('|'^ exclusive_or_expression)*
	;

exclusive_or_expression
	: and_expression ('^'^ and_expression)*
	;

and_expression
	: equality_expression ('&'^ equality_expression)*
	;
equality_expression
	: relational_expression (('=='^ | '!='^ ) relational_expression)*
	;

relational_expression
	: shift_expression (('<'^ | '>'^ | '<='^ | '>='^ ) shift_expression)*
	;

shift_expression
	: additiveExpression (('<<'^| '>>'^) additiveExpression)*
	;

additiveExpression
	:	multiplicativeExpression ( '+'^ multiplicativeExpression | '-'^ multiplicativeExpression)*
	;

multiplicativeExpression
	:	unaryExpression ( '*'^ unaryExpression | '/'^ unaryExpression)*
	;

unaryExpression
	:	ID '(' expression_list_by_comma?  ')'
	|	ID '[' additiveExpression ']'
	|	primaryExpression
	|	'('! conditional_expression ')'!
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

assignment
	:	ID '='^ expression
	;
lvalue
	:	UNARY_OPERATOR
	;

UNARY_OPERATOR
	:	'-'
	|	'!'
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

ASSIGNMENT_OPERATOR
	:	'='
	;

fragment
EXPONENT : ('e'|'E') ('+'|'-')? ('0'..'9')+ ;

WHITESPACE : ( '\t' | ' ' | '\u000C' )+ 	{ $channel = HIDDEN; } ;


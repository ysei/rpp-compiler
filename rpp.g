grammar rpp;

options
{
	output = AST;
	language = C;
	k = 3;
}


tokens {
	BLOCK;
	FUNC_RETURN_TYPE;
	FUNC_PARAMS;
	FUNC_PARAM;
	FUNC_BODY;
	FUNC_DEF;
	IF;
	THEN;
	ELSE;
	WHILE;
	RETURN;
	ASSIGNMENT;
	BINOP;
}
/*
@parser::header {
package org.dubikdev.toycompiler;
}

@lexer::header{
package org.dubikdev.toycompiler;
}
*/
prog: NEWLINE!* ( progElem ((NEWLINE!)+ | EOF!))*
	;

progElem
	:	statement
	|	funcDecl
	;

statement
	: expression	
	;

funcDecl
	: 'def'	RPP_ID ('(' funcParamDecl (',' funcParamDecl)* ')' )? funcReturnDecl? NEWLINE+ funcBody? 'end' -> ^(FUNC_DEF RPP_ID ^(FUNC_PARAMS funcParamDecl*) ^(FUNC_RETURN_TYPE funcReturnDecl?) funcBody?)
	;

funcParamDecl
	: TYPE RPP_ID -> ^(FUNC_PARAM TYPE RPP_ID)
	;

funcReturnDecl
	: ':'! TYPE^
	;

funcBody
	: funcBodyElement+ -> ^(BLOCK funcBodyElement+)
	;
	
funcBodyElement
	: assignment NEWLINE!+
	| conditional_expression NEWLINE!+
	| 'return' constant_expression NEWLINE+ -> ^(RETURN constant_expression)
	| if_stmt NEWLINE!+
	| while_stmt NEWLINE!+
	;	

if_stmt
	:	'if' expression NEWLINE+ thenBody=funcBody? ('else' NEWLINE+ elseBody=funcBody?)? 'end'
			-> ^(IF expression ^(THEN $thenBody?) ^(ELSE $elseBody?)?)
	;

while_stmt
	: 'while' expression NEWLINE+ funcBody? 'end' -> ^(WHILE expression ^(BLOCK funcBody?))
	;	
	
range_boundary
	:	(RPP_ID | INT)
	;
	
constant_expression
	:	conditional_expression
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
	:	multiplicativeExpression ( ( '+'^ | '-'^ ) multiplicativeExpression)*
		;

multiplicativeExpression
	:	unaryExpression ( '*'^ unaryExpression | '/'^ unaryExpression)*
	;

unaryExpression
	:	RPP_ID '(' expression_list_by_comma?  ')'
	|	RPP_ID '[' additiveExpression ']'
	|	primaryExpression
	|	'('! conditional_expression ')'!
	;

expression_list_by_comma
	:	expression (',' expression)*
	;

primaryExpression
	:	RPP_ID
	|	INT
	|       FLOAT
	|	STRING
	;

argument_expression_list
	: additiveExpression (',' additiveExpression)*
	;

assignment
	:	RPP_ID '=' expression -> ^(ASSIGNMENT RPP_ID expression)
	;
lvalue
	:	UNARY_OPERATOR
	;

TYPE	:	'int'
	|	'float'
	|	'boolean'
	|	'String'
	|	'Array'
	|	'Hash'
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

RPP_ID  :	('a'..'z'|'A'..'Z'|'_') ('a'..'z'|'A'..'Z'|'0'..'9'|'_')*
    ;

INT :	'0'..'9'+
    ;

FLOAT
    :   ('0'..'9')+ '.' ('0'..'9')* EXPONENT?
    |   '.' ('0'..'9')+ EXPONENT?
    |   ('0'..'9')+ EXPONENT
    ;

/** Match various string types.  Note that greedy=false implies '''
 *  should make us exit loop not continue.
 */
STRING
    :   ('r'|'u'|'ur')?
        (   '\'\'\'' (options {greedy=false;}:.)* '\'\'\''
        |   '"""' (options {greedy=false;}:.)* '"""'
        |   '"' (ESC|~('\\'|'\n'|'"'))* '"'
        |   '\'' (ESC|~('\\'|'\n'|'\''))* '\''
        )
	;

fragment
ESC
	:	'\\' .
	;
   	
ASSIGNMENT_OPERATOR
	:	'='
	;

fragment
EXPONENT : ('e'|'E') ('+'|'-')? ('0'..'9')+ ;

WHITESPACE : ( '\t' | ' ' | '\u000C' )+ 	{ $channel = HIDDEN; } ;


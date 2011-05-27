grammar ruby;

options {
	k = 6; // set to 6 the token lookahead
    buildAST = true;
}

// begin Ruby Program
program 
    :   ( class_def (NEWLINE)+  | statement (NEWLINE)+)* 
    ;

class_def
    :   'class'^ 
        class_name
        (NEWLINE 
        | '<' class_parent 
        ('<' class_parent
        )* NEWLINE )
        class_body
        'end'
    ;

class_name
    :   IDENTIFIER 
    ;

class_parent
    :   IDENTIFIER
    ;

class_body
    : ( (statement NEWLINE  ) | ( method_def NEWLINE) )+
    ;

method_def
    :   'def'^ 
        (class_name '.')? method_name
        ((method_par)*
        |( '(' method_par (',' method_par )* ')' )* )
        ( NEWLINE )+
        method_body
        'end'
    ;

method_name
    :   IDENTIFIER
    ;

method_par
    :   IDENTIFIER
    ;

method_body
    : (statement NEWLINE  )*
    ;

// Instruction in the program body
statement
    :   // if
          'if'^ 
          '(' c=condition ')' NEWLINE b=block 'end' 
	| 'break if'^ 
          '(' c=condition ')' 
	| // loop do
          'loop' 'do'^ NEWLINE loopBody=block 'end'
	| // yield
          'yield'^
          '(' operando ')' 
        | // return
          'return'^ (operando  | '('operando ')' )
        | // return
          'return'^ (booleani | '(' booleani ')' )
        | // puts
          'puts'^ statement 
	| // id = (number | id | string)
          (DAT IDENTIFIER) 
          '='^ (operando | '(' operando_par ')' )
        | // id = id (id, id, ...)
          (DAT IDENTIFIER)
          '='^ 
          (class_name '.' method_name)
          ( ('(' ')') | ( '(' 
          opAsClassMet=operando 
          (',' 
          op1AsClassMet=operando )* 
          ')' ) )?
        | // id = id (id, id, ...)
          (DAT IDENTIFIER)
          '='^ 
          method_name
          ( ('(' ')') | ( '(' 
          operando
          (',' 
          operando )* 
          ')' ) )?
        | // id = (number | id | string)
	  (AT IDENTIFIER) 
          '='^ (operando | '(' operando ')' )
        | // id = id (id, id, ...)
          (AT iAsClassMetF:IDENTIFIER)
          '='^ 
          (class_name '.' method_name)
          ( ('(' ')') | ( '(' 
          operando 
          (',' 
          operando )* 
          ')' ) )?
        | // id = id (id, id, ...) 
          (AT IDENTIFIER)
          '='^ 
          method_name
          ( ('(' ')') | ( '(' 
          operando 
          (',' 
          operando )* 
          ')' ) )?
        | // id = (number | id | string)
          IDENTIFIER  
          '='^ (operando  | '(' operando ')' )
        | // id = id (id, id, ...)
          IDENTIFIER 
          '='^ 
          (class_name '.' method_name)
          ( ('(' ')')| ( '(' 
          opAsClassMet=operando 
          (',' 
          op1AsClassMet=operando)* 
          ')' ) )?
        | // id = id (id, id, ...)
          IDENTIFIER 
          '='^ 
          method_name
          ( ('(' ')')  | ( '(' 
          operando 
          (',' 
          operando )* 
          ')' ) )?
        | operando 
    ;

block
    :   (s=statement NEWLINE  )+
    ;
	   
// condition or operation 
condition
    :   // condition operator (IDENTIFIER | NUMBER)
          booleani 
        | booleani ('&'^ 
                    | '|'^) 
          booleani  
        | '(' condition ')' 
        | booleani '|'^
          condition 
        | booleani '&'^ booleani ('&'^
                               | '|'^) 
        condition
    ;

// math instruction
instruction returns
    :     '(' operando_par ')'  ( '/'^ operando
                                | '*'^  operando  
                                | '+'^  operando 
                                | '-'^ operando  )?
        | '(' instruction_par ')' ( '/'^  operando
                                  | '*'^ operando
                                  | '+'^ operando
                                  | '-'^ operando )?
        | (DAT IDENTIFIER) ( '/'^ operando 
                           | '*'^ operando 
                           | '+'^ operando 
                           | '-'^ operando  )
        | (AT IDENTIFIER) ( '/'^ operando 
                          | '*'^ operando 
                          | '+'^ operando 
                          | '-'^ operando )
        | IDENTIFIER ( '/'^ operando 
                     | '*'^ operando
                     | '+'^ operando 
                     | '-'^ operando  )
        | //NUMBER operator NUMBER
          n:NUMBER ( '/'^ operando
                   | '*'^ operando 
                   | '+'^ operando
                   | '-'^ operando)
    ;

instruction_par
    :   instruction  
    ;

operando_par
    :   operando  
    ;

// 'return'
operando
    :     NUMBER 
        | (DAT IDENTIFIER) 
        | (AT IDENTIFIER) 
        | IDENTIFIER 
        | class_name '.' 'new' ( '.' method_name  ('.' method_name)* )?
          ( '(' 
          operando 
          (',' 
          operando 
          )* ')' )?
        | // id((id | number),(id|number)*) do |id|
          (class_name '.' method_name)
          ( ('(' ')')  | ( '(' 
          operando 
          (',' 
          operando  )* 
          ')' ) )?
          ( 'do' '|' IDENTIFIER 
                 '|' NEWLINE 
                  block
                  'end')?
        | // id((id | number),(id|number)*) do |id|
          method_name
          ( ('(' ')')  | ( '(' 
          operando
          (',' 
          operando )* 
          ')' ) )?
	  ( 'do' '|' IDENTIFIER 
                 '|' NEWLINE 
                 block
                 'end')?
        | instruction 
        | '(' instruction_par ')' 
    ;

booleani
    :     'true' 
        | 'false'
        | operando ( '<'^ operando
                   | '<='^ operando
                   | '>='^ operando 
                   | '>'^  operando
                   | '=='^ operando  )
      
    ;


WS	: ' '{ $channel=HIDDEN; }
	;

LPAREN  : '('
	;

RPAREN  : ')'
	;

LT      : '<'
        ;
LE      : '<='
        ;
GE      : '>='
        ;
GT      : '>'
        ;
EGUAL   : '=='
        ;
DIV     : '/'
        ;
MUL     : '*'
        ;
ASSIGN  : '='
        ;
PLUS    : '+'
        ;
OR      : '|' 
        ;  
AND     : '&'
        ;
SUB     : '-'
        ;
MOD     : '%'
        ;
NUMBER  : ('0'..'9')+ 
        ;
POINT   : ('.')+
        ;
AT      : '@'
        ;
DAT     : '@@'
        ;
	 
// Id
IDENTIFIER : ('a'..'z'|'A'..'Z')+ (NUMBER)*
           ;

// new line
NEWLINE : ( '\r\n' // DOS
        | '\r' // MAC
        | '\n' // Unix
        );
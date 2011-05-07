%{
#include <string>
#include "ast.hpp"
#include "parser.hpp"
#include "global.hpp"

extern "C" int yywrap(void) { }
#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
#define TOKEN(t) (yylval.token = t)
void scan_buffer(const char * buf)
{
    yy_scan_string(buf);
}
%}

%%
"if"                    return TOKEN(TIF);
"end"                   return TOKEN(TEND);
"begin"                 return TOKEN(TBEGIN);
"else"                  return TOKEN(TELSE);
"\n"                    /* if(rpp::doNewLine) { rpp::doNewLine = false; return TOKEN(TNEWLINE); } */
[ \t]                   ;
[a-zA-Z_][a-zA-Z0-9_]*  SAVE_TOKEN; return TIDENTIFIER;
[0-9]+\.[0-9]*          SAVE_TOKEN; return TDOUBLE;
[0-9]+                  SAVE_TOKEN; return TINTEGER;
"="                     return TOKEN(TEQUAL);
"=="                    return TOKEN(TCEQ);
"!="                    return TOKEN(TCNE);
"<"                     return TOKEN(TCLT);
"<="                    return TOKEN(TCLE);
">"                     return TOKEN(TCGT);
">="                    return TOKEN(TCGE);
"("                     return TOKEN(TLPAREN);
")"                     return TOKEN(TRPAREN);
"{"                     return TOKEN(TLBRACE);
"}"                     return TOKEN(TRBRACE);
"."                     return TOKEN(TDOT);
","                     return TOKEN(TCOMMA);
"+"                     return TOKEN(TPLUS);
"-"                     return TOKEN(TMINUS);
"*"                     return TOKEN(TMUL);
"/"                     return TOKEN(TDIV);
.                       printf("Unknown token!\n"); yyterminate();
%%

%{

#include <stdio.h>
#include "y.tab.h"

%}

%%

"//"[^\n]*              { /* consume // - comments */ }
[ \t\n]+                { /* consume whitespace characters */ }

"return"                { return TOK_RETURN; }
"int"                   { yylval.string=strdup(yytext); return TOK_TYPE; }
"main"                  { return TOK_IDENTIFIER; }
"cout"                  { return TOK_COUT; }
"{"                     { return TOK_LBRACE; }
"}"                     { return TOK_RBRACE; }
"("                     { return TOK_LPAREN; }
")"                     { return TOK_RPAREN; }
"+"                     { return TOK_ADD; }
"-"                     { return TOK_SUB; }
"*"                     { return TOK_MUL; }
"/"                     { return TOK_DIV; }
"!"                     { return TOK_NEG; }
";"                     { return TOK_SEMI; }
"="                     { return TOK_EQUALS; }
"<"                     { return TOK_LBRACKET; }
[0-9]+                  { yylval.number=atoi(yytext); return TOK_UINT; }
[a-zA-Z_]+              { yylval.string=strdup(yytext); return TOK_IDENTIFIER; }

%%

int yywrap() {
    return 1;
}
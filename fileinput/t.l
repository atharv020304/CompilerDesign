%{
    #include "parser.tab.h"  
    #include <stdlib.h>
    #include <string.h>
    #include"symbol_table.h"
    
%}

%option noyywrap

%%

[ \t]+                  ;  
"+" { return '+'; }
"-" { return '-'; }
"*" { return '*'; }
"/" { return '/'; }
"del" {return DELETE; }

\n                      { return EOL; }  
"print"                 { return PRINT; }
"class"                 { return CLASS; }
[0-9]+\.[0-9]+          { yylval.fval = atof(yytext); return FLOAT_VALUE; }  
[0-9]+                  { yylval.ival = atoi(yytext); return INT_VALUE; }
"'"[^\']"'"             { yylval.cval = yytext[1]; return CHAR_VALUE; }  
\"[^\"]+\"              { yylval.sval = strdup(yytext + 1); yylval.sval[strlen(yylval.sval) - 1] = '\0'; return STRING_VALUE; }  
[a-zA-Z_][a-zA-Z0-9_]*  { yylval.sval = strdup(yytext); return IDENTIFIER; }
"="                     { return ASSIGN; }
";"                     { return SEMICOLON; }
":"                     {return COLON; }
","                     { return COMMA;}
"("                     { return '('; }  
")"                     { return ')'; }
"^"                     {return '^'; }
.                       { printf("Unknown token: %s\n", yytext); return yytext[0]; }  
%%



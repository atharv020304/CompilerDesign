%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "symbol_table.h"

    extern int yylex();
    void yyerror(const char *s);


%}

%union {
    int ival;
    float fval;
    char cval;
    char *sval;
    void *ptr;  
    struct Data** dt;
}

%token PRINT ASSIGN SEMICOLON COLON COMMA EOL
%token CLASS
%token <ival> INT_VALUE
%token <fval> FLOAT_VALUE
%token <cval> CHAR_VALUE
%token <sval> STRING_VALUE IDENTIFIER
%token DELETE
%type <ptr> expression
%type <dt> attribute_list
%type <sval> CLS_DEC
%left '+' '-' 
%left '*' '/'

%%

program:
    program statement EOL { }
    | statement EOL { }
    ;

statement:
    class_definition
    | object_instantiation
    | IDENTIFIER ASSIGN expression SEMICOLON {
        insertSymbol($1, $3);
    }
    | IDENTIFIER '^' IDENTIFIER ASSIGN expression SEMICOLON {
        setAttributeValue($1, $3, $5);
    }
    | PRINT IDENTIFIER SEMICOLON {
        printSymbol($2);
    }
    | PRINT SEMICOLON {
        printSymbolTable();
    }
    ;


class_definition:
    CLS_DEC attribute_list  { createAttributes($1, $2); } 
    ;

CLS_DEC:
    CLASS IDENTIFIER COLON {
        createClass($2);
        $$ = $2;  /* Store the class name */
    }
    ;

attribute_list:
    IDENTIFIER {
        addAttribute($$, $1); 
    }
    | attribute_list COMMA IDENTIFIER {
        addAttribute($$, $3);
    }
    ;



object_instantiation:
    IDENTIFIER ASSIGN IDENTIFIER '(' ')' SEMICOLON {
        createObject($3,$1);
    }
    ;


expression:
    INT_VALUE {
        int* val = malloc(sizeof(int));
        *val = $1;
        $$ = createData(val, INT_TYPE);
    }
    | FLOAT_VALUE {
        float* val = malloc(sizeof(float));
        *val = $1;
        $$ = createData(val, FLOAT_TYPE);
    }
    | CHAR_VALUE {
        char* val = malloc(sizeof(char));
        *val = $1;
        $$ = createData(val, CHAR_TYPE);
    }
    | STRING_VALUE {
        char* val = strdup($1);
        $$ = createData(val, STRING_TYPE);
    }
    | IDENTIFIER {
        $$ = getSymbolValue($1);
    }
    | expression '+' expression {
        Data* data1 = (Data*)$1;
        Data* data2 = (Data*)$3;
        if (data1->type == INT_TYPE && data2->type == INT_TYPE) {
            int* val = malloc(sizeof(int));
            *val = *(int*)data1->value + *(int*)data2->value;
            $$ = createData(val, INT_TYPE);
        } else if (data1->type == FLOAT_TYPE && data2->type == FLOAT_TYPE) {
            float* val = malloc(sizeof(float));
            *val = *(float*)data1->value + *(float*)data2->value;
            $$ = createData(val, FLOAT_TYPE);
        } else {
            yyerror("Type mismatch in addition");
        }
    }
    | expression '-' expression {
        Data* data1 = (Data*)$1;
        Data* data2 = (Data*)$3;

        if (data1->type == INT_TYPE && data2->type == INT_TYPE) {
            int* val = malloc(sizeof(int));
            *val = *(int*)data1->value - *(int*)data2->value;
            $$ = createData(val, INT_TYPE);
        } else if (data1->type == FLOAT_TYPE && data2->type == FLOAT_TYPE) {
            float* val = malloc(sizeof(float));
            *val = *(float*)data1->value - *(float*)data2->value;
            $$ = createData(val, FLOAT_TYPE);
        } else {
            yyerror("Type mismatch in subtraction");
        }
    }
    | expression '*' expression {
        Data* data1 = (Data*)$1;
        Data* data2 = (Data*)$3;

        if (data1->type == INT_TYPE && data2->type == INT_TYPE) {
            int* val = malloc(sizeof(int));
            *val = *(int*)data1->value * *(int*)data2->value;
            $$ = createData(val, INT_TYPE);
        } else if (data1->type == FLOAT_TYPE && data2->type == FLOAT_TYPE) {
            float* val = malloc(sizeof(float));
            *val = *(float*)data1->value * *(float*)data2->value;
            $$ = createData(val, FLOAT_TYPE);
        } else {
            yyerror("Type mismatch in multiplication");
        }
    }
    | expression '/' expression {
        Data* data1 = (Data*)$1;
        Data* data2 = (Data*)$3;

        if (data1->type == INT_TYPE && data2->type == INT_TYPE) {
            if (*(int*)data2->value == 0) {
                yyerror("Division by zero error");
            }
            int* val = malloc(sizeof(int));
            *val = *(int*)data1->value / *(int*)data2->value;
            $$ = createData(val, INT_TYPE);
        } else if (data1->type == FLOAT_TYPE && data2->type == FLOAT_TYPE) {
            if (*(float*)data2->value == 0.0f) {
                yyerror("Division by zero error");
            }
            float* val = malloc(sizeof(float));
            *val = *(float*)data1->value / *(float*)data2->value;
            $$ = createData(val, FLOAT_TYPE);
        } else {
            yyerror("Type mismatch in division");
        }
    }
    ;


%%

void yyerror(const char *s) {
    extern char *yytext;   
    extern int yylineno;
    fprintf(stderr, "Syntax Error at line %d: %s. Unexpected token: '%s'\n", 
            yylineno, s, yytext);
}


int main(){
    yyparse();
    return 0;
}


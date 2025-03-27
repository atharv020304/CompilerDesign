%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "symbol_table.h"

    extern int yylex();
    void yyerror(const char *s);
     AttrNode* attrList = NULL;

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
%type <sval> attribute_list
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
    CLS_DEC attribute_list {
        pushAttrListToClass($1, attrList);
        freeAttrList(attrList);             
        attrList = NULL;
    }
    ;

CLS_DEC:
    CLASS IDENTIFIER COLON {
        createClass($2);
        $$ = strdup($2);  
    }
    ;

attribute_list:
    IDENTIFIER {
        attrList = createAttrList($1);  
    }
    | attribute_list COMMA IDENTIFIER {
        addToAttrList(&attrList, $3);  
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


int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    extern FILE *yyin;
    yyin = file;        

    yyparse();          

    fclose(file);       
    return 0;
}





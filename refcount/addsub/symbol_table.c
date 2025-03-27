#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Symbol* symbolTable = NULL;

void* createData(void* value, DataType type) {
    Symbol* sym = symbolTable;

    while (sym) {
        if (sym->data->type == type) {
            if ((type == INT_TYPE && *((int*)sym->data->value) == *((int*)value)) ||
                (type == FLOAT_TYPE && *((float*)sym->data->value) == *((float*)value)) ||
                (type == STRING_TYPE && strcmp((char*)sym->data->value, (char*)value) == 0)) {
                addRef(sym->data);
                return sym->data;
            }
        }
        sym = sym->next;
    }

    Data* data = malloc(sizeof(Data));
    data->type = type;
    data->value = value;
    data->ref_count = 0;

    return data;
}


void insertSymbol(char* name, Data* data) {
    Symbol* sym = symbolTable;
    Symbol* prev = NULL;

    while (sym) {
        if (strcmp(sym->name, name) == 0) {  
            removeRef(sym->data); 
            sym->data = data; 
            // removeRef(sym->data);  
            addRef(data);       
            return;
        }
        prev = sym;
        sym = sym->next;
    }

    sym = malloc(sizeof(Symbol));
    strcpy(sym->name, name);
    sym->data = data;
    addRef(data);  
    sym->next = symbolTable;
    symbolTable = sym;
}

Data* getSymbolValue(char* name) {
    Symbol* sym = symbolTable;
    while (sym) {
        if (strcmp(sym->name, name) == 0) {
            return sym->data;
        }
        sym = sym->next;
    }
    printf("Error: Undefined variable %s\n", name);
    return NULL;
}

void printSymbol(char* name) {
    Data* data = getSymbolValue(name);
    if (!data) return;

    printf("%s = ", name);
    switch (data->type) {
        case INT_TYPE: printf("%d\n", *(int*)data->value); break;
        case FLOAT_TYPE: printf("%f\n", *(float*)data->value); break;
        case CHAR_TYPE: printf("'%c'\n", *(char*)data->value); break;
        case STRING_TYPE: printf("\"%s\"\n", (char*)data->value); break;
    }
}

void addRef(Data* data){
    if(data){
        data->ref_count++;
    }
}

void removeRef(Data* data){
    if(data){
        data->ref_count--;
        if(data->ref_count == 0){
            free(data->value);
            free(data);
        }
    }
}
void printSymbolTable() {
    printf("\n--------------------------------------------\n");
    printf("| %-15s | %-10s | %-10s |\n", "Variable", "Value", "Ref Count");
    printf("--------------------------------------------\n");

    Symbol* sym = symbolTable;
    while (sym) {
        printf("| %-15s | ", sym->name);


        switch (sym->data->type) {
            case INT_TYPE:
                printf("%-10d | ", *(int*)sym->data->value);
                break;
            case FLOAT_TYPE:
                printf("%-10.2f | ", *(float*)sym->data->value);
                break;
            case CHAR_TYPE:
                printf("'%-8c' | ", *(char*)sym->data->value);
                break;
            case STRING_TYPE:
                printf("\"%-8s\" | ", (char*)sym->data->value);
                break;
        }

        printf("%-10d |\n", sym->data->ref_count);

        sym = sym->next;
    }

    printf("--------------------------------------------\n");
}


void deleteSymbol(char* name) {
    Symbol* sym = symbolTable;
    Symbol* prev = NULL;

    while (sym) {
        if (strcmp(sym->name, name) == 0) {
            if (prev) {
                prev->next = sym->next;
            } else {
                symbolTable = sym->next;
            }
            removeRef(sym->data);  
            free(sym);
            return;
        }
        prev = sym;
        sym = sym->next;
    }
}

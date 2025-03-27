#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { INT_TYPE, FLOAT_TYPE, CHAR_TYPE, STRING_TYPE, CLASS_TYPE ,OBJECT_TYPE} DataType;

typedef struct Data {
    DataType type;
    void* value;
    char* name;
    int size;
    struct Symbol* classSym;
} Data;

typedef struct Symbol {
    Data* data;
    struct Symbol* next;
} Symbol;

typedef struct AttrNode {
    char* name;
    struct AttrNode* next;
} AttrNode;


extern Symbol* symbolTable;

Data* createData(void* value, DataType type) ;
void insertSymbol(char* name, Data* newData) ;
Data* getSymbolValue(char* name) ;

void printSymbolTable() ;
void createAttributes(char* className, char* attrName);
void addAttribute(char* className, char* attrName);
void createObject(char* className, char* objectName);
Data* createClass(char* name);  
void setAttributeValue(char* objectName, char* attrName, Data* d);
void printSymbol(char* name);
char** createAttributeArray(char* name);
void pushAttrListToClass(char* className, AttrNode* head);
void addToAttrList(AttrNode** head, char* attrName);
AttrNode* createAttrList(char* attrName);
void freeAttrList(AttrNode* head); 


#endif
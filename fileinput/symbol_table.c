
#include "symbol_table.h"

Symbol* symbolTable = NULL;
int attrCount = 0;


Data* createData(void* value, DataType type) {
    Data* data = malloc(sizeof(Data));
    data->type = type;

    switch (type) {
        case INT_TYPE:
            data->value = malloc(sizeof(int));
            *(int*)(data->value) = *(int*)value;
            break;
        case FLOAT_TYPE:
            data->value = malloc(sizeof(float));
            *(float*)(data->value) = *(float*)value;
            break;
        case CHAR_TYPE:
            data->value = malloc(sizeof(char));
            *(char*)(data->value) = *(char*)value;
            break;
        case STRING_TYPE:
            data->value = strdup((char*)value); break;
        default:
            data->value = NULL;
    }

    data->name = strdup("");  
    return data;
}

void insertSymbol(char* name, Data* newData) {
    Symbol* sym = symbolTable;
    

    while (sym != NULL) {
        if (strcmp(sym->data->name, name) == 0) {
            sym->data = newData;
            return;
        }
        sym = sym->next;
    }

  
    Symbol* temp = malloc(sizeof(Symbol));
    temp->data = newData;
    
    temp->data->name = strdup(name); 
    temp->data->type = newData->type;
    temp->data->value = newData->value;
    
    temp->next = symbolTable;
    symbolTable = temp;
}

Data* getSymbolValue(char* name) {
    Symbol* sym = symbolTable;
    while (sym) {
        if (strcmp(sym->data->name, name) == 0) {
            return sym->data;
        }
        sym = sym->next;
    }
    printf("Undefined variable: %s\n", name);
    return NULL;
}

void printSymbol(char* name){
    Symbol* sym = symbolTable;
    while (sym) {
        if (strcmp(sym->data->name, name) == 0) {
            printf("%s: %s\n", sym->data->name, sym->data->value);
            break;
        }
        sym = sym->next;
    }
}

AttrNode* createAttrList(char* attrName) {
    AttrNode* head = (AttrNode*)malloc(sizeof(AttrNode));
    head->name = strdup(attrName);
    head->next = NULL;
    return head;
}

void addToAttrList(AttrNode** head, char* attrName) {
    AttrNode* newNode = (AttrNode*)malloc(sizeof(AttrNode));
    newNode->name = strdup(attrName);
    newNode->next = NULL;

    if (*head == NULL) {
        *head = newNode;
        return;
    }

    AttrNode* temp = *head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newNode;
}

void pushAttrListToClass(char* className, AttrNode* head) {
    AttrNode* temp = head;
    
    while (temp != NULL) {
        addAttribute(className, temp->name);
        temp = temp->next;
    }

}

void freeAttrList(AttrNode* head) {
    AttrNode* temp;
    while (head) {
        temp = head;
        head = head->next;
        free(temp->name);
        free(temp);
    }
}



void printSymbolTable() {
    Symbol* sym = symbolTable;

    printf("\n================== SYMBOL TABLE ==================\n");
    printf("| %-15s | %-10s | %-20s |\n", "Name", "Type", "Value");
    printf("--------------------------------------------------\n");

    while (sym) {
        printf("| %-15s | ", sym->data->name);  

        switch (sym->data->type) {
            case INT_TYPE:
                printf("%-10s | = %d\n","INT", *(int*)sym->data->value);
                break;
            case FLOAT_TYPE:
                printf("%-10s | = %.2f\n", "FLOAT", *(float*)sym->data->value);
                break;
            case CHAR_TYPE:
                printf("%-10s | = '%c'\n", "CHAR", *(char*)sym->data->value);
                break;
            case STRING_TYPE:
                printf("%-10s | = \"%s\"\n", "STRING", (char*)sym->data->value);
                break;

            case CLASS_TYPE: {
                printf("%-10s | %s\n", "CLASS", sym->data->name);
                Data** attrList = (Data**)sym->data->value;
                
                printf("  |- Attributes:\n");
                if (attrList != NULL && sym->data->size > 0) {
                    for (int i = 0; i < sym->data->size; i++) {
                        printf("  |    - %s\n", attrList[i]->name);
                    }
                } else {
                    printf("  |    (No attributes defined)\n");
                }
                break;
            }

            case OBJECT_TYPE: {
                printf("%-10s | %s\n", "OBJECT", sym->data->name);
                printf("  |- Attributes:\n");
            
                Symbol* attrSym = sym->data->classSym;
                
                if (attrSym != NULL) {
                    while (attrSym) {
                        printf("  |    - %s = ", attrSym->data->name);
                        if (attrSym->data->value) {
                            switch (attrSym->data->type) {
                                case INT_TYPE:
                                    printf("%d", *(int*)attrSym->data->value);
                                    break;
                                case FLOAT_TYPE:
                                    printf("%.2f", *(float*)attrSym->data->value);
                                    break;
                                case CHAR_TYPE:
                                    printf("'%c'", *(char*)attrSym->data->value);
                                    break;
                                case STRING_TYPE:
                                    printf("\"%s\"", (char*)attrSym->data->value);
                                    break;
                                default:
                                    printf("Unknown Value");
                                    break;
                            }
                        } else {
                            printf("NULL");
                        }
                        printf("\n");
                        attrSym = attrSym->next;
                    }
                } else {
                    printf("  |    (No attributes assigned)\n");
                }
                break;
            }
            

            default:
                printf("%-10s | ???\n", "UNKNOWN");
                break;
        }

        sym = sym->next;
    }

    printf("==================================================\n");
}




void createAttributes(char* className, char* attrName) {
    Symbol* temp = NULL;
    Symbol* sym = symbolTable;

    while (sym) {
        if (strcmp(sym->data->name, className) == 0) {
            temp = sym;
            break;
        }
        sym = sym->next;
    }

    if (!temp) {
        printf("Error: Class %s not found!\n", className);
        return;
    }

    if (temp->data->value == NULL) {
        temp->data->value = malloc(sizeof(Data*) * 1);
        temp->data->size = 0;
    } else {
        temp->data->value = realloc(temp->data->value, sizeof(Data*) * (temp->data->size + 1));
    }

    Data* attrData = (Data*)malloc(sizeof(Data));
    attrData->name = strdup(attrName);
    attrData->type = -1;
    attrData->value = NULL;
    attrData->classSym = NULL;

    ((Data**)temp->data->value)[temp->data->size] = attrData;
    temp->data->size++;
}

void addAttribute(char* className, char* attrName) {
    Symbol* temp = NULL;
    Symbol* sym = symbolTable;

    while (sym) {
        if (strcmp(sym->data->name, className) == 0) {
            temp = sym;
            break;
        }
        sym = sym->next;
    }

    if (!temp) {
        printf("Error: Class %s not found!\n", className);
        return;
    }

    if (temp->data->value == NULL) {
        temp->data->value = malloc(sizeof(Data*) * 1);
        temp->data->size = 0;
    } else {
        temp->data->value = realloc(temp->data->value, sizeof(Data*) * (temp->data->size + 1));
    }

    Data* attrData = (Data*)malloc(sizeof(Data));
    attrData->name = strdup(attrName);
    attrData->type = -1;
    attrData->value = NULL;

    ((Data**)temp->data->value)[temp->data->size] = attrData;
    temp->data->size++;
}

Data* createClass(char* name) {
    Data* dta = (Data*)malloc(sizeof(Data));
    dta->type = CLASS_TYPE;
    dta->value = malloc(sizeof(Data*) * 1);  
    dta->name = strdup(name);
    dta->size = 0;

    insertSymbol(name, dta);
    return dta;
}



void createObject(char* className, char* objectName) {
    Symbol* sym = symbolTable;
    Symbol* temp = NULL;

    while (sym) {
        if (strcmp(sym->data->name, className) == 0) {
            temp = sym;
            break;
        }
        sym = sym->next;
    }

    if (temp == NULL) {
        printf("Error: Class '%s' not found.\n", className);
        return;
    }

    Data** attrList = (Data**)temp->data->value;

    Data* obj = (Data*)malloc(sizeof(Data));
    obj->name = strdup(objectName);
    obj->type = OBJECT_TYPE;
    obj->classSym = NULL;

    for (int i = 0; i < temp->data->size; i++) {
        if (attrList[i] == NULL) continue;  
        Symbol* attrSym = (Symbol*)malloc(sizeof(Symbol));
        attrSym->data = (Data*)malloc(sizeof(Data));

        attrSym->data->name = strdup(attrList[i]->name);
        attrSym->data->type = attrList[i]->type;
        attrSym->data->value = NULL;
        attrSym->next = obj->classSym;
        obj->classSym = attrSym;
    }

    insertSymbol(objectName, obj);
}



void setAttributeValue(char* objectName, char* attrName, Data* d) {
    Symbol* sym = symbolTable;
    
    while (sym) {
        if (strcmp(sym->data->name, objectName) == 0 && sym->data->type == OBJECT_TYPE) {
            break;
        }
        sym = sym->next;
    }

    if (!sym) {
        printf("Error: Object '%s' not found.\n", objectName);
        return;
    }

    Symbol* attrSym = sym->data->classSym;  


    while (attrSym) {
        char* temp;
        if (strcmp(attrSym->data->name, attrName) == 0) {
            temp = attrSym->data->name;
            attrSym->data = d;
            attrSym->data->name = temp;
            return;
        }
        attrSym = attrSym->next;
    }

    printf("Error: Attribute '%s' not found in object '%s'.\n", attrName, objectName);
}


// int main() {
//     int val = 10;
//     float fval = 20.5;
//     char cval = 'i';
//     char* sval = "Hello";

//     Data* intData = createData(&val, INT_TYPE);
//     insertSymbol("integer", intData);

//     Data* floatData = createData(&fval, FLOAT_TYPE);
//     insertSymbol("float_var", floatData);

//     Data* charData = createData(&cval,CHAR_TYPE);
//     insertSymbol("char_var", charData);

//     Data* stringData = createData(sval,STRING_TYPE);
//     insertSymbol("string_var", stringData);


//     Data* class = createClass("MYCLASS");
//     createAttributes("MYCLASS","a");
//     addAttribute("MYCLASS","b");

//     createObject("MYCLASS","op");

//     Data* class1 = createClass("MYCLASS1");
//     createAttributes("MYCLASS1","a1");
//     addAttribute("MYCLASS1","b1");

//     createObject("MYCLASS1","op1");


//     Data* newdata = createData(&cval,CHAR_TYPE);
//     setAttributeValue("op","a",newdata);



//     printSymbolTable();
//     return 0;
// } 
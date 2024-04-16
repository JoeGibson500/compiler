#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "lexer.h"
#include "parser.h"
#include <stdbool.h>

struct VariableSymbol {

    char variableSymbolName[128];
    struct VariableSymbol* next; // point to next variable symbol

} VariableSymbol;


struct MethodSymbol {

    char methodSymbolName[128];

    struct VariableSymbol* variableSymbols; // list of variable symbols
    struct MethodSymbol* next; // point to next method symbol

} MethodSymbol;


struct ClassSymbol {

    char classSymbolName[128];
    struct MethodSymbol* methodSymbols; // list of Method Symbols 
    struct ClassSymbol* next; // point to next class symbol

} ClassSymbol;


struct Program {

    struct ClassSymbol* classSymbols; // list of Class symbols

} Program;



struct LibraryMethod {
    char methodName[128];
};

struct LibraryClass {
    char className[128];
    struct LibraryMethod methods[20];
    int methodCount;
};




void InitProgramTable();
void InitClassTable(const char* className);
void InitMethodTable(const char* className, const char* methodName);

void InsertClassSymbol(const char* className);
void InsertMethodSymbol(const char* className, const char* methodName);
void InsertVariableSymbol(const char* className, const char* methodName, const char* variableName);

bool FindMethodSymbol(const char *className, const char* methodName);
bool FindVariableSymbol(const char *className, const char* methodName, const char* variableName);
bool FindClassSymbol(const char* className);


bool IsJackLibraryClass(const char* className);
bool IsJackLibraryMethod(const char* className, const char* methodName);

void PrintSymbols();

#endif

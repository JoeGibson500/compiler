#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "lexer.h"
#include "parser.h"

struct VariableSymbol {

    char variableName[128];
    struct VariableSymbol* next; // point to next variable symbol

} VariableSymbol;


struct MethodSymbol {

    char methodSymbolName[128];

    struct VariableSymbol* variableSymbols; // list of variable symbols
    struct MethodSymbol* next; // point to next method symbol

} MethodSymbol;


struct ClassSymbol {

    char className[128];
    struct MethodSymbol* methodSymbols; // list of Method Symbols 
    struct ClassSymbol* next; // point to next class symbol

} ClassSymbol;


struct Program {

    struct ClassSymbol* classSymbols; // list of Class symbols

} Program;


void InitProgramTable();
void InitClassTable(const char* className);
void InitMethodTable(const char* className, const char* methodName);

void InsertClassSymbol(const char* className);
void InsertMethodSymbol(const char* className, const char* methodName);
void InsertVariableSymbol(const char* className, const char* methodName, const char* variableName);

#endif

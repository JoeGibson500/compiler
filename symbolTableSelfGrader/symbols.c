/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
The Symbol Tables Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name: Joseph Gibson
Student ID: 201619051
Email: sc22jg@leeds.ac.uk
Date Work Commenced: 12/03/2004
*************************************************************************/

#include "symbols.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct Program programTable;

void InitProgramTable() {
    programTable.classSymbols = NULL;
}

void InsertClassSymbol(const char* className) {

    // Create a new class
    struct ClassSymbol* newClass = (struct ClassSymbol*)malloc(sizeof(struct ClassSymbol));

    if(newClass == NULL) {
        printf("Memory allocation error");
    }

    //Copy class name to new node
    strcpy(newClass->classSymbolName, className);

    //Initialise the method symbols list to null 
    newClass->methodSymbols = NULL;

    //Set the next pointer to the current list of classes
    newClass->next = programTable.classSymbols;

    //Update the program symbol table to point to the new class as the head of the list
    programTable.classSymbols = newClass;

}

void InsertMethodSymbol(const char* className, const char* methodName, const char* methodReturnType) {

    // Create a new method
    struct MethodSymbol* newMethod = (struct MethodSymbol*)malloc(sizeof(struct MethodSymbol));

    if(newMethod == NULL) {
        printf("Memory allocation error");
    }

    // Copy method name to new node
    strcpy(newMethod->methodSymbolName, methodName);
    strcpy(newMethod->methodReturnType, methodReturnType);
    // Initialize the variable symbols list to null
    newMethod->variableSymbols = NULL;

    // Find the appropriate class
    struct ClassSymbol* currentClass;

    currentClass = programTable.classSymbols;
    while(strcmp(currentClass->classSymbolName, className) && currentClass != NULL) {
        currentClass = currentClass->next;
    }

    if(currentClass == NULL) {
        printf("Class not found %s", className);
        free(newMethod);
    }

    // Insert new method at the beginning of the method symbols list
    newMethod->next = currentClass->methodSymbols;
    currentClass->methodSymbols = newMethod;

}

void InsertVariableSymbol(const char* className, const char* methodName, const char* variableName, const char* currentType) {

    // Create new variable symbol
    struct VariableSymbol* newVariable = (struct VariableSymbol*)malloc(sizeof(struct VariableSymbol));

    if (newVariable == NULL) {
        printf("Failed to allocate memory for new variable\n");
        return;
    }

    // Copy variable name to new node
    strcpy(newVariable->variableSymbolName, variableName);
    strcpy(newVariable->variableTypeName, currentType);

    // Find the appropriate class
    struct ClassSymbol* currentClass;

    currentClass = programTable.classSymbols;
    while(strcmp(currentClass->classSymbolName, className) && currentClass != NULL) {
        currentClass = currentClass->next;
    }
    
    if(currentClass == NULL) {
        printf("Class not found %s", className);
        free(newVariable);
    }    

    // Find appropriate method within the current class

    struct MethodSymbol* currentMethod;
    currentMethod = currentClass->methodSymbols;
    while(strcmp(currentMethod->methodSymbolName, methodName) && currentMethod != NULL) {
        currentMethod = currentMethod->next;
    }

    if(currentMethod == NULL) {
        free(newVariable);
    }    

    // Insert new variable into variable symbols list for the method
    newVariable->next = currentMethod->variableSymbols;
    currentMethod->variableSymbols = newVariable;
}

bool FindClassSymbol(const char* className) { 

    struct ClassSymbol* currentClass = NULL;
    
    currentClass = programTable.classSymbols;
    while(currentClass != NULL) {
        if (!strcmp(currentClass->classSymbolName, className)) {
            return true;
        }
        currentClass = currentClass->next;
    }
    return false;
}

bool FindMethodSymbol(const char* className, const char* methodName) {

    // Initialize currentClass to NULL
    struct ClassSymbol* currentClass = NULL;

    // Find the appropriate class
    currentClass = programTable.classSymbols;  // Start from the beginning of the class symbols list
    while(currentClass != NULL && strcmp(currentClass->classSymbolName, className) != 0) {
        currentClass = currentClass->next;
    }

    if(currentClass == NULL) {
        // printf("Class not found %s", className);
        return false;
    }


    struct MethodSymbol* currentMethod = NULL;
    currentMethod = currentClass->methodSymbols;
    while (currentMethod != NULL) {
        if(!strcmp(currentMethod->methodSymbolName, methodName)) {
            return true;
        }
        currentMethod = currentMethod->next;
    }
    
    return false;
}

bool FindVariableSymbol(const char* className, const char* methodName, const char* variableName) {

    // Initialize currentClass to NULL
    struct ClassSymbol* currentClass = NULL;

    // Find the appropriate class
    currentClass = programTable.classSymbols;  // Start from the beginning of the class symbols list
    while(currentClass != NULL && strcmp(currentClass->classSymbolName, className) != 0) {
        currentClass = currentClass->next;
    }

    if(currentClass == NULL) {
        return false;
    }

    struct MethodSymbol* currentMethod = NULL;
    currentMethod = currentClass->methodSymbols;
    while (currentMethod != NULL && strcmp(currentMethod->methodSymbolName, methodName)) {
        currentMethod = currentMethod->next;
    }

    if(currentMethod == NULL) {
        return false;
    }

    struct VariableSymbol* currentVariable = NULL;
    currentVariable = currentMethod->variableSymbols;

    while (currentVariable != NULL) {
        if (!strcmp(currentVariable->variableSymbolName, variableName)) {
            return true;
        }
        currentVariable = currentVariable->next;
    }
    return false;
}

char* FindVariableType(const char* className, const char* methodName, const char* variableName) {

    // Initialize currentClass to NULL
    struct ClassSymbol* currentClass = NULL;

    // Find the appropriate class
    currentClass = programTable.classSymbols;  // Start from the beginning of the class symbols list
    while(currentClass != NULL && strcmp(currentClass->classSymbolName, className) != 0) {
        currentClass = currentClass->next;
    }

    if(currentClass == NULL) {
        return "notype";
    }

    struct MethodSymbol* currentMethod = NULL;
    currentMethod = currentClass->methodSymbols;
    while (currentMethod != NULL && strcmp(currentMethod->methodSymbolName, methodName)) {
        currentMethod = currentMethod->next;
    }

    if(currentMethod == NULL) {
        return "notype";
    }

    struct VariableSymbol* currentVariable = NULL;
    currentVariable = currentMethod->variableSymbols;

    while (currentVariable != NULL) {
        if (!strcmp(currentVariable->variableSymbolName, variableName)) {
            return currentVariable->variableTypeName;
        }
        currentVariable = currentVariable->next;
    }

    return "notype";
}

char* FindMethodReturnType(const char* className, const char* methodName) {

    // Initialize currentClass to NULL
    struct ClassSymbol* currentClass = NULL;

    // Find the appropriate class
    currentClass = programTable.classSymbols;  // Start from the beginning of the class symbols list
    while(currentClass != NULL && strcmp(currentClass->classSymbolName, className) != 0) {
        currentClass = currentClass->next;
    }

    if(currentClass == NULL) {
        return "notype";
    }

    struct MethodSymbol* currentMethod = NULL;
    currentMethod = currentClass->methodSymbols;
    while (currentMethod != NULL) {

        if(!strcmp(currentMethod->methodSymbolName, methodName)) {
            return currentMethod->methodReturnType;
        }

        currentMethod = currentMethod->next;
    }

    return "notype";

}

void PrintSymbols() {
    
    struct ClassSymbol* currentClass = programTable.classSymbols;
    while(currentClass != NULL) {
        printf("Class: %s\n", currentClass->classSymbolName);

        // Print methods for this class
        struct MethodSymbol* currentMethod = currentClass->methodSymbols;
        while(currentMethod != NULL) {
            printf("\tMethod: %s\n", currentMethod->methodSymbolName);

            // Print variables for this method
            struct VariableSymbol* currentVariable = currentMethod->variableSymbols;
            while(currentVariable != NULL) {
                printf("\t\tVariable: %s\n", currentVariable->variableSymbolName);
                currentVariable = currentVariable->next;
            }

            currentMethod = currentMethod->next;
        }

        currentClass = currentClass->next;
    }
}

// Static array to hold the library classes and their methods
static struct LibraryClass libraryClasses[8] = {
    {"Math", {{"abs"}, {"sqrt"}, {"min"}, {"max"}, {"divide"}, {"multiply"}}, 6},
    {"String", {{"new"}, {"dispose"}, {"length"}, {"charAt"}, {"setCharAt"}, {"appendChar"}, {"eraseLastChar"}, {"intValue"}, {"setInt"}, {"newLine"}, {"backSpace"}, {"doubleQuote"}}, 12},
    {"Array", {{"dispose"}, {"new"}}, 2},
    {"Output", {{"printChar"}, {"printInt"}, {"println"}, {"moveCursor"}, {"printString"}, {"backSpace"}, {"init"}}, 7},
    {"Screen", {{"drawPixel"}, {"drawLine"}, {"drawRectangle"}, {"drawCircle"}, {"setColor"}, {"clearScreen"}}, 6},
    {"Keyboard", {{"keyPressed"}, {"readChar"}, {"readLine"}, {"readInt"}}, 4},
    {"Memory", {{"peek"}, {"poke"}, {"alloc"}, {"deAlloc"}}, 4},
    {"Sys", {{"halt"}, {"error"}, {"wait"}}, 3}
};

bool IsJackLibraryClass(const char* className) {
    for (int i = 0; i < 8; i++) {
        if (strcmp(libraryClasses[i].className, className) == 0) {
            return true;
        }

    }
    return false;
}

bool IsJackLibraryMethod(const char* className, const char* methodName) {
    for (int i = 0; i < 8; i++) {
        if (strcmp(libraryClasses[i].className, className) == 0) {
            for (int j = 0; j < libraryClasses[i].methodCount; j++) {
                if (strcmp(libraryClasses[i].methods[j].methodName, methodName) == 0) {
                    return true;
                }
            }
        }
    }
    return false;
}







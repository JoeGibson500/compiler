
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

void InitProgramTable();
void InsertClassSymbol(const char* className);
void InsertMethodSymbol(const char* className, const char* methodName);
void InsertVariableSymbol(const char* className, const char* methodName, const char* variableName);
void PrintClassSymbols();


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


void InsertMethodSymbol(const char* className, const char* methodName) {

    // Create a new method
    struct MethodSymbol* newMethod = (struct MethodSymbol*)malloc(sizeof(struct MethodSymbol));

    if(newMethod == NULL) {
        printf("Memory allocation error");
    }

    // Copy method name to new node
    strcpy(newMethod->methodSymbolName, methodName);

    // Initialize the variable symbols list to null
    newMethod->variableSymbols = NULL;


    // Find the appropriate class
    struct ClassSymbol* currentClass = (struct ClassSymbol*)malloc(sizeof(struct ClassSymbol));
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

void InsertVariableSymbol(const char* className, const char* methodName, const char* variableName) {

    // Create new variable symbol]
    struct VariableSymbol* newVariable = (struct VariableSymbol*)malloc(sizeof(struct VariableSymbol));

    // Copy variable name to new node
    strcpy(newVariable->variableSymbolName, variableName);

    // Find the appropriate class
    struct ClassSymbol* currentClass = (struct ClassSymbol*)malloc(sizeof(struct ClassSymbol));
    while(strcmp(currentClass->classSymbolName, className) && currentClass != NULL) {
        currentClass = currentClass->next;
    }
    
    if(currentClass == NULL) {
        printf("Class not found %s", className);
        free(newVariable);
    }    

    // Find appropriate method within the current class
    struct MethodSymbol* currentMethod = (struct MethodSymbol*)malloc(sizeof(struct MethodSymbol));
    while(strcmp(currentMethod->methodSymbolName, methodName) && currentMethod != NULL) {
        currentMethod = currentMethod->next;
    }

    // Insert new variable into variable symbols list for the method
    newVariable->next = currentMethod->variableSymbols;
    currentMethod->variableSymbols = newVariable;
}

void PrintClassSymbols() {
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

void PrintMethodSymbols(const char* className) {
    struct ClassSymbol* currentClass = programTable.classSymbols;
    while(currentClass != NULL && strcmp(currentClass->className, className) != 0) {
        currentClass = currentClass->next;
    }

    if(currentClass == NULL) {
        printf("Class not found: %s\n", className);
        return;
    }

    printf("Class: %s\n", currentClass->className);

    // Print methods for this class
    struct MethodSymbol* currentMethod = currentClass->methodSymbols;
    while(currentMethod != NULL) {
        printf("\tMethod: %s\n", currentMethod->methodSymbolName);

        // Print variables for this method
        struct VariableSymbol* currentVariable = currentMethod->variableSymbols;
        while(currentVariable != NULL) {
            printf("\t\tVariable: %s\n", currentVariable->variableName);
            currentVariable = currentVariable->next;
        }

        currentMethod = currentMethod->next;
    }
}


void PrintVariableSymbols(const char* className, const char* methodName) {
    struct ClassSymbol* currentClass = programTable.classSymbols;
    while(currentClass != NULL && strcmp(currentClass->className, className) != 0) {
        currentClass = currentClass->next;
    }

    if(currentClass == NULL) {
        printf("Class not found: %s\n", className);
        return;
    }

    printf("Class: %s\n", currentClass->classSymbolName);

    // Find the method in the class
    struct MethodSymbol* currentMethod = currentClass->methodSymbols;
    while(currentMethod != NULL && strcmp(currentMethod->methodSymbolName, methodName) != 0) {
        currentMethod = currentMethod->next;
    }

    if(currentMethod == NULL) {
        printf("Method not found: %s\n", methodName);
        return;
    }

    printf("\tMethod: %s\n", currentMethod->methodSymbolName);

    // Print variables for this method
    struct VariableSymbol* currentVariable = currentMethod->variableSymbols;
    while(currentVariable != NULL) {
        printf("\t\tVariable: %s\n", currentVariable->variableName);
        currentVariable = currentVariable->next;
    }
}
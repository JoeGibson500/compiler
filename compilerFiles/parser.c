#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#include "lexer.h"
#include "parser.h"
#include "symbols.h"
#include "compiler.h"

// you can declare prototypes of parser functions below

ParserInfo classDeclar();
ParserInfo memberDeclar();
ParserInfo classVarDeclar();
ParserInfo type();
ParserInfo subroutineDeclar();
ParserInfo paramList(); 
ParserInfo subroutineBody();
ParserInfo statement();
ParserInfo varDeclarStatement();
ParserInfo letStatement();
ParserInfo ifStatement();
ParserInfo whileStatement();
ParserInfo doStatement();
ParserInfo subroutineCall();
ParserInfo expressionList();
ParserInfo returnStatement();
ParserInfo expression();
ParserInfo relationalExpression();
ParserInfo arithmeticExpression();
ParserInfo term();
ParserInfo factor();
ParserInfo operand();

bool isFactorOrOperand();
bool isIntegerConstant(const char* string);
bool isStatement();
bool isType();
bool isLexerError();

ParserInfo pi;
Token t;

char currentClassName[128];
char currentMethodName[128];
char currentVariableName[128];
char classInstanceName[128];
char classInstanceMethodName[128];
char currentJackLibraryClass[128];
char currentType[128];

// int phaseNumber = 1;

// an expression starts with either -|~|integerConstant|identifier| (expression)| stringLiteral | true | false | null | this
bool isFactorOrOperand() {

	if(!strcmp(t.lx, "-") || !strcmp(t.lx, "~")) {
		return true;
	} else if (isIntegerConstant(t.lx)) {
		return true;
	} else if (t.tp == ID) {
		return true;
	} else if (t.tp == STRING) {
		return true;
	} else if (!strcmp(t.lx, "true") || !strcmp(t.lx, "false") || !strcmp(t.lx, "null") || !strcmp(t.lx, "this")) {
		return true;
	} else if (t.tp == SYMBOL && !strcmp(t.lx, "(")) {
		//printf("bracket peeked\n");
		return true;
	}
	return false;
}

bool isIntegerConstant(const char* string) {

    if (string == NULL || *string == '\0') {
        return false;
    }

    // Check if all characters are digits
    for (int i = 0; string[i] != '\0'; ++i) {
        if (!isdigit((unsigned char)string[i])) {
            return false;
        }
    }

    return true;
}

bool isStatement() {

	if (!strcmp(t.lx, "var") || !strcmp(t.lx, "let") || !strcmp(t.lx, "if") || !strcmp(t.lx, "while") || !strcmp(t.lx, "do") || !strcmp(t.lx, "return")){
		return true;
	} else {
		return false;
	}
}

bool isType() {
	
	if (t.tp == ID) {
		return true;
	} else if (!strcmp(t.lx, "int") || !strcmp(t.lx, "char") || !strcmp(t.lx, "boolean")) {
		return true;
	} else {
		return false;
	}
}

bool isLexerError() {

	if(t.tp == ERR) {
		pi.er = lexerErr;
		pi.tk = t;
		return true;
	} else {
		return false;
	}
}

// classDeclar -> class identifier { {memberDeclar} }
ParserInfo classDeclar() {

	// Eat Starting word "class"
	t = GetNextToken();	
	if (isLexerError()) return pi;

	// printf("FIRST TOKEN = %s", t.lx);

	if (t.tp == RESWORD && !strcmp(t.lx, "class")) {
		//printf("Class found\n");
	} else {
		//printf("Class not found\n");
		pi.er = classExpected;
		pi.tk = t;
	}

	if (pi.er != none) return pi; 

	// Eat name of class
	t = GetNextToken();
	if (isLexerError()) return pi;
	
	if (t.tp == ID) {
		//printf("Identifier found\n");
		strcpy(currentClassName, t.lx);
		if(phaseNumber == 1) {
			if (FindClassSymbol(currentClassName)) {
				pi.er = redecIdentifier;
				pi.tk = t;
			} else {
				InsertClassSymbol(currentClassName);
			}
		}
	} else {
		//printf("Identifier not found\n");
		pi.er = idExpected;
		pi.tk = t;
	}

	if (pi.er != none) return pi;

	// Eat opening brace 
	t = GetNextToken();
	if (isLexerError()) return pi;

	if (t.tp == SYMBOL && !strcmp(t.lx, "{")) {
		//printf("Curly brace found\n");
	} else {
		//printf("Curly brace not found\n");
		pi.er = openBraceExpected;
		pi.tk = t;
	}

	if (pi.er != none) return pi; 	

	// Eat Member decaration(s)  
	t = PeekNextToken();

	while (t.tp != SYMBOL || strcmp(t.lx, "}")) {
        
		pi = memberDeclar();

		if (pi.er != none) return pi; 

        t = PeekNextToken();
	}	

	// Eat Closing brace
	t = GetNextToken();
	if (isLexerError()) return pi;
	if (t.tp == SYMBOL && !strcmp(t.lx, "}")) {
		//printf("Closing brace found\n");
	} else {
		//printf("Closing brace not found\n");
		pi.er = closeBraceExpected;
		pi.tk = t;
	}

	return pi;
}

// memberDeclar -> classVarDeclar | subRoutineDeclar
ParserInfo memberDeclar() {
	
	// Check if class variable or subroutine declaration
	t = PeekNextToken();

	if (t.tp == RESWORD && (!strcmp(t.lx, "static") || !strcmp(t.lx, "field"))){
		//printf("classVar peeked\n");
		// strcpy(currentMethodName, t.lx);
		// InsertMethodSymbol(currentClassName, currentMethodName);

		pi = classVarDeclar();

	} else if ((t.tp == RESWORD) && (!strcmp(t.lx, "function") || !strcmp(t.lx, "constructor") || !strcmp(t.lx, "method"))) {
		//printf("subroutineDeclar peeked\n");
		// strcpy(currentMethodName, t.lx);
		// InsertMethodSymbol(currentClassName, currentMethodName);
		pi = subroutineDeclar();
		// InsertMethodSymbol(currentClassName, t.lx);
	} else {
		//printf("Member declaration not found\n");
		pi.er = memberDeclarErr;
		pi.tk = t;
	}
	return pi;
}

// classVarDeclar -> (static | field) type identifier {, identifier} ;
ParserInfo classVarDeclar() {
	
	// Eat Static or field resererved word 
	t = GetNextToken();
	if (isLexerError()) return pi;
	if (t.tp == RESWORD && (!strcmp(t.lx, "static") || !strcmp(t.lx, "field"))){
		//printf("Variable declaration keyword found\n");
	} else {
		//printf("Variable declaration keyword not found\n");
		pi.er = classVarErr;
		pi.tk = t;
	}	

	if (pi.er != none) return pi; 

	// Check if type
	t = PeekNextToken();
	if (isType()) {
		pi = type();
	} else {
		pi.er = illegalType;
		pi.tk = t;
	}

	if (pi.er != none) return pi; 

	// Eat first identifier
	t = GetNextToken();
	if (isLexerError()) return pi;

	

	if(t.tp == ID) {
		//printf("Identifier found = %s\n", t.lx);
		strcpy(currentVariableName, t.lx);
		if (phaseNumber == 1) {
			if (FindMethodSymbol(currentClassName, currentVariableName)) {
				pi.er = redecIdentifier;
				pi.tk = t;
			} else {
				InsertMethodSymbol(currentClassName, currentVariableName);
			}
		}
	} else {
		//printf("Identifier not found\n");
		pi.er = idExpected;
		pi.tk = t;
	}

	if (pi.er != none) return pi; 

	// Check if more identifiers, if so eat them
	t = PeekNextToken();
	while (t.tp == SYMBOL && !strcmp(t.lx, ",")) {

		t = GetNextToken();
		if (isLexerError()) return pi;
		t = GetNextToken();
		if (isLexerError()) return pi;

		if (t.tp == ID) {
			//printf("Identifier found\n");
			strcpy(currentMethodName, t.lx);

			if (phaseNumber == 1) {
				if (FindMethodSymbol(currentClassName, currentMethodName)) {
					pi.er = redecIdentifier;
					pi.tk = t;
				} else {
					InsertMethodSymbol(currentClassName, currentMethodName);
				}
			}

		} else {
			//printf("Identifier not found");
			pi.er = idExpected;
			pi.tk = t;
		}
		t = PeekNextToken();
	}

	if (pi.er != none) return pi; 

	// Eat Semi-colon
	t = GetNextToken();
	if (isLexerError()) return pi;

	if(t.tp == SYMBOL && !strcmp(t.lx, ";")) {
		//printf("Semi-colon found\n");
	} else {
		pi.er = semicolonExpected;
		pi.tk = t;
	}

	return pi;
}

// type -> int | char | boolean | identifier
ParserInfo type() {

	// Eat type reserved word
	t = GetNextToken();
	if (isLexerError()) return pi;
	if (!strcmp(t.lx, "int") || !strcmp(t.lx, "char") || !strcmp(t.lx, "boolean")) {
		//printf("Variable type found\n");
	} else if (t.tp == ID) {
		
		strcpy(currentVariableName, t.lx);
		if (phaseNumber == 2) {

			if (FindClassSymbol(currentClassName) || FindMethodSymbol(currentClassName, currentVariableName) || FindVariableSymbol(currentClassName, currentMethodName, currentVariableName)) {
				// Do nothing
			} else {
				printf("Undeclared identifier");
				pi.er = undecIdentifier;
				pi.tk = t;
			}
		}
	}
	 else {
		//printf("Illegal type\n!");
		pi.er = illegalType;
		pi.tk = t;
	}

	return pi;
}

// subroutine -> (constructor | function | method) (type|void) identifier (paramList) subroutineBody
ParserInfo subroutineDeclar() {

	// Eat constructor or function or method resererved word 
	t = GetNextToken();
	if (isLexerError()) return pi;
	if (t.tp == RESWORD && (!strcmp(t.lx, "constructor") || !strcmp(t.lx, "function") || !strcmp(t.lx, "method"))){
		//printf("Subroutine declaration keyword found\n");

	} else {
		//printf("Subroutine declaration keyword not found\n");
		pi.er = subroutineDeclarErr;
		pi.tk = t;
	}	

	if (pi.er != none) return pi; 

	// Eat "type" or "void"
	t = PeekNextToken();
	if(!strcmp(t.lx, "void")) {
		t = GetNextToken();
		if (isLexerError()) return pi;
		//printf("Return type found = void\n");
	} else if (isType()) {
		pi = type();
		//printf("Return type found = %s\n", t.lx);
	} else {
		//printf("Return type not found = %s\n", t.lx);
		pi.er = illegalType;
		pi.tk = t;
	}
	
	if (pi.er != none) return pi; 

	// Eat identifier
	t = GetNextToken();
	if (isLexerError()) return pi;

	if (t.tp == ID) {
		//printf("Identifier found\n");

		strcpy(currentMethodName, t.lx);

		if (phaseNumber == 1) {

			if (FindMethodSymbol(currentClassName, currentMethodName)) {
				pi.er = redecIdentifier;
				pi.tk = t;
			} else {
				InsertMethodSymbol(currentClassName, currentMethodName);
			}

		}

	} else {
		//printf("Identifier not found\n");
		pi.er = idExpected;
		pi.tk = t;
	}

	if (pi.er != none) return pi; 

	// Eat opening parenthesis
	t = GetNextToken();
	if (isLexerError()) return pi;

	if (t.tp == SYMBOL && !strcmp(t.lx, "(")) {
		//printf("opening bracket found\n");
	} else {
		//printf("opening bracket not found\n");
		pi.er = openParenExpected;
		pi.tk = t;
	}

	if (pi.er != none) return pi; 

	// Check for parameter list
	t = PeekNextToken();
	if (isType()) {
		//printf("Parameter peeked\n");
		pi = paramList();
	}

	if (pi.er != none) return pi; 

	// Eat closing paranthesis
	t = GetNextToken();
	if (isLexerError()) return pi;

	if (t.tp == SYMBOL && !strcmp(t.lx, ")")) {
		//printf("closing bracket found\n");
	} else {
		//printf("closing bracket not found\n");
		pi.er = closeParenExpected;
		pi.tk = t;
	}

	if (pi.er != none) return pi; 

	// Check for subroutine body 
	t = PeekNextToken(); 
	if (t.tp == SYMBOL && !strcmp(t.lx, "{")) {
		//printf("Peeked subroutineBody\n");
		pi = subroutineBody();
	} else {
		pi.er = openBraceExpected;
		pi.tk = t;
	}

	return pi;
}

// paramList -> type identifier {, type identifier} | ε
ParserInfo paramList() { 

	// Check for type
	t = PeekNextToken();

	if (isType()) {
		pi = type();
	} else {
		//printf("Type expected\n");
		pi.er = illegalType;
		pi.tk = t;
	}

	if (pi.er != none) return pi; 

	// Eat identifier 
	t = GetNextToken();
	if (isLexerError()) return pi;

	if (t.tp == ID) {
		//printf("Identifier found\n");
	} else {
		//printf("Identifier not found\n");
		pi.er = idExpected;
		pi.tk = t;
	}

	if (pi.er != none) return pi; 

	// Check for additional identifer list
	t = PeekNextToken();

	while (t.tp == SYMBOL && !strcmp(t.lx, ",")) {

		// Eat comma 
		t = GetNextToken();
		if (isLexerError()) return pi;

		// Check for type 
		t = PeekNextToken();

		if (isType()) {
			pi = type();
		} else {
			//printf("Illegal type");
			pi.er = illegalType;
			pi.tk = t;
		}

		if (pi.er != none) return pi; 

		// Eat identifier
		t = GetNextToken();
		if (isLexerError()) return pi;

		if (t.tp == ID) {
			//printf("Identifier found\n");
		} else {
			//printf("Identifier not found\n");
			pi.er = idExpected;
			pi.tk = t;
		}

		if (pi.er != none) return pi; 

		// Check again
		t = PeekNextToken();
	}

	return pi;
}

//subroutineBody -> { {statement} }
ParserInfo subroutineBody() { 

	// Eat opening curly brace 
	t = GetNextToken();
	if (isLexerError()) return pi;

	if (t.tp == SYMBOL && !strcmp(t.lx, "{")) {
		//printf("Curly brace found\n");
	} else {
		//printf("Curly brace not found\n");
		pi.er = openBraceExpected;
		pi.tk = t;
	}

	if (pi.er != none) return pi; 

	// Check for statement
	t = PeekNextToken();

	if (t.tp == RESWORD) {
		//printf("Statement peeked\n");
		pi = statement();
	} else {
		//printf("Statement expected, idk error code\n");
	}

	if (pi.er != none) return pi; 

	// Check for extra statements
	t = PeekNextToken();

	while(isStatement()) {
		
		//printf("Statement peeked\n");
		pi = statement();

		if (pi.er != none) return pi; 

		t = PeekNextToken();
	}

	// Eat closing curly brace
	t = GetNextToken();
	if (isLexerError()) return pi;

	if (t.tp == SYMBOL && !strcmp(t.lx, "}")) {
		//printf("Found closing curly brace\n");
	} else {
		//printf("CLosing curly brace expected\n");
		pi.er = closeBraceExpected;
		pi.tk = t;
	}
	return pi;
}

// statement -> varDeclarStatement | letStatemnt | ifStatement | whileStatement | doStatement | returnStatemnt
ParserInfo statement() { 

	// Determine statement 
	if(!strcmp(t.lx, "var")) {
		//printf("Variable declaration statement peeked\n");
		pi = varDeclarStatement();
	} else if(!strcmp(t.lx, "let")) {
		//printf("Let statement peeked\n");
		pi = letStatement();
	} else if(!strcmp(t.lx, "if")) {
		//printf("If statement peeked\n");
		pi = ifStatement();
	} else if(!strcmp(t.lx, "while")) {
		//printf("While statement peeked\n");
		pi = whileStatement();
	} else if(!strcmp(t.lx, "do")) {
		//printf("Do statement peeked\n");
		pi = doStatement();
	} else if (!strcmp(t.lx, "return")) {
		//printf("return statement peeked\n");
		pi = returnStatement();
	}

	return pi;
}

// varDeclarStatement -> var type identifier { , identifier } ;
ParserInfo varDeclarStatement() { 

	// Eat "var"
	t = GetNextToken();
	if (isLexerError()) return pi;

	if(t.tp == RESWORD && !strcmp(t.lx, "var")){
		//printf("Variable declaration eaten");
	} else {
		//printf("Var expected, idk the error");
	}

	// Check if type
	t = PeekNextToken();
	if (isType()) {
		pi = type();
	} else {
		pi.er = illegalType;
		pi.tk = t;
	}

	if (pi.er != none) return pi; 

	// Eat first identifier
	t = GetNextToken();
	if (isLexerError()) return pi;
	if(t.tp == ID) {
		//printf("Identifier found = %s\n", t.lx);
		strcpy(currentVariableName, t.lx);

		if(phaseNumber == 1) {
			if (FindVariableSymbol(currentClassName, currentMethodName, currentVariableName)) {
				printf("redeclaration of variable");
				pi.er = redecIdentifier;
				pi.tk = t;
			} else {
				InsertVariableSymbol(currentClassName, currentMethodName, currentVariableName);
			}

		}
	} else {
		//printf("Identifier not found\n");
		pi.er = idExpected;
		pi.tk = t;
	}

	if (pi.er != none) return pi; 

	// Check if more identifiers, if so eat them
	t = PeekNextToken();
	while (t.tp == SYMBOL && !strcmp(t.lx, ",")) {
		
		t = GetNextToken();
		if (isLexerError()) return pi;
		t = GetNextToken();
		if (isLexerError()) return pi;

		if (t.tp == ID) {
			//printf("Identifier found = %s\n",t.lx);
			strcpy(currentVariableName, t.lx);
			InsertVariableSymbol(currentClassName, currentMethodName, currentVariableName);
		} else {
			//printf("Identifier not found");
			pi.er = idExpected;
			pi.tk = t;
		}
		t = PeekNextToken();
	}

	if (pi.er != none) return pi; 

	// Eat Semi-colon
	t = GetNextToken();
	if (isLexerError()) return pi;

	if(t.tp == SYMBOL && !strcmp(t.lx, ";")) {
		//printf("Semi-colon found\n");
	} else {
		pi.er = semicolonExpected;
		pi.tk = t;
	}

	return pi;
}

// letStatemnt -> let identifier [ [ expression ] ] = expression ;
ParserInfo letStatement() {

	// Eat "let"
	t = GetNextToken();
	if (isLexerError()) return pi;

	if (t.tp == RESWORD && !strcmp(t.lx, "let")) {
		//printf("Let identifier found\n");
	} else {
		//printf("Let identifier not found, idk the error code\n");
		pi.er = syntaxError;
		pi.tk = t;
	}

	if (pi.er != none) return pi; 

	// Eat identifier
	t = GetNextToken();
	if (isLexerError()) return pi;

	if (t.tp == ID) {

		strcpy(currentVariableName, t.lx);

		if (phaseNumber == 2) {

			if (FindVariableSymbol(currentClassName, currentMethodName, currentVariableName) || FindMethodSymbol(currentClassName, currentVariableName)) {
				// Do nothing
			} else {
				printf("Undeclared identifier");
				pi.er = undecIdentifier;
				pi.tk = t;
			}		

		}
	} else {
		//printf("Identifier not found\n");
		pi.er = idExpected;
		pi.tk = t;
	}

	if (pi.er != none) return pi; 

	// Check for "[expression] = expression" or just " = expression"
	t = PeekNextToken();

	if (t.tp == SYMBOL && !strcmp(t.lx, "=")) {
		
		t = GetNextToken();
		if (isLexerError()) return pi;
		//printf("Equals peeked\n");
	} else if (t.tp == SYMBOL && !strcmp(t.lx, "[")) {
		
		t = GetNextToken();
		if (isLexerError()) return pi;

		// Check for expression
		t = PeekNextToken();

		if (isFactorOrOperand()) {
			//printf("Expression peeked\n");
			pi = expression();
		} else {
			//printf("Expression expected, idk the error code\n");
			return pi;
		}
		
		if (pi.er != none) return pi; 

		// Eat closing square bracket 
		t = GetNextToken();
		if (isLexerError()) return pi;

		if (t.tp == SYMBOL && !strcmp(t.lx, "]")) {
			//printf("Closing square bracket found\n");
		} else {
			//printf("Expected closing square bracket\n");
			pi.er = closeBracketExpected;
			pi.tk = t;
		}

		if (pi.er != none) return pi; 

		// Eat "="
		t = GetNextToken();
		if (isLexerError()) return pi;

		if (t.tp == SYMBOL && !strcmp(t.lx, "=")) {
			//printf("Equals sign found\n");
		} else {
			//printf("equals sign expected\n");
			pi.er = equalExpected;
			pi.tk = t;
		}

	} else {
		//printf("Equals expected\n");
		pi.er = equalExpected;
		pi.tk = t;
	}

	if (pi.er != none) return pi; 

	// Check for expression
	t = PeekNextToken();

	if (isFactorOrOperand()) {
		//printf("Expression peeked\n");
		pi = expression();
	} else {
		//printf("Expression expected, idk the error code\n");
		pi.er = syntaxError;
		pi.tk = t;
	}

	if (pi.er != none) return pi; 

	// Eat Semi-colon
	t = GetNextToken();
	if (isLexerError()) return pi;

	if(t.tp == SYMBOL && !strcmp(t.lx, ";")) {
		//printf("Semi-colon found\n");
	} else {
		//printf("Semi-colon expected\n");
		pi.er = semicolonExpected;
		pi.tk = t;
	}

	return pi;
}

// ifStatement -> if ( expression ) { {statement} } [else { {statement} }]
ParserInfo ifStatement(){

	// Eat "if"
	t = GetNextToken();
	if (isLexerError()) return pi;

	// Eat opening paranthesis
	t = GetNextToken();
	if (isLexerError()) return pi;

	if (t.tp == SYMBOL && !strcmp(t.lx, "(")) {
		//printf("opening bracket found\n");
	} else {
		//printf("opening bracket expected\n");
		pi.er = openParenExpected;
		pi.tk = t;
	}

	if (pi.er != none) return pi; 

	// Check for expression
	t = PeekNextToken();

	if (isFactorOrOperand()) {
		//printf("Expression peeked\n");
		pi = expression();
	} else {
		//printf("Expression expected, idk the error code\n");
		pi.er = syntaxError;
		pi.tk = t;
	}

	if (pi.er != none) return pi; 

	// Eat closing parenthesis 
	t = GetNextToken();
	if (isLexerError()) return pi;

	if (t.tp == SYMBOL && !strcmp(t.lx, ")")) {
		//printf("Closing bracket found\n");
	} else {
		//printf("Closing bracket expected\n");
		pi.er = closeParenExpected;
		pi.tk = t;
	}

	if (pi.er != none) return pi; 

	// Eat opening curly brace
	t = GetNextToken();
	if (isLexerError()) return pi;

	if (t.tp == SYMBOL && !strcmp(t.lx, "{")) {
		//printf("opening curly brace found\n");
	} else {
		//printf("opening curly brace expected\n");
		pi.er = openBraceExpected;
		pi.tk = t;
	}

	// Check if  more statements
	t = PeekNextToken();

	while(isStatement()) {
		//printf("Statement peeked");
		pi = statement();

		if (pi.er != none) return pi; 

		t = PeekNextToken();
	} 


	t = GetNextToken();
	if (isLexerError()) return pi;

	if (t.tp == SYMBOL && !strcmp(t.lx, "}")) {
		//printf("Closing curly brace found\n");
	} else {
		//printf("Closing curly brace expected\n");
		pi.er = closeBraceExpected;
		pi.tk = t;
	}

	if (pi.er != none) return pi; 


	// Check for if-else statement
	t = PeekNextToken();

	if (t.tp == RESWORD && !strcmp(t.lx, "else")) {

		t = GetNextToken();
		if (isLexerError()) return pi;

		t = GetNextToken();
		if (isLexerError()) return pi;

		if (t.tp == SYMBOL && !strcmp(t.lx, "{")) {
			//printf("opening curly brace found\n");
		} else {
			//printf("opening curly brace expected\n");
			pi.er = openBraceExpected;
			pi.tk = t;
		}

		if (pi.er != none) return pi; 

		t = PeekNextToken();

		if (isStatement()) {
			//printf("Statement peeked\n");
			pi = statement();
		} else {
			//printf("Statement expected, idk the error code\n");
			pi.er = syntaxError;
			pi.tk = t;
		}

		// Check for extra statements 
		t = PeekNextToken();

		while(isStatement()) {
			//printf("Statement peeked");
			pi = statement();

			if (pi.er != none) return pi; 

			t = PeekNextToken();
		} 

		// Eat closing curly bracket
		t = GetNextToken();
		if (isLexerError()) return pi;

		if (t.tp == SYMBOL && !strcmp(t.lx, "}")) {
			//printf("Closing curly brace found\n");
		} else {
			//printf("Closing curly brace expected\n");
			pi.er = closeBraceExpected;
			pi.tk = t;
		}	
	}
	return pi;
}

// whileStatement -> while ( expression ) { {statement} }
ParserInfo whileStatement() {

	// Eat "while"
	t = GetNextToken();
	if (isLexerError()) return pi;

	// Eat opening curly parenthesis
	t = GetNextToken();
	if (isLexerError()) return pi;

	if (t.tp == SYMBOL && !strcmp(t.lx, "(")) {
		//printf("Opening bracket found\n");
	} else {
		//printf("Opening bracket expected");
		pi.er = openParenExpected;
		pi.tk = t;
	}
	
	if (pi.er != none) return pi; 

	t = PeekNextToken();

	if (isFactorOrOperand()) {
		//printf("Expression peeked\n");
		pi = expression();
	} else {
		//printf("Expression expected, idk the error code\n");
		pi.er = syntaxError;
		pi.tk = t;
	}

	if (pi.er != none) return pi; 

	t = GetNextToken();
	if (isLexerError()) return pi;

	if (t.tp == SYMBOL && !strcmp(t.lx, ")")) {
		//printf("Closing bracket found\n");
	} else {
		//printf("Closing bracket expected\n");
		pi.er = openBraceExpected;
		pi.tk = t;
	}

	if (pi.er != none) return pi; 

	// Eat opening curly brace
	t = GetNextToken();
	if (isLexerError()) return pi;

	if (t.tp == SYMBOL && !strcmp(t.lx, "{")) {
		//printf("opening curly brace found\n");
	} else {
		//printf("opening curly brace expected\n");
		pi.er = openBraceExpected;
		pi.tk = t;
	}

	if (pi.er != none) return pi; 

	// Check for more statements
	t = PeekNextToken();

	while(isStatement()) {
		//printf("Statement peeked");
		pi = statement();

		if (pi.er != none) return pi; 

		t = PeekNextToken();
	} 

	// Eat closing curly brace
	t = GetNextToken();
	if (isLexerError()) return pi;

	if (t.tp == SYMBOL && !strcmp(t.lx, "}")) {
		//printf("Closing curly brace found\n");
	} else {
		//printf("Closing curly brace expected\n");
		pi.er = closeBraceExpected;
		pi.tk = t;
	}
	return pi;
}

// doStatement -> do subroutineCall ; 
ParserInfo doStatement(){

	// Eat "do"
	t = GetNextToken();
	if (isLexerError()) return pi;

	if (t.tp == RESWORD && !strcmp(t.lx, "do")) {
		//printf("Do found\n");
	} else {
		//printf("Do expected\n");
		pi.er = syntaxError;
		pi.tk = t;
	}
	
	if (pi.er != none) return pi; 
	
	// Check for subroutine starting identifier
	t = PeekNextToken();

	if (t.tp == ID) {
		//printf("Subroutinecall peeked\n");
		pi = subroutineCall();

	} else {
		//printf("Identifier expected\n");
		pi.er = idExpected;
		pi.tk = t;
	}

	if (pi.er != none) return pi; 
	
	// Eat semi-colon
	t = GetNextToken();
	if (isLexerError()) return pi;

	if(t.tp == SYMBOL && !strcmp(t.lx, ";")) {
		//printf("Semi-colon found\n");
	} else {
		//printf("Semi-colon expected\n");
		pi.er = semicolonExpected;
		pi.tk = t;
	}
	return pi;
}

// subroutineCall -> identifier [ . identifier ] ( expressionList )
ParserInfo subroutineCall(){
	
	// Eat identifier
	t = GetNextToken();
	if (isLexerError()) return pi;

	if (t.tp == ID) {

		strcpy(classInstanceName, t.lx);
		// strcpy(currentVariableName, t.lx);
		if (phaseNumber == 2) {

			if (FindClassSymbol(classInstanceName) || FindMethodSymbol(currentClassName, classInstanceName) || FindVariableSymbol(currentClassName, currentMethodName, classInstanceName)) {
				// Do nothing
			} else if (IsJackLibraryClass(classInstanceName)) {
				strcpy(currentJackLibraryClass, t.lx); 
			} else {
				printf("Undeclared identifier POO");
				pi.er = undecIdentifier;
				pi.tk = t;
			}	
		}
	} else {
		//printf("Identifier not found");
		pi.er = idExpected;
		pi.tk = t;
	}

	if (pi.er != none) return pi; 

	// Check for ".identifier"
	t = PeekNextToken();

	if (t.tp == SYMBOL && !strcmp(t.lx, ".")) {
		
		// Eat "."
		t = GetNextToken();
		if (isLexerError()) return pi;

		// Eat identifier
		t = PeekNextToken();

		if (t.tp == ID) {
			t = GetNextToken();
			if (isLexerError()) return pi;
			//printf("Identifier found\n");

			strcpy(classInstanceMethodName, t.lx);
		
			if (phaseNumber == 2) {
				 if (FindMethodSymbol(classInstanceName, classInstanceMethodName)) {
					// Do nothing
				} else if (IsJackLibraryMethod(classInstanceName, classInstanceMethodName)) {
					// Do nothing also 
				} else {
					printf("Undeclared identifier");
					pi.er = undecIdentifier;
					pi.tk = t;
				}	
			}			
		} else {
			//printf("Identifier expected\n");
			pi.er = idExpected;
			pi.tk = t;
		}

		if (pi.er != none) return pi; 

	}

	// Eat opening parenthesis
	t = GetNextToken();
	if (isLexerError()) return pi;

	if (t.tp == SYMBOL && !strcmp(t.lx, "(")) {
		//printf("Opening bracket found\n");
	} else {
		//printf("Opening bracket expected");
		pi.er = openParenExpected;
		pi.tk = t;
	}

	if (pi.er != none) return pi; 

	// Check for expressionList
	t = PeekNextToken();

	if (isFactorOrOperand()) {
		//printf("Expression list peeked\n");
		pi = expressionList();
	} else {
		//printf("Expression list expected, idk the error code\n");
	}

	// Eat closing parenthesis 
	t = GetNextToken();
	if (isLexerError()) return pi;

	if (t.tp == SYMBOL && !strcmp(t.lx, ")")) {
		//printf("Closing bracket found WEEEEE\n");
	} else {
		//printf("Closing bracket expected \n");
		pi.er = closeParenExpected;
		pi.tk = t;
	}
	return pi;
}

// returnStatemnt -> return [ expression ] 
ParserInfo returnStatement(){

	// Eat "return"
	t = GetNextToken();
	if (isLexerError()) return pi;

	// Check if followed by expression
	t = PeekNextToken();

	if (isFactorOrOperand()) {
		//printf("Expression peeked\n");
		pi = expression();
	} 

	// Eat Semi-colon
	t = GetNextToken();
	if (isLexerError()) return pi;

	if(t.tp == SYMBOL && !strcmp(t.lx, ";")) {
		//printf("Semi-colon found\n");
	} else {
		//printf("Semi-colon expected\n");
		pi.er = semicolonExpected;
		pi.tk = t;
	}
	return pi;
}

// expressionList -> expression { , expression } | ε
ParserInfo expressionList() {

	if (isFactorOrOperand()) {
		//printf("Expression peeked\n");
		pi = expression();
	} 

	if (pi.er != none) return pi; 

	// Check for additional expressions in list
	t = PeekNextToken();

	while (t.tp == SYMBOL && !strcmp(t.lx, ",")) {
		//printf("expression list peeked !!!");
		t = GetNextToken();
		if (isLexerError()) return pi;
		t = PeekNextToken();

		if (isFactorOrOperand()) {
			//printf("Expression peeked");
			pi = expression();
		} else {
			//printf("Expression expected, idk the error code");
			return pi;
		}
		t = PeekNextToken();
	}
	return pi;
}

// expression -> relationalExpression { ( & | | ) relationalExpression }
ParserInfo expression() {

	// Check for relationalExpression
	if (isFactorOrOperand()) {
		//printf("Relational expression peeked\n");
		pi = relationalExpression();
	} else {
		//printf("Relational expression expected, idk the error code\n");
		pi.er = syntaxError;
		pi.tk = t;
	}
	
	if (pi.er != none) return pi; 
	
	// Check for additional " (& | |) relationalExpression"
	t = PeekNextToken(); 

	while (t.tp == SYMBOL && (!strcmp(t.lx, "&") || !strcmp(t.lx, "|"))) {

		// Eat symbol
		t = GetNextToken();
		if (isLexerError()) return pi;

		// Check for relationalExpression
		t = PeekNextToken();

		if (isFactorOrOperand()) {
			//printf("Relational expression peeked\n");
			pi = relationalExpression();
		} else {
			//printf("Relational expression expected, idk the error code\n");
			pi.er = syntaxError;
			pi.tk = t;
		}
		t = PeekNextToken();
	}
	return pi;
}

// relationalExpression -> ArithmeticExpression { ( = | > | < ) ArithmeticExpression }
ParserInfo relationalExpression() {

	// Check for arithmeticalExpression
	if (isFactorOrOperand()) {
		//printf("arithmetic expression peeked\n");
		pi = arithmeticExpression();
	} else {
		//printf("arithmetic expression expected, idk the error code\n");
		pi.er = syntaxError;
		pi.tk = t;
	}

	if (pi.er != none) return pi; 

	// Check for additional "( = | > | < ) ArithmeticExpression"
	while (t.tp == SYMBOL && (!strcmp(t.lx, "=") || !strcmp(t.lx, ">")) || !strcmp(t.lx, "<")) {

		t = GetNextToken();
		if (isLexerError()) return pi;
		t = PeekNextToken();

		if (isFactorOrOperand()) {
			//printf("Arithmetic expression peeked\n");
			pi = arithmeticExpression();
		} else {
			//printf("Arithmetic expression expected, idk the error code\n");
			pi.er = syntaxError;
			pi.tk = t;
		}

		if (pi.er != none) return pi; 

		t = PeekNextToken();
	}
	return pi;
}

// ArithmeticExpression -> term { ( + | - ) term }
ParserInfo arithmeticExpression() {

	// Check for term 
	if (isFactorOrOperand()) {
		//printf("Term peeked = %s\n", t.lx);
		pi = term();
	} else {
		//printf("Term expected, idk the error code");
		pi.er = syntaxError;
		pi.tk = t;
	}
	
	if (pi.er != none) return pi; 

	// Check for additional "( + | - ) term"
	t = PeekNextToken();

	while (t.tp == SYMBOL && (!strcmp(t.lx, "+") || !strcmp(t.lx, "-"))) {

		t = GetNextToken();
		if (isLexerError()) return pi;
		t = PeekNextToken();

		if (isFactorOrOperand()) {
			//printf("Term peeked\n");
			pi = term();
		} else {
			//printf("Term expected, idk the error code\n");
			pi.er = syntaxError;
			pi.tk = t;
		}

		if (pi.er != none) return pi; 

		t = PeekNextToken();
	}
	return pi;
}

// term -> factor { ( * | / ) factor }
ParserInfo term() {
	
	if (isFactorOrOperand()) {
		//printf("Factor peeked = %s\n", t.lx);
		pi = factor();
	} else {
		//printf("Factor expected, idk the error code\n");
		pi.er = syntaxError;
		pi.tk = t;
	}
	
	if (pi.er != none) return pi; 

	// Check for additional "( * | / ) factor"
	t = PeekNextToken();

	while (t.tp == SYMBOL && (!strcmp(t.lx, "*") || !strcmp(t.lx, "/"))) {

		t = GetNextToken();
		if (isLexerError()) return pi;
		t = PeekNextToken();

		if (isFactorOrOperand()) {
			//printf("Factor peeked\n");
			pi = factor();
		} else {
			//printf("Factor expected, idk the error code\n");
			pi.er = syntaxError;
			pi.tk = t;
		}

		if (pi.er != none) return pi; 


		t = PeekNextToken();
	}

	return pi;
}

// factor -> ( - | ~ | ε ) operand
ParserInfo factor() {

	// Check for "( - | ~ | ε )"
	if (t.tp == SYMBOL && !strcmp(t.lx, "-") || !strcmp(t.lx, "~")) {

		t = GetNextToken();
		if (isLexerError()) return pi;
		//printf("Symbol found");
	} 

	t = PeekNextToken();

	if (isFactorOrOperand()) {
		//printf("Operand found = %s\n", t.lx);
		pi = operand();
	} else {
		//printf("Operand expected, idk the error code\n");
		pi.er = syntaxError;
		pi.tk = t;
	}

	return pi;
}

// operand -> integerConstant | identifier [.identifier ] [ [ expression ] | (expressionList) ] | (expression) | stringLiteral | true | false | null | this
ParserInfo operand() {
	
	// Check if token is integerConstant or identifier
	if (isIntegerConstant(t.lx)) { // Integer constant
		
		// Eat integer constant
		t = GetNextToken();
		if (isLexerError()) return pi;
		//printf("Integer constant found\n");

	} else if (t.tp == ID) { // identifier [.identifier ] [ [ expression ] | (expressionList ) ]
		
		// Eat identifier 
		t = GetNextToken();
		if (isLexerError()) return pi;

		strcpy(currentVariableName, t.lx);
		if (phaseNumber == 2) {

			if (FindMethodSymbol(currentClassName, currentMethodName) || FindVariableSymbol(currentClassName, currentMethodName, currentVariableName)) {
				// Do nothing
			} else {
				printf("Undeclared identifier");
				pi.er = undecIdentifier;
				pi.tk = t;
			}		
		}

		if (pi.er != none) return pi;

		// Check if identifier is followed by .identifier
		t = PeekNextToken();
		if (t.tp == SYMBOL && !strcmp(t.lx, ".")) {
			
			t = GetNextToken();
			if (isLexerError()) return pi;

			t = GetNextToken();
			if (isLexerError()) return pi;
			if (t.tp == ID) {
				//printf("Identifier found = %s\n", t.lx);

				strcpy(currentVariableName, t.lx);
				if (phaseNumber == 2) {
					if (FindMethodSymbol(currentClassName, currentMethodName) || FindVariableSymbol(currentClassName, currentMethodName, currentVariableName)) {
						// Do nothing
					} else {
						printf("Undeclared identifier");
						pi.er = undecIdentifier;
						pi.tk = t;
					}	
				}	
			} else {
				//printf("Identifier expected\n");
				pi.er = idExpected;
				pi.tk = t;
			}

			if (pi.er != none) return pi; 

			t = PeekNextToken();
		} 

		if (t.tp == SYMBOL && !strcmp(t.lx, "[")) {
			
			t = GetNextToken();
			if (isLexerError()) return pi;
			//printf("Opening square bracket found\n");

			t = PeekNextToken();
			if (isFactorOrOperand()) {
				//printf("Expression peeked\n");
				pi = expression();
			} else {
				//printf("Expression expected, idk the error code\n");
				pi.er = syntaxError;
				pi.tk = t;
			}
			
			if (pi.er != none) return pi; 


			t = GetNextToken();
			if (isLexerError()) return pi;

			if (t.tp == SYMBOL && !strcmp(t.lx, "]")) {
				//printf("Found closing square bracket\n");
			} else {
				//printf("Closing square bracket expected");
				pi.er = closeBracketExpected;
				pi.tk = t;
			}
		} else if (t.tp == SYMBOL && !strcmp(t.lx, "(")) {
			
			t = GetNextToken();
			if (isLexerError()) return pi;
			//printf("Opening bracket found\n");
			//printf("token = %s", t.lx);

			t = PeekNextToken();
			if (isFactorOrOperand()) {
				//printf("Expression peeked\n");
				pi = expressionList();
			}
			
			if (pi.er != none) return pi; 

			t = GetNextToken();
			if (isLexerError()) return pi;

			if (t.tp == SYMBOL && !strcmp(t.lx, ")")) {
				//printf("Found closing bracket\n");
			} else {
				//printf("Closing bracket expected");
				pi.er = closeParenExpected;
				pi.tk = t;
			}
		}
	} else if (t.tp == SYMBOL && !strcmp(t.lx, "(")) { // (expression)

		t = GetNextToken();
		if (isLexerError()) return pi;
		t = PeekNextToken();

		if (isFactorOrOperand()) {
			//printf("expression peeked\n");
			pi = expression();
		} else {
			//printf("Expression expected, idk the error code\n");
		}

		if (pi.er != none) {
			return pi;
		}

		t = GetNextToken();
		if (isLexerError()) return pi;

		if (t.tp == SYMBOL && !strcmp(t.lx, ")")) {
			//printf("Closing bracket found");
		} else {
			//printf("Closing bracket expected");
			pi.er = closeParenExpected;
			pi.tk = t;
		}
	} else if (t.tp == STRING) {

		t = GetNextToken();
		if (isLexerError()) return pi;
		//printf("String literal found");

	} else if (t.tp == RESWORD) {

		//printf("Reserved word peeked\n");

		t = GetNextToken();
		if (isLexerError()) return pi;

		if(!strcmp(t.lx, "true") || !strcmp(t.lx, "false") || !strcmp(t.lx, "this") || !strcmp(t.lx, "null")) {
			//printf("Reserved word found = %s\n", t.lx);
		} else { 
			//printf("Illegal reserved word, idk the error\n");
		}
	}
	return pi;
}

int InitParser (char* file_name) {

	InitLexer(file_name);
	return 1;
	
}

ParserInfo Parse ()
{

	// phaseNumber = 2;
	classDeclar();
	printf("pi.er = %u, pi.tk = %s\n",  pi.er , t.lx);
	// PrintSymbols();
	return pi; 


	// pi.er = none;

	// classDeclar();
	
	// if (pi.er != none) {
	// 	return pi;
	// }

	// printf("pi.er = %u, pi.tk = %s\n",  pi.er , t.lx);
	// PrintSymbols();

	// RewindFile();

	// phaseNumber = 2;

	// classDeclar();

	// printf("pi.er = %u, pi.tk = %s\n",  pi.er , t.lx);
	// PrintSymbols();


	// return pi; 
}

int StopParser ()
{
	StopLexer();
	return 1;
}

// #ifndef TEST_PARSER
// int main ()
// {
	
// 	InitParser("Pong/Main.jack");
// 	Parse();
// 	return 1;
// }
// #endif

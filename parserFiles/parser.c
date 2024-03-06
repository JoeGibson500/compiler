#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lexer.h"
#include "parser.h"


// you can declare prototypes of parser functions below

ParserInfo classDeclar();
ParserInfo memberDeclar();
ParserInfo classVarDeclar();
ParserInfo type();
ParserInfo subroutineDeclar();
ParserInfo paramList(); 
ParserInfo subroutineBody();
ParserInfo statement(Token* token);
// ParserInfo varDeclarStatement();
// ParserInfo letStatement();


ParserInfo pi;

// classDeclar -> class identifier { {memberDeclar} }
ParserInfo classDeclar() {

	// Eat Starting word "class"
	Token token = GetNextToken();
	if (token.tp == RESWORD && !strcmp(token.lx, "class")) {
		printf("Class found\n");
	} else {
		printf("Class not found\n");
		pi.er = classExpected;
		pi.tk = token;
	}

	if (pi.er != none) {
		return pi;
	}

	// Eat name of class
	token = GetNextToken();
	if (token.tp == ID) {
		printf("Identifier found\n");
	} else {
		printf("Identifier not found\n");
		pi.er = idExpected;
		pi.tk = token;
	}

	if (pi.er != none) {
		return pi;
	}

	// Eat opening brace 
	token = GetNextToken();
	if (token.tp == SYMBOL && !strcmp(token.lx, "{")) {
		printf("Curly brace found\n");
	} else {
		printf("Curly brace not found\n");
		pi.er = openBraceExpected;
		pi.tk = token;
	}

	if (pi.er != none) {
		return pi;
	}	

	// Eat Member decaration(s)  
	token = PeekNextToken();

	// while (token.tp != SYMBOL || strcmp(token.lx, "}")) {
	for (int i =0; i<3;i++) {
        
		pi = memberDeclar();

		if (pi.er != none) {
			return pi;
		}

        token = PeekNextToken();
	}	

	

	// Eat Closing brace
	token = GetNextToken();
	if (token.tp == SYMBOL && !strcmp(token.lx, "}")) {
		printf("Closing brace found\n");
	} else {
		printf("Closing brace not found\n");
		pi.er = closeBraceExpected;
		pi.tk = token;
	}

	return pi;
}

// memberDeclar -> classVarDeclar | subRoutineDeclar
ParserInfo memberDeclar() {
	
	// Check if class variable or subroutine declaration
	Token token = PeekNextToken();
	if (token.tp == RESWORD && (!strcmp(token.lx, "static") || !strcmp(token.lx, "field"))){
		printf("classVar peeked\n");
		pi = classVarDeclar();
	} else if ((token.tp == RESWORD) && (!strcmp(token.lx, "function") || !strcmp(token.lx, "constructor") || !strcmp(token.lx, "method"))) {
		printf("memberVar peeked\n");
		pi = subroutineDeclar();
	} else {
		printf("Member declaration not found\n");
		pi.er = memberDeclarErr;
		pi.tk = token;
	}
	return pi;
}

// classVarDeclar -> (static | field) type identifier {, identifier} ;
ParserInfo classVarDeclar() {
	
	// Eat Static or field resererved word 
	Token token = GetNextToken();
	if (token.tp == RESWORD && (!strcmp(token.lx, "static") || !strcmp(token.lx, "field"))){
		printf("Variable declaration keyword found\n");
	} else {
		printf("Variable declaration keyword not found\n");
		pi.er = classVarErr;
		pi.tk = token;
	}	

	if (pi.er != none) {
		return pi;
	}

	// Check if type
	Token PeekNextToken();
	if (token.tp == RESWORD) {
		pi = type();
	} else {
		pi.er = illegalType;
		pi.tk = token;
	}

	if (pi.er != none) {
		return pi;
	}

	// Eat first identifier
	token = GetNextToken();
	if(token.tp == ID) {
		printf("Identifier found = %s\n", token.lx);
	} else {
		printf("Identifier not found\n");\
		pi.er = idExpected;
		pi.tk = token;
	}
	if (pi.er != none) {
		return pi;
	}	

	// Check if more identifiers, if so eat them
	token = PeekNextToken();
	while(token.tp == SYMBOL && !strcmp(token.lx, ",")) {
		
		token = GetNextToken();
		token = GetNextToken();

		if (token.tp == ID) {
			printf("Identifier found = %s\n",token.lx);
		} else {
			printf("Identifier not found");
			pi.er = idExpected;
			pi.tk = token;
		}
		token = PeekNextToken();
	}

	if (pi.er != none) {
		return pi;
	}	

	// Eat Semi-colon
	token = GetNextToken();
	if(token.tp == SYMBOL && !strcmp(token.lx, ";")) {
		printf("Semi-colon found\n");
	} else {
		pi.er = semicolonExpected;
		pi.tk = token;
	}

	return pi;
}

// type -> int | char | boolean | identifier
ParserInfo type() {

	// Eat type reserved word
	Token token = GetNextToken();
	if (token.tp == RESWORD && (!strcmp(token.lx, "int") | !strcmp(token.lx, "char") | !strcmp(token.lx, "boolean") | !strcmp(token.lx, "identifier"))) {
		printf("Variable type found\n");
	} else {
		pi.er = illegalType;
		pi.tk = token;
	}

	return pi;
}

// subroutine -> (constructor | function | method) (type|ParserInfo) identifier (paramList) subroutineBody
ParserInfo subroutineDeclar() {

	// Eat constructor or function or method resererved word 
	Token token = GetNextToken();
	if (token.tp == RESWORD && (!strcmp(token.lx, "constructor") || !strcmp(token.lx, "function") || !strcmp(token.lx, "method"))){
		printf("Subroutine declaration keyword found\n");
	} else {
		printf("Subroutine declaration keyword not found\n");
		pi.er = subroutineDeclarErr;
		pi.tk = token;
	}	

	if (pi.er != none) {
		return pi;
	}

	token = PeekNextToken();
	if (token.tp == RESWORD) {
		if(!strcmp(token.lx, "void")) {
			token = GetNextToken();
			printf("Return type found = void\n");
		} else {
			pi = type();
			printf("Return type found = %s\n", token.lx);

		}
	} else {
		printf("Return type not found\n");
		// idk the error
		pi.er = illegalType;
		pi.tk = token;
	}
	
	if (pi.er != none) {
		return pi;
	}

	token = GetNextToken();

	if (token.tp == ID) {
		printf("Identifier found\n");
	} else {
		printf("Identifier not found\n");
		pi.er = idExpected;
		pi.tk = token;
	}

	if (pi.er != none) {
		return pi;
	}

	token = GetNextToken();

	if (token.tp == SYMBOL && !strcmp(token.lx, "(")) {
		printf("opening bracket found\n");
	} else {
		printf("opening bracket not found\n");
		pi.er = openParenExpected;
		pi.tk = token;
	}

	token = PeekNextToken();

	// call Paramlist();
	if (token.tp == RESWORD) {
		printf("Paramater peeked\n");
		pi = paramList();
	} else {
		printf("illegal paramater\n");
		pi.er = illegalType;
		pi.tk = token;
	}

	if (pi.er != none) {
		return pi;
	}

	token = GetNextToken();

	if (token.tp == SYMBOL && !strcmp(token.lx, ")")) {
		printf("closing bracket found\n");
	} else {
		printf("closing bracket not found\n");
		pi.er = closeBracketExpected;
		pi.tk = token;
	}

	if (pi.er != none) {
		return pi;
	}

	token = PeekNextToken(); 

	if (token.tp == SYMBOL && !strcmp(token.lx, "{")) {
		printf("Peeked subroutineBody\n");
		pi = subroutineBody();
	} else {
		pi.er = openBraceExpected;
		pi.tk = token;
	}

	return pi;
}

// paramList -> type identifier {, type identifier} | ε
ParserInfo paramList() { 

	Token token = PeekNextToken();

	if (token.tp == RESWORD) {
		pi = type();
	}

	if (pi.er != none) {
		return pi;
	}

	token = GetNextToken();

	if (token.tp == ID) {
		printf("Identifier found\n");
	} else {
		printf("Identifier not found\n");
		pi.er = idExpected;
		pi.tk = token;
	}

	if (pi.er != none) {
		return pi;
	}

	token = PeekNextToken();

	if (strcmp(token.lx, "")) {

		while (token.tp == SYMBOL && !strcmp(token.lx, ",")) {

			token = GetNextToken();
			token = PeekNextToken();

			if (token.tp == RESWORD) {
				pi = type();
			} else {
				printf("Illegal type");
				pi.er = illegalType;
				pi.tk = token;
			}

			token = GetNextToken();

			if (token.tp == ID) {
				printf("Identifier found\n");
			} else {
				printf("Identifier not found\n");
				pi.er = idExpected;
				pi.tk = token;
			}
		}
	}

	return pi;
}

ParserInfo subroutineBody() { 

	Token token = GetNextToken();

	if (token.tp == SYMBOL && !strcmp(token.lx, "{")) {
		printf("Curly brace found\n");
	} else {
		printf("Curly brace not found\n");
		pi.er = openBraceExpected;
		pi.tk = token;
	}

	if (pi.er != none) {
		return pi;
	}

	token = PeekNextToken();

	if (token.tp == RESWORD) {
		printf("Statement peeked\n");
		statement(&token);
	} else {
		printf("idk what error this is");
	}

	printf("Token = %s\n", token.lx);
	return pi;
}

// statement -> varDeclarStatement | letStatemnt | ifStatement | whileStatement | doStatement | returnStatemnt
ParserInfo statement(Token* token) { 

	if(!strcmp(token->lx, "var")) {
		printf("Variable declaration statement peeked\n");
		// pi = varDeclarStatement();
	} else if(!strcmp(token->lx, "let")) {
		printf("Let statement peeked\n");
		// pi = letStatement();
	} else if(!strcmp(token->lx, "if")) {
		printf("If statement peeked\n");
	} else if(!strcmp(token->lx, "while")) {
		printf("While statement peeked\n");
	} else if(!strcmp(token->lx, "do")) {
		printf("Do statement peeked\n");
	} 	

	return pi;
}

// // varDeclarStatement -> var type identifier { , identifier } ;
// ParserInfo varDeclarStatement() { 

// 	Token token = GetNextToken();

// 	if(token.tp == RESWORD && !strcmp(token.lx, "var")){
// 		printf("Variable declaration eaten");
// 	} else {
// 		printf("Var expected, idk the error");
// 	}

// 	// Check if type
// 	token = PeekNextToken();
// 	if (token.tp == RESWORD) {
// 		pi = type();
// 	} else {
// 		pi.er = illegalType;
// 		pi.tk = token;
// 	}

// 	if (pi.er != none) {
// 		return pi;
// 	}

// 	// Eat first identifier
// 	token = GetNextToken();
// 	if(token.tp == ID) {
// 		printf("Identifier found = %s\n", token.lx);
// 	} else {
// 		printf("Identifier not found\n");\
// 		pi.er = idExpected;
// 		pi.tk = token;
// 	}
// 	if (pi.er != none) {
// 		return pi;
// 	}	


// 	// Check if more identifiers, if so eat them
// 	token = PeekNextToken();
// 	while(token.tp == SYMBOL && !strcmp(token.lx, ",")) {
		
// 		token = GetNextToken();
// 		token = GetNextToken();

// 		if (token.tp == ID) {
// 			printf("Identifier found = %s\n",token.lx);
// 		} else {
// 			printf("Identifier not found");
// 			pi.er = idExpected;
// 			pi.tk = token;
// 		}
// 		token = PeekNextToken();
// 	}

// 	if (pi.er != none) {
// 		return pi;
// 	}	

// 	// Eat Semi-colon
// 	token = GetNextToken();
// 	if(token.tp == SYMBOL && !strcmp(token.lx, ";")) {
// 		printf("Semi-colon found\n");
// 	} else {
// 		pi.er = semicolonExpected;
// 		pi.tk = token;
// 	}
// }

// // letStatemnt -> let identifier [ [ expression ] ] = expression ;
// ParserInfo letStatement() {

// 	Token token = GetNextToken();

// 	if (token.tp == RESWORD && !strcmp(token.lx, "let")) {
// 		printf("Let identifier found\n");
// 	} else {
// 		printf("Let identifier not found, idk the error code\n");
// 	}

// 	token = GetNextToken();

// 	if (token.tp == ID) {
// 		printf("Identifier found\n");
// 	} else {
// 		printf("Identifier not found");
// 		pi.er = idExpected;
// 		pi.tk = token;
// 	}

// 	if (pi.er != none) {
// 		return pi;
// 	}

// 	token = PeekNextToken();

// 	if (token.tp == SYMBOL && !strcmp(token.lx, "=")) {
// 		printf("Equals peeked");
// 	}
// 	// } else if (token.tp == SYMBOL && !strcmp(token.lx, "[")) {
// 	// 	token = GetNextToken();
// 	// 	token = PeekNextToken();

// 	// 	// call expression();  NOT FINISHED****
// 	// }

// 	if (pi.er != none) {
// 		return pi;
// 	}

// 	// token = PeekNextToken();

// 	// again call expression();


// 	if (pi.er != none) {
// 		return pi;
// 	}	

// 	// // Eat Semi-colon
// 	// token = GetNextToken();
// 	// if(token.tp == SYMBOL && !strcmp(token.lx, ";")) {
// 	// 	printf("Semi-colon found\n");
// 	// } else {
// 	// 	pi.er = semicolonExpected;
// 	// 	pi.tk = token;
// 	// }

// }

int InitParser (char* file_name)
{
	InitLexer(file_name);
	return 1;
}

ParserInfo Parse ()
{
	// ParserInfo pi;
	// implement the function
	pi.er = none;

	classDeclar();
	printf("pi.er = %u", pi.er);
	if (pi.er != none) {
		return pi;
	}

	return pi;
}

int StopParser ()
{
	return 1;
}

#ifndef TEST_PARSER
int main ()
{
	InitParser("justVariables.jack");
	Parse();
	
	return 1;
}
#endif

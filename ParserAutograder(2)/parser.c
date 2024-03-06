#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lexer.h"
#include "parser.h"


// you can declare prototypes of parser functions below

void classDeclar(ParserInfo* pi);
void memberDeclar(ParserInfo* pi);
void classVarDeclar(ParserInfo* pi);
void type(ParserInfo* pi);
void subroutineDeclar(ParserInfo* pi);
void paramList(ParserInfo* pi); 
void subroutineBody(ParserInfo* pi);
void statement(ParserInfo* pi, Token* token);
void varDeclarStatement(ParserInfo* pi);
void letStatement(ParserInfo* pi);




// classDeclar -> class identifier { {memberDeclar} }
void classDeclar(ParserInfo* pi) {

	// Eat Starting word "class"
	Token token = GetNextToken();
	if (token.tp == RESWORD && !strcmp(token.lx, "class")) {
		printf("Class found\n");
	} else {
		printf("Class not found\n");
		pi->er = classExpected;
		pi->tk = token;
	}

	if (pi->er != none) {
		return;
	}

	// Eat name of class
	token = GetNextToken();
	if (token.tp == ID) {
		printf("Identifier found\n");
	} else {
		printf("Identifier not found\n");
		pi->er = idExpected;
		pi->tk = token;
	}

	if (pi->er != none) {
		return;
	}

	// Eat opening brace 
	token = GetNextToken();
	if (token.tp == SYMBOL && !strcmp(token.lx, "{")) {
		printf("Curly brace found\n");
	} else {
		printf("Curly brace not found\n");
		pi->er = openBraceExpected;
		pi->tk = token;
	}

	if (pi->er != none) {
		return;
	}	

	// Eat Member decaration(s)  
	token = PeekNextToken();

	// while (token.tp != SYMBOL || strcmp(token.lx, "}")) {
	for (int i =0; i<3;i++) {
        memberDeclar(pi);
        token = PeekNextToken();
    }

	if (pi->er != none) {
		return;
	}

	// Eat Closing brace
	token = GetNextToken();
	if (token.tp == SYMBOL && !strcmp(token.lx, "}")) {
		printf("Closing brace found\n");
	} else {
		printf("Closing brace not found\n");
		pi->er = closeBraceExpected;
		pi->tk = token;
	}
}

// memberDeclar -> classVarDeclar | subRoutineDeclar
void memberDeclar(ParserInfo* pi) {
	
	// Check if class variable or subroutine declaration
	Token token = PeekNextToken();
	if (token.tp == RESWORD && (!strcmp(token.lx, "static") || !strcmp(token.lx, "field"))){
		printf("classVar peeked\n");
		classVarDeclar(pi);
	} else if ((token.tp == RESWORD) && (!strcmp(token.lx, "function") || !strcmp(token.lx, "constructor") || !strcmp(token.lx, "method"))) {
		printf("memberVar peeked\n");
		subroutineDeclar(pi);
	} else {
		printf("Member declaration not found\n");
		pi->er = memberDeclarErr;
		pi->tk = token;
	}
}

// classVarDeclar -> (static | field) type identifier {, identifier} ;
void classVarDeclar(ParserInfo* pi) {
	
	// Eat Static or field resererved word 
	Token token = GetNextToken();
	if (token.tp == RESWORD && (!strcmp(token.lx, "static") || !strcmp(token.lx, "field"))){
		printf("Variable declaration keyword found\n");
	} else {
		printf("Variable declaration keyword not found\n");
		pi->er = classVarErr;
		pi->tk = token;
	}	

	if (pi->er != none) {
		return;
	}

	// Check if type
	Token PeekNextToken();
	if (token.tp == RESWORD) {
		type(pi);
	} else {
		pi->er = illegalType;
		pi->tk = token;
	}

	if (pi->er != none) {
		return;
	}

	// Eat first identifier
	token = GetNextToken();
	if(token.tp == ID) {
		printf("Identifier found = %s\n", token.lx);
	} else {
		printf("Identifier not found\n");\
		pi->er = idExpected;
		pi->tk = token;
	}
	if (pi->er != none) {
		return;
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
			pi->er = idExpected;
			pi->tk = token;
		}
		token = PeekNextToken();
	}

	if (pi->er != none) {
		return;
	}	

	// Eat Semi-colon
	token = GetNextToken();
	if(token.tp == SYMBOL && !strcmp(token.lx, ";")) {
		printf("Semi-colon found\n");
	} else {
		pi->er = semicolonExpected;
		pi->tk = token;
	}
}

// type -> int | char | boolean | identifier
void type(ParserInfo* pi) {

	// Eat type reserved word
	Token token = GetNextToken();
	if (token.tp == RESWORD && (!strcmp(token.lx, "int") | !strcmp(token.lx, "char") | !strcmp(token.lx, "boolean") | !strcmp(token.lx, "identifier"))) {
		printf("Variable type found\n");
	} else {
		pi->er = illegalType;
		pi->tk = token;
	}
}

// subroutine -> (constructor | function | method) (type|void) identifier (paramList) subroutineBody
void subroutineDeclar(ParserInfo* pi) {

	// Eat constructor or function or method resererved word 
	Token token = GetNextToken();
	if (token.tp == RESWORD && (!strcmp(token.lx, "constructor") || !strcmp(token.lx, "function") || !strcmp(token.lx, "method"))){
		printf("Subroutine declaration keyword found\n");
	} else {
		printf("Subroutine declaration keyword not found\n");
		pi->er = subroutineDeclarErr;
		pi->tk = token;
	}	

	if (pi->er != none) {
		return;
	}

	token = PeekNextToken();
	if (token.tp == RESWORD) {
		if(!strcmp(token.lx, "void")) {
			token = GetNextToken();
			printf("Return type found = void\n");
		} else {
			type(pi);
			printf("Return type found = %s\n", token.lx);

		}
	} else {
		printf("Return type not found\n");
		// idk the error
		pi->er = illegalType;
		pi->tk = token;
	}
	
	if (pi->er != none) {
		return;
	}

	token = GetNextToken();

	if (token.tp == ID) {
		printf("Identifier found\n");
	} else {
		printf("Identifier not found\n");
		pi->er = idExpected;
		pi->tk = token;
	}

	if (pi->er != none) {
		return;
	}

	token = GetNextToken();

	if (token.tp == SYMBOL && !strcmp(token.lx, "(")) {
		printf("opening bracket found\n");
	} else {
		printf("opening bracket not found\n");
		pi->er = openParenExpected;
		pi->tk = token;
	}

	token = PeekNextToken();

	// call Paramlist();
	if (token.tp == RESWORD) {
		printf("Paramater peeked\n");
		paramList(pi);
	} else {
		printf("illegal paramater\n");
		pi->er = illegalType;
		pi->tk = token;
	}

	if (pi->er != none) {
		return;
	}

	token = GetNextToken();

	if (token.tp == SYMBOL && !strcmp(token.lx, ")")) {
		printf("closing bracket found\n");
	} else {
		printf("closing bracket not found\n");
		pi->er = closeBracketExpected;
		pi->tk = token;
	}

	if (pi->er != none) {
		return;
	}

	token = PeekNextToken(); 

	if (token.tp == SYMBOL && !strcmp(token.lx, "{")) {
		printf("Peeked subroutineBody\n");
		subroutineBody(pi);
	} else {
		pi->er = openBraceExpected;
		pi->tk = token;
	}
}

// paramList -> type identifier {, type identifier} | ε
void paramList(ParserInfo* pi) { 

	Token token = PeekNextToken();

	if (token.tp == RESWORD) {
		type(pi);
	}

	if (pi->er != none) {
		return;
	}

	token = GetNextToken();

	if (token.tp == ID) {
		printf("Identifier found\n");
	} else {
		printf("Identifier not found\n");
		pi->er = idExpected;
		pi->tk = token;
	}

	if (pi->er != none) {
		return;
	}

	token = PeekNextToken();

	if (strcmp(token.lx, "")) {

		while (token.tp == SYMBOL && !strcmp(token.lx, ",")) {

			token = GetNextToken();
			token = PeekNextToken();

			if (token.tp == RESWORD) {
				type(pi);
			} else {
				printf("Illegal type");
				pi->er = illegalType;
				pi->tk = token;
			}

			token = GetNextToken();

			if (token.tp == ID) {
				printf("Identifier found\n");
			} else {
				printf("Identifier not found\n");
				pi->er = idExpected;
				pi->tk = token;
			}
		}
	}
}

void subroutineBody(ParserInfo* pi) { 

	Token token = GetNextToken();

	if (token.tp == SYMBOL && !strcmp(token.lx, "{")) {
		printf("Curly brace found\n");
	} else {
		printf("Curly brace not found\n");
		pi->er = openBraceExpected;
		pi->tk = token;
	}

	if (pi->er != none) {
		return;
	}

	token = PeekNextToken();

	if (token.tp == RESWORD) {
		printf("statement peeked");
		statement(pi, &token);
	} else {
		printf("idk what error this is");
	}
	printf("token =%s\n", token.lx);
}

// statement -> varDeclarStatement | letStatemnt | ifStatement | whileStatement | doStatement | returnStatemnt
void statement(ParserInfo* pi, Token* token) { 

	if(!strcmp(token->lx, "var")){
		printf("Variable declaration statement peeked\n");
		varDeclarStatement(pi);
	} else if(!strcmp(token->lx, "let")){
		printf("Let statement peeked\n");
		letStatement(pi);
	} else if(!strcmp(token->lx, "if")){
		printf("If statement peeked\n");
	} else if(!strcmp(token->lx, "while")){
		printf("While statement peeked\n");
	} else if(!strcmp(token->lx, "do")){
		printf("Do statement peeked\n");
	} 	

}

// varDeclarStatement -> var type identifier { , identifier } ;
void varDeclarStatement(ParserInfo* pi) { 

	Token token = GetNextToken();

	if(token.tp == RESWORD && !strcmp(token.lx, "var")){
		printf("Variable declaration eaten");
	} else {
		printf("Var expected, idk the error");
	}

	// Check if type
	token = PeekNextToken();
	if (token.tp == RESWORD) {
		type(pi);
	} else {
		pi->er = illegalType;
		pi->tk = token;
	}

	if (pi->er != none) {
		return;
	}

	// Eat first identifier
	token = GetNextToken();
	if(token.tp == ID) {
		printf("Identifier found = %s\n", token.lx);
	} else {
		printf("Identifier not found\n");\
		pi->er = idExpected;
		pi->tk = token;
	}
	if (pi->er != none) {
		return;
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
			pi->er = idExpected;
			pi->tk = token;
		}
		token = PeekNextToken();
	}

	if (pi->er != none) {
		return;
	}	

	// Eat Semi-colon
	token = GetNextToken();
	if(token.tp == SYMBOL && !strcmp(token.lx, ";")) {
		printf("Semi-colon found\n");
	} else {
		pi->er = semicolonExpected;
		pi->tk = token;
	}
}

// letStatemnt -> let identifier [ [ expression ] ] = expression ;
void letStatement(ParserInfo* pi) {

	Token token = GetNextToken();

	if (token.tp == RESWORD && !strcmp(token.lx, "let")) {
		printf("Let identifier found\n");
	} else {
		printf("Let identifier not found, idk the error code\n");
	}

	token = GetNextToken();

	if (token.tp == ID) {
		printf("Identifier found\n");
	} else {
		printf("Identifier not found");
		pi->er = idExpected;
		pi->tk = token;
	}

	if (pi->er != none) {
		return;
	}

	token = PeekNextToken();

	if (token.tp == SYMBOL && !strcmp(token.lx, "=")) {
		printf("Equals peeked");
	}
	// } else if (token.tp == SYMBOL && !strcmp(token.lx, "[")) {
	// 	token = GetNextToken();
	// 	token = PeekNextToken();

	// 	// call expression();  NOT FINISHED****
	// }

	if (pi->er != none) {
		return;
	}

	// token = PeekNextToken();

	// again call expression();


	if (pi->er != none) {
		return;
	}	

	// // Eat Semi-colon
	// token = GetNextToken();
	// if(token.tp == SYMBOL && !strcmp(token.lx, ";")) {
	// 	printf("Semi-colon found\n");
	// } else {
	// 	pi->er = semicolonExpected;
	// 	pi->tk = token;
	// }

}

int InitParser (char* file_name)
{
	InitLexer(file_name);
	return 1;
}

ParserInfo Parse ()
{
	ParserInfo pi;
	// implement the function
	pi.er = none;

	classDeclar(&pi);
	printf("pi->er = %u", pi.er);
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

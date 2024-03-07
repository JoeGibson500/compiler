#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

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

ParserInfo pi;
Token token;

// an expression starts with either -|~|integerConstant|identifier| (expression)| stringLiteral | true | false | null | this
bool isFactorOrOperand() {

	if(!strcmp(token.lx, "-") || !strcmp(token.lx, "~")) {
		return true;
	} else if (isIntegerConstant(token.lx)) {
		return true;
	} else if (token.tp == ID) {
		return true;
	} else if (token.tp == STRING) {
		return true;
	} else if (!strcmp(token.lx, "true") || !strcmp(token.lx, "false") || !strcmp(token.lx, "null") || !strcmp(token.lx, "this")) {
		return true;
	} else if (token.tp == SYMBOL && !strcmp(token.lx, "(")) {
		// printf("bracket peeked\n");
		return true;
		// token = PeekNextToken();
		// if (isFactorOrOperand()) {
		// 	return true;
		// }
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

	if (!strcmp(token.lx, "var") || !strcmp(token.lx, "let") || !strcmp(token.lx, "if") || !strcmp(token.lx, "while") || !strcmp(token.lx, "do") || !strcmp(token.lx, "return")){
		return true;
	} else {
		return false;
	}
}

bool isType() {
	
	if (token.tp == ID) {
		return true;
	} else if (!strcmp(token.lx, "int") || !strcmp(token.lx, "char") || !strcmp(token.lx, "boolean")) {
		return true;
	} else {
		return false;
	}
}

// classDeclar -> class identifier { {memberDeclar} }
ParserInfo classDeclar() {

	// Eat Starting word "class"
	token = GetNextToken();
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

	while (token.tp != SYMBOL || strcmp(token.lx, "}")) {
	// for (int i =0; i<5;i++) {
        
		printf("MEMBER DECLAR PEEKED ***********;\n");
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
	token = PeekNextToken();

	printf("token = %s\n", token.lx);
	if (token.tp == RESWORD && (!strcmp(token.lx, "static") || !strcmp(token.lx, "field"))){
		printf("classVar peeked\n");
		pi = classVarDeclar();
	} else if ((token.tp == RESWORD) && (!strcmp(token.lx, "function") || !strcmp(token.lx, "constructor") || !strcmp(token.lx, "method"))) {
		printf("subroutineDeclar peeked\n");
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
	token = GetNextToken();
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
	token = PeekNextToken();
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
	token = GetNextToken();
	if (!strcmp(token.lx, "int") || !strcmp(token.lx, "char") || !strcmp(token.lx, "boolean")) {
		printf("Variable type found\n");
	} else if (token.tp == ID) {
		printf("CONSTRUCTOR IDENTIFIER FOUND !!!");
	}
	 else {
		printf("Illegal type\n!");
		pi.er = illegalType;
		pi.tk = token;
	}

	return pi;
}

// subroutine -> (constructor | function | method) (type|ParserInfo) identifier (paramList) subroutineBody
ParserInfo subroutineDeclar() {

	// Eat constructor or function or method resererved word 
	token = GetNextToken();
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
	if(!strcmp(token.lx, "void")) {
		token = GetNextToken();
		printf("Return type found = void\n");
	} else if (token.tp == ID) {
		pi = type();
		printf("Return type found = %s\n", token.lx);
	} else {
		printf("Return type not found = %s\n", token.lx);
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
		printf("Parameter peeked\n");
		pi = paramList();
	}
	// } else {
	// 	printf("illegal parameter\n");
	// 	pi.er = illegalType;
	// 	pi.tk = token;
	// }

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

	token = PeekNextToken();

	if (isType()) {
		pi = type();
	} else {
		printf("Type expected\n");
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

	token = PeekNextToken();

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

		token = PeekNextToken();
	}

	return pi;
}

//subroutineBody -> { {statement} }
ParserInfo subroutineBody() { 

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

	token = PeekNextToken();

	if (token.tp == RESWORD) {
		printf("Statement peeked\n");
		pi = statement();
	} else {
		printf("Statement expected, idk error code\n");
	}

	if (pi.er != none) {
		return pi;
	}


	token = PeekNextToken();
	// if(isStatement()) {

	while(isStatement()) {
		printf("Statement peeked\n");
		pi = statement();

		if (pi.er != none) {
			return pi;
		}

		token = PeekNextToken();
	}

	token = GetNextToken();

	if (token.tp == SYMBOL && !strcmp(token.lx, "}")) {
		printf("Found closing curly brace\n");
	} else {
		printf("CLosing curly brace expected\n");
		pi.er = closeBraceExpected;
		pi.tk = token;
	}
	return pi;
}

// statement -> varDeclarStatement | letStatemnt | ifStatement | whileStatement | doStatement | returnStatemnt
ParserInfo statement() { 

	if(!strcmp(token.lx, "var")) {
		printf("Variable declaration statement peeked\n");
		pi = varDeclarStatement();
	} else if(!strcmp(token.lx, "let")) {
		printf("Let statement peeked\n");
		pi = letStatement();
	} else if(!strcmp(token.lx, "if")) {
		printf("If statement peeked\n");
		pi = ifStatement();
	} else if(!strcmp(token.lx, "while")) {
		printf("While statement peeked\n");
		pi = whileStatement();
	} else if(!strcmp(token.lx, "do")) {
		printf("Do statement peeked\n");
		pi = doStatement();
	} else if (!strcmp(token.lx, "return")) {
		printf("return statement peeked\n");
		pi = returnStatement();
	}

	return pi;
}

// varDeclarStatement -> var type identifier { , identifier } ;
ParserInfo varDeclarStatement() { 

	token = GetNextToken();

	if(token.tp == RESWORD && !strcmp(token.lx, "var")){
		printf("Variable declaration eaten");
	} else {
		printf("Var expected, idk the error");
	}

	// Check if type
	token = PeekNextToken();
	if (isType()) {
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

// letStatemnt -> let identifier [ [ expression ] ] = expression ;
ParserInfo letStatement() {

	token = GetNextToken();

	if (token.tp == RESWORD && !strcmp(token.lx, "let")) {
		printf("Let identifier found\n");
	} else {
		printf("Let identifier not found, idk the error code\n");
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

	if (token.tp == SYMBOL && !strcmp(token.lx, "=")) {
		token = GetNextToken();
		printf("Equals peeked\n");
	} else if (token.tp == SYMBOL && !strcmp(token.lx, "[")) {
		token = GetNextToken();
		token = PeekNextToken();

		// call expression();  NOT FINISHED****

		if (isFactorOrOperand()) {
			printf("Expression peeked\n");
			pi = expression();
		} else {
			printf("Expression expected, idk the error code\n");
			return pi;
		}
		
		if (pi.er != none) {
			return pi;
		}

		token = GetNextToken();

		if (token.tp == SYMBOL && !strcmp(token.lx, "]")) {
			printf("Closing square bracket found\n");
		} else {
			printf("Expected closing square bracket\n");
			pi.er = closeBracketExpected;
			pi.tk = token;
		}

		token = GetNextToken();

		if (token.tp == SYMBOL && !strcmp(token.lx, "=")) {
			printf("Equals sign found\n");
		} else {
			printf("equals sign expected\n");
			pi.er = equalExpected;
			pi.tk = token;
		}

	} else {
		printf("Equals expected\n");
		pi.er = equalExpected;
		pi.tk = token;
	}

	if (pi.er != none) {
		return pi;
	}


	token = PeekNextToken();

	if (isFactorOrOperand()) {
		printf("Expression peeked\n");
		pi = expression();
	} else {
		printf("Expression expected, idk the error code\n");
		return pi;
	}

	if (pi.er != none) {
		return pi;
	}	

	// Eat Semi-colon
	token = GetNextToken();
	if(token.tp == SYMBOL && !strcmp(token.lx, ";")) {
		printf("Semi-colon found\n");
	} else {
		printf("Semi-colon expected\n");
		pi.er = semicolonExpected;
		pi.tk = token;
	}

	return pi;
}

// ifStatement -> if ( expression ) { {statement} } [else { {statement} }]
ParserInfo ifStatement(){

	token = GetNextToken();

	token = GetNextToken();

	if (token.tp == SYMBOL && !strcmp(token.lx, "(")) {
		printf("opening bracket found\n");
	} else {
		printf("opening bracket expected\n");
		pi.er = openParenExpected;
		pi.tk = token;
	}

	if (pi.er != none) {
		return pi;
	}

	token = PeekNextToken();

	if (isFactorOrOperand()) {
		printf("Expression peeked\n");
		pi = expression();
	} else {
		printf("Expression expected, idk the error code\n");
	}

	if (pi.er != none) {
		return pi;
	}

	token = GetNextToken();

	if (token.tp == SYMBOL && !strcmp(token.lx, ")")) {
		printf("Closing bracket found\n");
	} else {
		printf("Closing bracket expected\n");
		pi.er = closeParenExpected;
		pi.tk = token;
	}

	if (pi.er != none) {
		return pi;
	}

	token = GetNextToken();

	if (token.tp == SYMBOL && !strcmp(token.lx, "{")) {
		printf("opening curly brace found\n");
	} else {
		printf("opening curly brace expected\n");
		pi.er = openBraceExpected;
		pi.tk = token;
	}

	// token = PeekNextToken();

	// if (isStatement()) {
	// 	printf("Statement peeked\n");
	// 	pi = statement();
	// } else {
	// 	printf("Statement expected, idk the error code\n");
	// }

	token = PeekNextToken();


	while(isStatement()) {
		printf("Statement peeked");
		pi = statement();

		if (pi.er != none) {
			return pi;
		}

		token = PeekNextToken();
	} 


	token = GetNextToken();

	if (token.tp == SYMBOL && !strcmp(token.lx, "}")) {
		printf("Closing curly brace found\n");
	} else {
		printf("Closing curly brace expected\n");
		pi.er = closeBraceExpected;
		pi.tk = token;
	}


	token = PeekNextToken();

	if (token.tp == RESWORD && !strcmp(token.lx, "else")) {

		token = GetNextToken();

		token = GetNextToken();

		if (token.tp == SYMBOL && !strcmp(token.lx, "{")) {
			printf("opening curly brace found\n");
		} else {
			printf("opening curly brace expected\n");
			pi.er = openBraceExpected;
			pi.tk = token;
		}

		token = PeekNextToken();

		if (isStatement()) {
			printf("Statement peeked\n");
			pi = statement();
		} else {
			printf("Statement expected, idk the error code\n");
		}

		token = PeekNextToken();


		while(isStatement()) {
			printf("Statement peeked");
			pi = statement();

			if (pi.er != none) {
				return pi;
			}

			token = PeekNextToken();
		} 

		token = GetNextToken();

		if (token.tp == SYMBOL && !strcmp(token.lx, "}")) {
			printf("Closing curly brace found\n");
		} else {
			printf("Closing curly brace expected\n");
			pi.er = closeBraceExpected;
			pi.tk = token;
		}	

	}

	return pi;
}

// whileStatement -> while ( expression ) { {statement} }
ParserInfo whileStatement() {

	token = GetNextToken();

	token = GetNextToken();

	if (token.tp == SYMBOL && !strcmp(token.lx, "(")) {
		printf("Opening bracket found\n");
	} else {
		printf("Opening bracket expected");
		pi.er = openParenExpected;
		pi.tk = token;
	}

	token = PeekNextToken();

	if (isFactorOrOperand()) {
		printf("Expression peeked\n");
		pi = expression();
	} else {
		printf("Expression expected, idk the error code\n");
	}

	token = GetNextToken();

	if (token.tp == SYMBOL && !strcmp(token.lx, ")")) {
		printf("Closing bracket found\n");
	} else {
		printf("Closing bracket expected\n");
		pi.er = openBraceExpected;
		pi.tk = token;
	}

	token = GetNextToken();

	if (token.tp == SYMBOL && !strcmp(token.lx, "{")) {
		printf("opening curly brace found\n");
	} else {
		printf("opening curly brace expected\n");
		pi.er = openBraceExpected;
		pi.tk = token;
	}

	token = PeekNextToken();

	while(isStatement()) {
		printf("Statement peeked");
		pi = statement();

		if (pi.er != none) {
			return pi;
		}

		token = PeekNextToken();
	} 

	token = GetNextToken();

	if (token.tp == SYMBOL && !strcmp(token.lx, "}")) {
		printf("Closing curly brace found\n");
	} else {
		printf("Closing curly brace expected\n");
		pi.er = closeBraceExpected;
		pi.tk = token;
	}

	return pi;
}

// doStatement -> do subroutineCall ; 
ParserInfo doStatement(){


	token = GetNextToken();

	if (token.tp == RESWORD && !strcmp(token.lx, "do")) {
		printf("Do found\n");
	} else {
		printf("Do expected\n");
	}
	
	if(pi.er != none) {
		return pi;
	}
	
	token = PeekNextToken();

	if (token.tp == ID) {
		printf("Subroutinecall peeked\n");
		pi = subroutineCall();

	} else {
		printf("Identifier expected\n");
		pi.er = idExpected;
		pi.tk = token;
	}

	if(pi.er != none) {
		return pi;
	}
	
	token = GetNextToken();

	// Eat Semi-colon
	if(token.tp == SYMBOL && !strcmp(token.lx, ";")) {
		printf("Semi-colon found\n");
	} else {
		printf("Semi-colon expected\n");
		pi.er = semicolonExpected;
		pi.tk = token;
	}


	return pi;

}

// subroutineCall -> identifier [ . identifier ] ( expressionList )
ParserInfo subroutineCall(){
	
	token = GetNextToken();

	if (token.tp == ID) {
		printf("Identifier found\n");
	} else {
		printf("Identifier not found");
		pi.er = idExpected;
		pi.tk = token;
	}


	token = PeekNextToken();

	if (token.tp == SYMBOL && !strcmp(token.lx, ".")) {
		
		token = GetNextToken();

		token = PeekNextToken();

		if (token.tp == ID) {
			token = GetNextToken();
			printf("Identifier found\n");
		} else {
			printf("Identifier expected\n");
			pi.er = idExpected;
			pi.tk = token;
		}

		if (pi.er != none) {
			return pi;
		}

	}

	token = GetNextToken();

	if (token.tp == SYMBOL && !strcmp(token.lx, "(")) {
		printf("Opening bracket found\n");
	} else {
		printf("Opening bracket expected");
	}

	token = PeekNextToken();

	if (isFactorOrOperand()) {
		printf("Expression list peeked\n");
		pi = expressionList();
	} else {
		printf("Expression list expected, idk the error code\n");
	}

	token = GetNextToken();


	if (token.tp == SYMBOL && !strcmp(token.lx, ")")) {
		printf("Closing bracket found WEEEEE\n");
	} else {
		printf("Closing bracket expected \n");
	}

	return pi;
}

// returnStatemnt -> return [ expression ] 
ParserInfo returnStatement(){

	token = GetNextToken();

	token = PeekNextToken();

	if (isFactorOrOperand()) {
		printf("Expression peeked\n");
		pi = expression();
	} 

	token = GetNextToken();

	if(token.tp == SYMBOL && !strcmp(token.lx, ";")) {
		printf("Semi-colon found\n");
	} else {
		printf("Semi-colon expected\n");
		pi.er = semicolonExpected;
		pi.tk = token;
	}

	return pi;
}

// expressionList -> expression { , expression } | ε
ParserInfo expressionList() {

	// token = PeekNextToken();

	if (isFactorOrOperand()) {
		printf("Expression peeked\n");
		pi = expression();
	} else {
		printf("Expression expected, idk the error code\n");
		return pi;
	}

	if (pi.er != none) {
		return pi;
	}

	token = PeekNextToken();

	while (token.tp == SYMBOL && !strcmp(token.lx, ",")) {

		printf("expression list peeked !!!");
		token = GetNextToken();
		token = PeekNextToken();

		if (isFactorOrOperand()) {
			printf("Expression peeked");
			pi = expression();
		} else {
			printf("Expression expected, idk the error code");
			return pi;
		}
		token = PeekNextToken();
	}

	return pi;
}

// expression -> relationalExpression { ( & | | ) relationalExpression }
ParserInfo expression() {

	// token = PeekNextToken();

	if (isFactorOrOperand()) {
		printf("Relational expression peeked\n");
		pi = relationalExpression();
	} else {
		printf("Relational expression expected, idk the error code\n");
		return pi;
	}
	
	if(pi.er != none) {
		return pi;
	}
	
	token = PeekNextToken(); 

	while (token.tp == SYMBOL && (!strcmp(token.lx, "&") || !strcmp(token.lx, "|"))) {

		token = GetNextToken();
		token = PeekNextToken();

		if (isFactorOrOperand()) {
			printf("Relational expression peeked\n");
			pi = relationalExpression();
		} else {
			printf("Relational expression expected, idk the error code\n");
			return pi;
		}

		token = PeekNextToken();
	}

	return pi;
}

// relationalExpression -> ArithmeticExpression { ( = | > | < ) ArithmeticExpression }
ParserInfo relationalExpression() {

	// token = PeekNextToken();

	if (isFactorOrOperand()) {
		printf("arithmetic expression peeked\n");
		pi = arithmeticExpression();
	} else {
		printf("arithmetic expression expected, idk the error code\n");
	}

	if (pi.er != none) {
		return pi;
	}

	while (token.tp == SYMBOL && (!strcmp(token.lx, "=") || !strcmp(token.lx, ">")) || !strcmp(token.lx, "<")) {

		token = GetNextToken();
		token = PeekNextToken();

		if (isFactorOrOperand()) {
			printf("Arithmetic expression peeked\n");
			pi = arithmeticExpression();
		} else {
			printf("Arithmetic expression expected, idk the error code\n");
		}

		if (pi.er != none) { 
			return pi;
		}

		token = PeekNextToken();
	}

	return pi;
}

// ArithmeticExpression -> term { ( + | - ) term }
ParserInfo arithmeticExpression() {

	// token = PeekNextToken();

	if (isFactorOrOperand()) {
		printf("Term peeked = %s\n", token.lx);
		pi = term();
	} else {
		printf("Term expected, idk the error code");
	}
	
	if (pi.er != none) {
		return pi;
	}
	
	token = PeekNextToken();

	while (token.tp == SYMBOL && (!strcmp(token.lx, "+") || !strcmp(token.lx, "-"))) {

		token = GetNextToken();
		token = PeekNextToken();

		if (isFactorOrOperand()) {
			printf("Term peeked\n");
			pi = term();
		} else {
			printf("Term expected, idk the error code\n");
		}

		if (pi.er != none) { 
			return pi;
		}

		token = PeekNextToken();
	}

	return pi;
}

// term -> factor { ( * | / ) factor }
ParserInfo term() {
	
	// token = PeekNextToken();

	if (isFactorOrOperand()) {
		printf("Factor peeked = %s\n", token.lx);
		pi = factor();
	} else {
		printf("Factor expected, idk the error code\n");
	}
	
	if (pi.er != none) {
		return pi;
	}

	token = PeekNextToken();

	while (token.tp == SYMBOL && (!strcmp(token.lx, "*") || !strcmp(token.lx, "/"))) {

		token = GetNextToken();
		token = PeekNextToken();

		if (isFactorOrOperand()) {
			printf("Factor peeked\n");
			pi = factor();
		} else {
			printf("Factor expected, idk the error code\n");
		}

		if (pi.er != none) { 
			return pi;
		}

		token = PeekNextToken();
	}

	return pi;
}

// factor -> ( - | ~ | ε ) operand
ParserInfo factor() {

	if (token.tp == SYMBOL && !strcmp(token.lx, "-") || !strcmp(token.lx, "~")) {
		token = GetNextToken();
		printf("Symbol found");
	} 

	if (isFactorOrOperand()) {
		printf("Operand found = %s\n", token.lx);
		pi = operand();
	} else {
		printf("Operand expected, idk the error code\n");
	}

	return pi;
}

// operand -> integerConstant | identifier [.identifier ] [ [ expression ] | (expressionList) ] | (expression) | stringLiteral | true | false | null | this
ParserInfo operand() {
	
	// token = PeekNextToken();

	if (isIntegerConstant(token.lx)) { // Integer constant
		token = GetNextToken();
		printf("Integer constant found\n");

	} else if (token.tp == ID) { // identifier [.identifier ] [ [ expression ] | (expressionList ) ]
		

		printf("Identifier peeked = %s\n", token.lx);

		token = GetNextToken();

		printf("Identifier found = %s\n", token.lx);

		token = PeekNextToken();
		if (token.tp == SYMBOL && !strcmp(token.lx, ".")) {
			
			token = GetNextToken();
			printf("Decimal point found\n");

			token = GetNextToken();
			if (token.tp == ID) {
				printf("Identifier found\n");
			} else {
				printf("Identifier expected\n");
				pi.er = idExpected;
				pi.tk = token;
			}

			if (pi.er != none) {
				return pi;
			}

			token = PeekNextToken();
		} 

		if (token.tp == SYMBOL && !strcmp(token.lx, "[")) {
			token = GetNextToken();
			printf("Opening square bracket found\n");

			token = PeekNextToken();
			if (isFactorOrOperand()) {
				printf("Expression peeked\n");
				pi = expression();
			} else {
				printf("Expression expected, idk the error code\n");
			}
			
			if(pi.er != none) {
				return pi;
			}

			token = GetNextToken();
			if (token.tp == SYMBOL && !strcmp(token.lx, "]")) {
				printf("Found closing square bracket\n");
			} else {
				printf("Closing square bracket expected");
				pi.er = closeBracketExpected;
				pi.tk = token;
			}
		} else if (token.tp == SYMBOL && !strcmp(token.lx, "(")) {
			
			token = GetNextToken();
			printf("Opening bracket found\n");

			token = PeekNextToken();
			if (isFactorOrOperand()) {
				printf("Expression peeked\n");
				pi = expressionList();
			} else {
				printf("Expression expected, idk the error code\n");
			}
			
			if(pi.er != none) {
				return pi;
			}

			token = GetNextToken();
			if (token.tp == SYMBOL && !strcmp(token.lx, ")")) {
				printf("Found closing bracket\n");
			} else {
				printf("Closing bracket expected");
				pi.er = closeParenExpected;
				pi.tk = token;
			}
		}
	} else if (token.tp == SYMBOL && !strcmp(token.lx, "(")) { // (expression)

		token = GetNextToken();
		token = PeekNextToken();

		if (isFactorOrOperand()) {
			printf("expression peeked\n");
			pi = expression();
		} else {
			printf("Expression expected, idk the error code\n");
		}

		if (pi.er != none) {
			return pi;
		}

		token = GetNextToken();

		if (token.tp == SYMBOL && !strcmp(token.lx, ")")) {
			printf("Closing bracket found");
		} else {
			printf("Closing bracket expected");
			pi.er = closeParenExpected;
			pi.tk = token;
		}
	} else if (token.tp == STRING) {

		token = GetNextToken();
		printf("String literal found");

	} else if (token.tp == RESWORD) {

		printf("Reserved word peeked\n");

		token = GetNextToken();

		if(!strcmp(token.lx, "true") || !strcmp(token.lx, "false") || !strcmp(token.lx, "this") || !strcmp(token.lx, "null")) {
			printf("Reserved word found = %s\n", token.lx);
		} else { 
			printf("Illegal reserved word, idk the error\n");
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
	InitParser("Main.jack");
	Parse();
	
	return 1;
}
#endif

/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
Lexer Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name: Joseph Gibson 
Student ID: 201619051
Email: sc22jg@leeds.ac.uk
Date Work Commenced: 08/02/2024
*************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "newLexer.h"
#include <stdbool.h>
#define NUMRESWORDS 21
#define MAXTOKENS 2000


// YOU CAN ADD YOUR OWN FUNCTIONS, DECLARATIONS AND VARIABLES HERE


const char* resWords[] = {"class", "constructor", "method", "function", /* Program components */
                          "int", "boolean", "char", "void", /* Primitive types */
                          "var", "static", "field", /* Variable declarations */
                          "let", "do", "if", "else", "while", "return", /* Statements */ 
                          "true", "false", "null", /* Constant values */
                          "this"}; /* Class Membership */
const char symbols[19] = {
    '(', ')', '[', ']', '{', '}',
    ',', ';', '=', '.',
    '+', '-', '*', '/', '&', '|', '<', '>', '~'
};

char* fileName;

FILE* file;
int tokenReady;
Token token;
int lineNumber;


int InitLexer(char* file_name) {


    file = fopen(file_name, "r");
    if(!file) {
        return 0;
    } 

    fileName = file_name;
  
  tokenReady = 0;
  lineNumber = 1;

  return 1;
}

bool isSymbol(char character) {
    int numSymbols = sizeof(symbols) / sizeof(symbols[0]);
    for(int i = 0; i < numSymbols; i++) {
        if (character == symbols[i]) { 
            return true;
        }
    }
    return false;
}

bool isReservedWord(char* string) {
  
  for(int i=0; i < NUMRESWORDS; i++) {
    if (strcmp(string, resWords[i]) == 0) {
      return true;
    }
  }
  return false;
}

int skipWhiteSpaceAndComment() {
    
    int character;
    character = getc(file);
    while (character != EOF && isspace(character)) {
        if (character == '\n') {
           lineNumber++;
        }
        character = getc(file);
    } // hit a nonspace character

    if (character == '/') {
        character = getc(file);
        if (character == '/') {
            // character = getc(file);
            // if (character == '\n') lineNumber++;
            while ( character != EOF && character != '\n') {
                character = getc(file);
                if (character == '\n')lineNumber++;
            }
            if (character == EOF) return character;
            character = getc(file);
            if (isspace(character) || character == '/') {
                ungetc(character, file);
                return skipWhiteSpaceAndComment();
            }
        } else if (character == '*') {
            // Multi-line comment
            int prev_char = 0;
            while (character != EOF) {
                // Check for the end of the comment
                if (prev_char == '*' && character == '/') {
                    // Read the next character after the comment
                    character = getc(file);
                    // Recursively call skipWhiteSpaceAndComment to handle consecutive comments
                    return skipWhiteSpaceAndComment();
                }
                if (character == '\n') {
                    lineNumber++;
                }
                // Update the previous character
                prev_char = character;
                // Read the next character
                character = getc(file);
            }
        }
    }
    // Return the character if it's not part of a comment
    return character;
}

Token generateToken() {
    
    strcpy(token.fl, fileName);
    token.tp = ERR;

    int character = skipWhiteSpaceAndComment();
    if (character == EOF) {
        token.tp = EOFile;
        token.ln = lineNumber;
        return token;
    }

    int i = 0;
    char tempCharacters[128];
    if (isalpha(character)) {

        while(character != EOF && isalpha(character)) {
            tempCharacters[i++] = character;
            character = getc(file);

        }
        tempCharacters[i] = '\0';
        character = ungetc(character, file);

        strcpy(token.lx, tempCharacters);
        if (isReservedWord(tempCharacters)) {
            token.tp = RESWORD;
        } else {
            token.tp = ID;
        }
        token.ln = lineNumber;
        return token;

    } else if (isdigit(character)) {
        while(character != EOF && isdigit(character)) {
            tempCharacters[i++] = character;
            character = getc(file);
        }
        tempCharacters[i] = '\0';
        character = ungetc(character, file);

        strcpy(token.lx, tempCharacters);
        token.tp = INT;
        token.ln = lineNumber;
        return token;        

    } else if (character == '"') {
        character = getc(file);
        while(character != EOF && (character != '"')) {
            tempCharacters[i++] = character;
            character = getc(file);
        }
        tempCharacters[i] = '\0';
        character = ungetc(character, file);

        strcpy(token.lx, tempCharacters);
        token.tp = STRING;
        token.ln = lineNumber;
        return token;

    } else if (isSymbol(character)) {
        tempCharacters[0] = character;
        tempCharacters[1] = '\0';
        strcpy(token.lx, tempCharacters);
        token.ln = lineNumber;
        return token;
    } else {
      return token;
    }
}

Token GetNextToken() { 

  if(tokenReady) {
    tokenReady = 0;
    return token;
  }
  Token generatedToken = generateToken();
  tokenReady = 0;
  return generatedToken;
}

Token PeekNextToken() { 

  if(tokenReady) {
    return token;
  }
  Token generatedToken = generateToken();
  tokenReady = 1;
  return generatedToken;
}

int StopLexer() {
    fclose(file);
    return 0;
}


// do not remove the next line
#ifndef TEST
int main () {
	// implement your main function here
  // NOTE: the autograder will not use your main function
  InitLexer("SquareGame.jack");
  
  Token nextToken =  GetNextToken();
  while (nextToken.tp != EOFile) {
  // for (int i=0; i< 14; i++) {
    printf("< %s, %d, %s, %u >\n", nextToken.fl, nextToken.ln, nextToken.lx, nextToken.tp );
    nextToken = GetNextToken();
  }
}
// do not remove the next line
#endif
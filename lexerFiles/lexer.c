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
#include "lexer.h"
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

int skipWhiteSpaceAndComment(Token token) {
    int character;
    character = getc(file);
    while (character != EOF && isspace(character)) {
        if (character == '\n') {
            lineNumber++;
        }
        character = getc(file);
    } // hit a non-space character

    if (character == '/') {
        character = getc(file);
        if (character == '/') {
            // Single-line comment
            while (character != EOF && character != '\n') {
                character = getc(file);
            }
            if (character == '\n') {
                lineNumber++;
            }
            if (character == EOF) {
                return character; // EOF encountered during single-line comment
            }
            // After a single-line comment, we continue to skip whitespace/comments
            return skipWhiteSpaceAndComment(token);
        } else if (character == '*') {
            // Multi-line comment
            int prev_char = 0;
            while (character != EOF) {
                character = getc(file); // Read the next character first to correctly handle characters after '*'
                // Check for the end of the comment
                if (prev_char == '*' && character == '/') {
                    character = getc(file); // Peek the next character after the closing '/'
                    if (character == '\n') {
                        lineNumber++; // Increment if the next character is a newline
                    } 
                    // Exit the comment and handle any following content
                    return skipWhiteSpaceAndComment(token);
                } else if (character == '\n') {
                    lineNumber++;
                }
                prev_char = character; // Update the previous character after processing the current character
            }
            if (character == EOF) {
                token.ec = EofInCom;
                token.tp = ERR;
                strcpy(token.lx, "Error: unexpected eof in comment");
                return character; // EOF encountered during multi-line comment
            }
        } else {
            // Not a comment, return the '/' and the next character to be processed
            ungetc(character, file); // Put back the character after '/'
            return '/'; // Return '/' to indicate it's not part of a comment
        }
    }
    // If the character is not a '/' or after processing comments, return the character
    return character;
}


Token generateToken() {
    
    strcpy(token.fl, fileName);
    token.tp = ERR;

    int character = skipWhiteSpaceAndComment(token);

    if (token.tp == ERR && token.ec == EofInCom) {
        // Error handling for unexpected EOF in comment
        // Since skipWhiteSpaceAndComment already set the token.lexeme, just return the token
        strcpy(token.lx, "Error: unexpected eof in comment");

        token.ln = lineNumber;
        return token;
    } else if (character == EOF) {
        // Handle normal EOF
        token.tp = EOFile;
        strcpy(token.lx, "End of File");
        token.ln = lineNumber;
        return token;
    }

    int i = 0;
    char tempCharacters[128];
    if (isalpha(character) || character == '_') {

        while(character != EOF && (isalnum(character) || character == '_')) {
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

            if(character == '\n') {
            token.ec = NewLnInStr;
            strcpy(token.lx, "Error: new line in string constant");
            token.tp = ERR;
        }
        }
        if(character == EOF) {
            token.ec = EofInStr;
            printf("test");
            strcpy(token.lx, "Error: unexpected eof in string constant");
            token.tp = ERR;
            return token;
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
        token.tp = SYMBOL;
        token.ln = lineNumber;
        return token;
    } else {
        token.ec = IllSym;
        token.ln = lineNumber;
        strcpy(token.lx, "Error: illegal symbol in source file");
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
  InitLexer("EofInComment.jack");
  
  Token nextToken =  GetNextToken();
  while (nextToken.tp != ERR) {
//   for (int i=0; i< 152; i++) {
    printf("< %s, %d, %s, %u >\n", nextToken.fl, nextToken.ln, nextToken.lx, nextToken.tp );
    nextToken = GetNextToken();
  }
  nextToken = GetNextToken();
  printf("< %s, %d, %s, %u >\n", nextToken.fl, nextToken.ln, nextToken.lx, nextToken.tp );
}
// do not remove the next line
#endif
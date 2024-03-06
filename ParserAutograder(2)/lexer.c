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
#define NUMSYMBOLS 19
#define NUMRESWORDS 21
#define MAXTOKENS 2000


// YOU CAN ADD YOUR OWN FUNCTIONS, DECLARATIONS AND VARIABLES HERE

// List of JACK reserved words
const char* resWords[] = {"class", "constructor", "method", "function", /* Program components */
                          "int", "boolean", "char", "void", /* Primitive types */
                          "var", "static", "field", /* Variable declarations */
                          "let", "do", "if", "else", "while", "return", /* Statements */ 
                          "true", "false", "null", /* Constant values */
                          "this"}; /* Class Membership */

// List of JACK symbols
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
    
    for(int i = 0; i < NUMSYMBOLS; i++) {
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

// Recursive function to skip to the next token
int skipWhiteSpaceAndComment(Token *token) {
    
    int character;
    character = getc(file);
    
    // Skip whitespace
    while (character != EOF && isspace(character)) {

        if (character == '\n') {

            lineNumber++;

        }

        character = getc(file);

    } 

    // Encountered a non-whitespace character
    if (character == '/') {

        character = getc(file);

        if (character == '/') { // Single-line comment

            while (character != EOF && character != '\n') {

                character = getc(file);

            }
            if (character == '\n') {

                lineNumber++;

            }
            if (character == EOF) {

                return character; 

            }
            
            //continue to skip
            return skipWhiteSpaceAndComment(token);

        } else if (character == '*') { // Multi-line comment
           
            int prev_char = 0;

            while (character != EOF) { // Skip past characters in comment

                character = getc(file); 
                
                if (prev_char == '*' && character == '/') { // End of comment reached

                    character = getc(file); // Peek at the character after end of comment
                    if (character == '\n') {

                        lineNumber++; 
                        return skipWhiteSpaceAndComment(token);

                    } else {

                        ungetc(character, file);
                        return skipWhiteSpaceAndComment(token);

                    }

                } else if (character == '\n') {

                    lineNumber++;

                }

                prev_char = character; // Update the previous character
            }
            if (character == EOF) {

                token->ec = EofInCom;
                strcpy(token->lx, "Error: unexpected eof in comment");
                return character; // EOF encountered during multi-line comment

            }
        
        } else { 
            
            ungetc(character, file); 
            return '/'; 

        }
    }

    // Non-whitespace or comment character encountered 
    return character;
}

Token generateToken() {
    
    // Initial token 
    strcpy(token.fl, fileName);
    token.tp = ERR;
    token.ec = 50;

    int character = skipWhiteSpaceAndComment(&token);
    
    if (token.ec == EofInCom) { // Error EOF
        
        token.tp = ERR;
        strcpy(token.lx, "Error: unexpected eof in comment");
        token.ln = lineNumber;
        return token;

    } else if (character == EOF) { // Normal EOF 

        token.tp = EOFile;
        strcpy(token.lx, "End of File");
        token.ln = lineNumber;
        return token;

    }

    int i = 0;
    char tempCharacters[128]; 

    if (isalpha(character) || character == '_') { // Identifier or Reserved token 

        while(character != EOF && (isalnum(character) || character == '_')) { // Identifiers can contain digits and _

            tempCharacters[i++] = character;
            character = getc(file);

        }
        character = ungetc(character, file);    
        
        tempCharacters[i] = '\0';
        strcpy(token.lx, tempCharacters);

        if (isReservedWord(tempCharacters)) { // Distinguish between Resword or ID 
        
            token.tp = RESWORD;

        } else {

            token.tp = ID;

        }

        token.ln = lineNumber;
        return token;

    } else if (isdigit(character)) { // INT token
        
        while(character != EOF && isdigit(character)) {
            
            tempCharacters[i++] = character;
            character = getc(file);

        }

        tempCharacters[i] = '\0';
        strcpy(token.lx, tempCharacters);

        character = ungetc(character, file);

        token.tp = INT;
        token.ln = lineNumber;
        return token;        

    } else if (character == '"') { // String literal token
        
        character = getc(file);

        while(character != EOF && (character != '"')) {

            tempCharacters[i++] = character;
            character = getc(file);

            if(character == '\n') {

                token.ec = NewLnInStr;
                strcpy(token.lx, "Error: new line in string constant");
                token.ln = lineNumber;
                token.tp = ERR;
                return token;

            }
            if(character == EOF) {

                token.ec = EofInStr;
                strcpy(token.lx, "Error: unexpected eof in string constant");
                token.ln = lineNumber;
                token.tp = ERR;
                return token;

            }
        }

        tempCharacters[i] = '\0';

        strcpy(token.lx, tempCharacters);
        token.tp = STRING;
        token.ln = lineNumber;
        return token;

    } else if (isSymbol(character)) { //Symbol token

        tempCharacters[0] = character;
        tempCharacters[1] = '\0';
        strcpy(token.lx, tempCharacters);
        token.tp = SYMBOL;
        token.ln = lineNumber;
        return token;

    } else { // Illegal Symbol token
        
        strcpy(token.lx, "Error: illegal symbol in source file");
        token.ec = IllSym;
        token.ln = lineNumber;
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
// #ifndef TEST
// int main () {
// 	// implement your main function here
//   // NOTE: the autograder will not use your main function
//   InitLexer("Ball.jack");
  
//   Token nextToken =  GetNextToken();
//   while (nextToken.tp != EOFile) {
//     printf("< %s, %d, %s, %u >\n", nextToken.fl, nextToken.ln, nextToken.lx, nextToken.tp );
//     nextToken = GetNextToken();
//   }
//   nextToken = GetNextToken();
//   printf("< %s, %d, %s, %u >\n", nextToken.fl, nextToken.ln, nextToken.lx, nextToken.tp );
// }
// // do not remove the next line
// #endif
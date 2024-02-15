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

// YOU CAN ADD YOUR OWN FUNCTIONS, DECLARATIONS AND VARIABLES HERE


char *buffer;
Token tokens[1000];
int tokenIndex = 0;
int lineNumber;

int currentPosition = 0;
const char* resWords[] = {"class", "constructor", "method", "function", /* Program components */
                          "int", "boolean", "char", "void", /* Primitive types */
                          "var", "static", "field", /* Variable declarations */
                          "let", "do", "if", "else", "while", "return", /* Statements */ 
                          "true", "false", "null", /* Constant values */
                          "this"}; /* Class Membership */

char symbols[18] = {
    '(', ')', '[', ']', '{', '}',
    ',', ';', '=', '.',
    '+', '-', '*', '/', '&', '|', '<', '>'
};

//function to check next character without incrementing the original filePointer
char peekNextCharacter(char *filePointer) {
  char *temporaryFilePointer;
  temporaryFilePointer = filePointer;

  temporaryFilePointer++;

  return *temporaryFilePointer;
}

char* skipWhiteSpaceAndComment(char *filePointer) {
    if (!filePointer) {
        return NULL;
    }

    while (*filePointer != '\0') {
        if (*filePointer == '\n') {
            lineNumber++;
            filePointer++;
        } else if (isspace((unsigned char)*filePointer)) {
          
            filePointer++;
        } else if (*filePointer == '/' && peekNextCharacter(filePointer) == '/') {
            while (*filePointer != '\n' && *filePointer != '\0') {
                filePointer++;
            }
        } else if (*filePointer == '/' && peekNextCharacter(filePointer) == '*') {
            filePointer += 2; // Skip the initial /*
            while (*filePointer != '\0' && !(*filePointer == '*' && peekNextCharacter(filePointer) == '/')) {
                if (*filePointer == '\n') {
                    lineNumber++;
                }
                filePointer++;
            }
            if (*filePointer != '\0') {
                filePointer += 2; // Skip past */
            }
        } else {
          break;
        }
    }
    return filePointer;
}


bool isReservedWord(const char* token) {
  int numResWords = sizeof(resWords) / sizeof(resWords[0]);
  for(int i=0; i < numResWords; i++) {
    if (strcmp(token, resWords[i]) == 0) {
      return true;
    }
  }
  return false;
}

bool isSymbol(char character) {
    int numSymbols = sizeof(symbols) / sizeof(symbols[0]);
    for(int i = 0; i < numSymbols; i++) {
        if (character == symbols[i]) { // Correctly dereference and compare
            printf("matched symbol: %c", symbols[i]);
            return true;
        }
    }
    printf("didnt match symbol");
    return false;
}

// IMPLEMENT THE FOLLOWING functions
//***********************************

// Initialise the lexer to read from source file
// file_name is the name of the source file
// This requires opening the file and making any necessary initialisations of the lexer
// If an error occurs, the function should return 0
// if everything goes well the function should return 1
int InitLexer (char* file_name) {
  FILE* file = fopen(file_name, "r");

  // strcpy(globalFileName, file_name);

  if (!file) {
    printf("File open error");
    return 0;
  }
  
  //determine size of file
  fseek(file, 0, SEEK_END);
  long sizeOfFile = ftell(file);
  fseek(file, 0, SEEK_SET);

  //allocate memory for buffer allowing space for null terminator
  buffer = (char *)malloc(sizeOfFile + 1);

  int bytes_read = fread(buffer, 1 , sizeOfFile, file);
  if(bytes_read != sizeOfFile) {
    return 0;
  }

  buffer[sizeOfFile] = '\0';
  
  char *filePointer;
  filePointer = buffer + currentPosition;

  lineNumber = 1;

  
  while (*filePointer != '\0') {


    Token t;
    strcpy(t.fl, file_name);
    filePointer = skipWhiteSpaceAndComment(filePointer);
    t.ln = lineNumber; // Save the current line number
   
    if (*filePointer == '\0') { // check if EOF
      t.tp = EOFile;
      tokens[tokenIndex++] = t;
      break;

    } else if (*filePointer == '"') { // check if string literal 

      filePointer++;
      char *startOfStringLiteral = filePointer; // point to the first element of string

      while (*filePointer != '"' && *filePointer != '\0') {
        filePointer++;
      } 
      if (*filePointer == '"') {
        
        t.tp = STRING;
        int sizeOfStringLiteral = filePointer - startOfStringLiteral;
        strncpy(t.lx, startOfStringLiteral, sizeOfStringLiteral);
        t.lx[sizeOfStringLiteral] = '\0';
        tokens[tokenIndex++] = t;

      }
      
      filePointer++;

    } else if (isSymbol(*filePointer)) { //since digit is neither a letter nor digit, it is a symbol
    // } else {
        t.tp = SYMBOL;
        printf("filepointer = %c", *filePointer);
        printf(" poo : %c", t.lx[0]);

        t.lx[0] = *filePointer;

        t.lx[1] = '\0';
        tokens[tokenIndex++] = t;
        filePointer++;

    } else if (isalpha(*filePointer)) { // check to see if token is a string/character
      char *startOfString = filePointer; //point to first element of string
      
      while (isalpha(*filePointer) && *filePointer != '\0') {
        filePointer++;
      }

      int sizeOfString = filePointer - startOfString;
      strncpy(t.lx, startOfString, sizeOfString);
      t.lx[sizeOfString] = '\0';

      // filePointer++;

      if (isReservedWord(t.lx)) {
        t.tp = RESWORD;

      } else {
        t.tp = ID;
      }
      tokens[tokenIndex++] = t;

    } else if (isdigit(*filePointer)) { // check to see if token is a number
      char *startOfNumber = filePointer; //point to first element of number
      
      while (isdigit(*filePointer) && *filePointer != '\0') {
        filePointer++;
      }

      int sizeOfNumber = filePointer - startOfNumber;
      strncpy(t.lx, startOfNumber, sizeOfNumber);
      t.lx[sizeOfNumber] = '\0';
      t.tp = INT;
      tokens[tokenIndex++] = t;

      // filePointer++;
     
    } else {
      
      filePointer++;
    }
  }
  

  // currentPosition = filePointer - buffer;
  currentPosition = 0;
  fclose(file);
  return 1;
}


// Get the next token from the source file
Token GetNextToken () {
	static int currentIndex = 0; // Static variable to keep track of the current index
  return tokens[currentIndex++];
}

// peek (look) at the next token in the source file without removing it from the stream
Token PeekNextToken () {
  Token t;
  t.tp = ERR;
  return t;
}

// clean out at end, e.g. close files, free memory, ... etc
int StopLexer () {

  // fclose(file);
  // free(buffer);
  // buffer = NULL;
	return 0;
}

// do not remove the next line
#ifndef TEST
int main () {
  printf("hello");
	// implement your main function here
  // NOTE: the autograder will not use your main function
  InitLexer("whiteSpaceFile.txt");
  Token nextToken =  GetNextToken();
  while (nextToken.tp != EOFile) {
    // printf("< %s, %d , %s, %u\n", nextToken.fl, nextToken.ln, nextToken.lx, nextToken.tp );
    nextToken = GetNextToken();
  }
  free(buffer);
  buffer = NULL;
  // printf("line number = %d\n", lineNumber);
}
// do not remove the next line
#endif

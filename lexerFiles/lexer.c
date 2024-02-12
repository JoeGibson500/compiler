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


// YOU CAN ADD YOUR OWN FUNCTIONS, DECLARATIONS AND VARIABLES HERE


char *buffer;

//function to check next character without incrementing the original filePointer
char peekNextCharacter(char *filePointer) {
  char *temporaryFilePointer;
  temporaryFilePointer = filePointer;

  temporaryFilePointer++;

  return *temporaryFilePointer;
}

//function to remove whitespace in the buffer
char* skipWhitespaceAndComment(char *filePointer) {
    if(!filePointer) { 
    
      return 0;
    }

    while (*filePointer != '\0') { // Check we are not at end of buffer
    
      
      if (isspace((unsigned char)*filePointer)) { //remove whitespace 
        filePointer++;
      }
      else if (*filePointer == '/' && peekNextCharacter(filePointer) == '/') { //skip past comment
        filePointer += 2;
        while (*filePointer != '\n' && *filePointer != '\0') {
          filePointer++;
        }
        if (*filePointer == '\n') {
          filePointer++; // move to next line 
        }
      }
      else {
        // we have reached a non comment and non whitespace character 
        break;
      }
    }
    return filePointer;
}


// IMPLEMENT THE FOLLOWING functions
//***********************************

// Initialise the lexer to read from source file
// file_name is the name of the source file
// This requires opening the file and making any necessary initialisations of the lexer
// If an error occurs, the function should return 0
// if everything goes well the function should return 1
int InitLexer (char* file_name) {
  FILE *file = fopen(file_name, "r");

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

  fclose(file);
  return 1;
}


// Get the next token from the source file
Token GetNextToken () {
	Token t;
  t.tp = ERR;

  char *filePointer;
  filePointer = buffer;
  
  filePointer = skipWhitespaceAndComment(filePointer);
    
  if (*filePointer == '\0') {
    
    t.tp = EOFile;
    return t;

  } else if (*filePointer == '"') {
    
    filePointer++;
    char *startOfString = filePointer; // point to the first element of string

    while (*filePointer != '"' && *filePointer != '\0') {
      filePointer++;
    } 
    if (*filePointer == '"') {
      
      t.tp = STRING;
      int sizeOfString = filePointer - startOfString;
      strncpy(t.lx, startOfString, sizeOfString);
      t.lx[sizeOfString] = '\0';
    }
    
    filePointer++;

  } else {
    
    t.tp = ERR;
    t.ec = EofInStr;

  }
  

  return t; 


  /*

  (DONE)C = (//) -> skip all the characters in the body of the comment and go back to 1 
  
  (DONE)C = (-1) -> return EOF token 

  C = (") -> keep reading more characters and store them into a string, until you hit another "
             put the resulting string (lexeme) into a token, of type string_ literal, and return the token.

  C = (letter) -> Keep reading more letters and/or digits, putting the into a string until a character that is
                  neither a letter nor a digit. 
                  Put the string (lexeme) in a token with a proper token type ( keyword or id) and return the token.

  C = (digit) -> Keep reading more digits, putting them into a string, until you hit a character that is not a digit.
                 Put the resulting in a token of type number, and return the token.
  C = (Symbol) -> Since is it not a letter or digit, tokenize it and return the token.  
  
  */
}

// peek (look) at the next token in the source file without removing it from the stream
Token PeekNextToken () {
  Token t;
  t.tp = ERR;
  return t;
}

// clean out at end, e.g. close files, free memory, ... etc
int StopLexer () {
	return 0;
}

// do not remove the next line
#ifndef TEST
int main () {
	// implement your main function here
  // NOTE: the autograder will not use your main function
  InitLexer("stringLiteralFile.txt");
  
  Token nextToken =  GetNextToken();

  if(nextToken.tp == EOFile) {
    printf("token type = EOFile\n");
  } else if (nextToken.tp == STRING) {
    printf("token type = string, '%s' ",nextToken.lx);
  }
  else {
    printf("token type = ERR\n");
    
  }

  
	return 0;
}
// do not remove the next line
#endif
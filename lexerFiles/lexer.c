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

//function to remove whitespace in the buffer
int removeWhiteSpace(char *filePointer) 
{
    if(!filePointer)
    {
      return 0;
    }

    while (isspace(buffer[*filePointer]))
    {
      filePointer++;
    }

    return 1;
}

char peekNextCharacter(char *filePointer)
{
  char *temporaryFilePointer;
  temporaryFilePointer = filePointer;

  temporaryFilePointer++;

  return *temporaryFilePointer;
}


// IMPLEMENT THE FOLLOWING functions
//***********************************

// Initialise the lexer to read from source file
// file_name is the name of the source file
// This requires opening the file and making any necessary initialisations of the lexer
// If an error occurs, the function should return 0
// if everything goes well the function should return 1
int InitLexer (char* file_name)
{
  FILE *file = fopen(file_name, "r");

  if (!file) 
  {
    printf("File open error");
    return 0;
  }
  
  //determine size of file
  fseek(file, 0, SEEK_END);
  long size_of_file = ftell(file);
  fseek(file, 0, SEEK_SET);

  //allocate memory for buffer allowing space for null terminator
  buffer = (char *)malloc(size_of_file + 1);

  int bytes_read = fread(buffer, 1 , size_of_file, file);
  if(bytes_read != size_of_file)
  {
    return 0;
  }

  fclose(file);
  return 1;
}


// Get the next token from the source file
Token GetNextToken ()
{
	Token t;
  t.tp = ERR;

  char *filePointer;
  filePointer = buffer;
  //1.skip any whitespace characters until we hit a non-whitespace character C
  if (removeWhiteSpace(filePointer) == 0)
  {
    printf("File read error");
  }
  else {
    
    printf("%c", *filePointer);
    // C = (//) -> skip all the characters in the body of the comment and go back to 1
    if (*filePointer == '/')
    {
      char nextCharacter = peekNextCharacter(filePointer);
      
      if (nextCharacter == '/')
      {
        while (*filePointer != '\n')
        {
          
          (*filePointer)++;
        }
      }
    }

  }

 
  

  return t; 


  /*

  C = (//) -> skip all the characters in the body of the comment and go back to 1

  C = (-1) -> return EOF token 

  C = (") -> keep reading more characters and store them into a string, until you hit another ""
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
Token PeekNextToken ()
{
  Token t;
  t.tp = ERR;
  return t;
}

// clean out at end, e.g. close files, free memory, ... etc
int StopLexer ()
{
	return 0;
}

// do not remove the next line
#ifndef TEST
int main ()
{
	// implement your main function here
  // NOTE: the autograder will not use your main function
  InitLexer("whiteSpaceFile.txt");
  GetNextToken();
	return 0;
}
// do not remove the next line
#endif
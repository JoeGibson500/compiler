/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
The Compiler Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name: Joseph Gibson
Student ID: 201619051
Email: sc22jg@leeds.ac.uk
Date Work Commenced: 22/03/2024
*************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <dirent.h>

#include "compiler.h"
#include "symbols.h"


int InitCompiler ()
{
	// InitProgramTable();
	return 1;
}

ParserInfo compile(char* dir_name) {

    InitProgramTable();
    // printf("Directory name = %s, symbol table should be empty : \n", dir_name);
    // PrintSymbols();

    ParserInfo p;
    p.er = none;

    DIR *dir;
    struct dirent *entry;

    dir = opendir(dir_name);
    if (dir == NULL) {
        printf("Failed to open directory");
        return p;
    }

    // First Pass: Build symbol tables
	// printf("FIRST PHASE\n");

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            char file_path[1024];
            sprintf(file_path, "%s/%s1", dir_name, entry->d_name);
			// printf("filepath = %s\n", file_path);
            InitParser(file_path);
            p = Parse();  // First phase
            if (p.er != none) {
                closedir(dir);
                return p;
            }
            StopParser();
        }
    }

    if (p.er != none) {
        closedir(dir);
        return p;
    }

    // PrintSymbols();

    // Reset the directory stream to re-read the files
    rewinddir(dir);

    // Second Pass: Semantic checks
	// printf("SECOND PHASE\n");
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            char file_path[1024];
            sprintf(file_path, "%s/%s2", dir_name, entry->d_name);
            // printf("filepath = %s\n", file_path);
            InitParser(file_path);
            p = Parse();  // Second phase
            if (p.er != none) {
                closedir(dir);
                return p;
            }
            StopParser();
        }
    }

    closedir(dir);
    return p;
}


int StopCompiler ()
{
	return 1;
}


#ifndef TEST_COMPILER
int main ()
{
	InitCompiler ();
	ParserInfo p = compile ("UNDECLAR_EXT_METH");
    // printf("Parser error = %u", p.er);

	// PrintError (p);
	StopCompiler ();
	return 1;
}
#endif

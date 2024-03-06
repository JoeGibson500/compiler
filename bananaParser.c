#include "lexer.h"
#include "parser.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>



void stmt();
void declar();
void type();
void assign();
void expr();
void term();
void factor();
void print();
void loop();
void error(const char*, Token);


void error(const char* msg, Token t)
{
	printf("Line %i at or near %s: %s\n", t.ln, t.x, msg);
	exit(0);
}



void prog()
{
	Token t = GetToken();
	if (t.t == key && !strcmp(t.x, "bello"))
		;
	else
		error("bello expected",t);
	t = PeekToken();
	while (t.t != eof && strcmp (t.x , "bank" ))
	{
		stmt();
		t = PeekToken();
	}
	t = GetToken();
	if (t.t == key && !strcmp(t.x, "bank"))
		;
	else
		error("bank expected",t);
	t = GetToken();
	if (t.t == key && !strcmp(t.x, "yu"))
		;
	else
		error("yu expected",t);
	t = GetToken();
	if (t.t == eof )
		;
	else
		error("eof expected",t);
	printf("TAMAM (OK in English)\n");
}

void stmt()
{
	Token t = PeekToken();
	if (!strcmp(t.x, "minion"))
		declar();
	else if (t.t == id)
		assign();
	else if (!strcmp(t.x, "para"))
		print();
	else if (!strcmp(t.x, "bee"))
		loop();
	else
		error("a keyword or id is expected",t);
}

void declar()
{
	Token t = GetToken();

	if (t.t == key && !strcmp(t.x, "minion"))
		;
	else
		error("minion expected",t);
	t = GetToken();
	if (t.t == id)
		;
	else
		error("id expected",t);
	t = GetToken();
	if (!strcmp (t.x , ":") )
		;
	else
		error(": expected",t);
}

void type()
{
	Token t = GetToken();
	if (!strcmp(t.x, "banana"))
		;
	else if (!strcmp(t.x, "gelato"))
		;
	else if (!strcmp(t.x, "bapple"))
		;
	else
		error("not a good type",t);
}

void assign()
{
	Token t = GetToken();
	if (t.t == id)
		;
	else
		error("id expected",t);
	t = GetToken();
	if (!strcmp (t.x , "@"))
		;
	else
		error("@ expected",t);
	expr();  // call the parser for the expr on the RHS
}

void expr()
{
	term();
	Token t = PeekToken();
	while (!strcmp(t.x,"+") || !strcmp(t.x, "-"))
	{ 
		// consume the operator
		t = GetToken(); // 
		term();
		t = PeekToken();
	}
}

void term()
{
	factor();
	Token t = PeekToken();
	while (!strcmp(t.x, "*") )
	{
		// consume the operator
		t = GetToken();
		factor();
		t = PeekToken();
	}
}

void factor()
{
	Token t = GetToken();
	if (t.t == id)
		;
	else if (t.t == num)
		;
	else
		error("id or num expected",t);
}

void print()
{
	Token t = GetToken();
	if (!strcmp(t.x, "para"))
		;
	else
		error("para expected",t);

	t = GetToken();
	if (!strcmp(t.x, "tu"))
		;
	else
		error("tu expected",t);

	expr();
}

void loop()
{
	Token t = GetToken();
	if (!strcmp(t.x, "bee"))
		;
	else
		error("bee expected",t);

	t = GetToken();
	if (!strcmp(t.x, "do"))
		;
	else
		error("do expected",t);
	expr();
	t = PeekToken();
	while (t.t != eof && strcmp(t.x, "stopa"))
	{
		stmt();
		t = PeekToken();
	}
	t = GetToken();
	if (!strcmp(t.x, "stopa"))
		;
	else
		error("stopa expected",t);
}